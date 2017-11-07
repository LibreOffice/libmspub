/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <algorithm>
#include <string.h>

#include <boost/numeric/conversion/cast.hpp>

#include <librevenge-stream/librevenge-stream.h>
#include <zlib.h>

#include "MSPUBMetaData.h"
#include "MSPUBParser.h"
#include "MSPUBCollector.h"
#include "MSPUBBlockID.h"
#include "MSPUBBlockType.h"
#include "MSPUBContentChunkType.h"
#include "MSPUBConstants.h"
#include "EscherContainerType.h"
#include "EscherFieldIds.h"
#include "libmspub_utils.h"
#include "ShapeType.h"
#include "ShapeFlags.h"
#include "Fill.h"
#include "FillType.h"
#include "ListInfo.h"
#include "Dash.h"
#include "TableInfo.h"
#include "VerticalAlign.h"

namespace libmspub
{

MSPUBParser::MSPUBParser(librevenge::RVNGInputStream *input, MSPUBCollector *collector)
  : m_input(input),
    m_length(boost::numeric_cast<unsigned>(getLength(input))),
    m_collector(collector),
    m_blockInfo(), m_contentChunks(),
    m_cellsChunkIndices(),
    m_pageChunkIndices(), m_shapeChunkIndices(),
    m_paletteChunkIndices(), m_borderArtChunkIndices(),
    m_fontChunkIndices(),
    m_unknownChunkIndices(), m_documentChunkIndex(),
    m_lastSeenSeqNum(-1), m_lastAddedImage(0),
    m_alternateShapeSeqNums(), m_escherDelayIndices()
{
}

MSPUBParser::~MSPUBParser()
{
}

bool MSPUBParser::lineExistsByFlagPointer(unsigned *flags,
                                          unsigned *geomFlags)
{
  return flags &&
         !(((*flags) & FLAG_USE_LINE) && !((*flags) & FLAG_LINE)) &&
         ((!geomFlags) || !((*geomFlags) & FLAG_GEOM_USE_LINE_OK)
          || ((*geomFlags) & FLAG_GEOM_LINE_OK));

}

unsigned MSPUBParser::getColorIndexByQuillEntry(unsigned entry)
{
  return entry;
}

short MSPUBParser::getBlockDataLength(unsigned type) // -1 for variable-length block with the data length as the first DWORD
{
  switch (type)
  {
  case DUMMY:
  case 0x5:
  case 0x8:
  case 0xa:
    return 0;
  case 0x10:
  case 0x12:
  case 0x18:
  case 0x1a:
  case 0x07:
    return 2;
  case 0x20:
  case 0x22:
  case 0x58:
  case 0x68:
  case 0x70:
  case 0xb8:
    return 4;
  case 0x28:
    return 8;
  case 0x38:
    return 16;
  case 0x48:
    return 24;
  case STRING_CONTAINER:
  case 0x80:
  case 0x82:
  case GENERAL_CONTAINER:
  case 0x8a:
  case 0x90:
  case 0x98:
  case 0xa0:
    return -1;
  }
  //FIXME: Debug assertion here? Should never get here.
  MSPUB_DEBUG_MSG(("Block of unknown type seen!\n"));
  return 0;
}

bool MSPUBParser::parse()
{
  MSPUB_DEBUG_MSG(("***NOTE***: Where applicable, the meanings of block/chunk IDs and Types printed below may be found in:\n\t***MSPUBBlockType.h\n\t***MSPUBBlockID.h\n\t***MSPUBContentChunkType.h\n*****\n"));
  if (!m_input->isStructured())
    return false;
  // No check: metadata are not important enough to fail if they can't be parsed
  parseMetaData();
  std::unique_ptr<librevenge::RVNGInputStream> quill(m_input->getSubStreamByName("Quill/QuillSub/CONTENTS"));
  if (!quill)
  {
    MSPUB_DEBUG_MSG(("Couldn't get quill stream.\n"));
    return false;
  }
  if (!parseQuill(quill.get()))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse quill stream.\n"));
    return false;
  }
  std::unique_ptr<librevenge::RVNGInputStream> contents(m_input->getSubStreamByName("Contents"));
  if (!contents)
  {
    MSPUB_DEBUG_MSG(("Couldn't get contents stream.\n"));
    return false;
  }
  if (!parseContents(contents.get()))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse contents stream.\n"));
    return false;
  }
  std::unique_ptr<librevenge::RVNGInputStream> escherDelay(m_input->getSubStreamByName("Escher/EscherDelayStm"));
  if (escherDelay)
  {
    parseEscherDelay(escherDelay.get());
  }
  std::unique_ptr<librevenge::RVNGInputStream> escher(m_input->getSubStreamByName("Escher/EscherStm"));
  if (!escher)
  {
    MSPUB_DEBUG_MSG(("Couldn't get escher stream.\n"));
    return false;
  }
  if (!parseEscher(escher.get()))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse escher stream.\n"));
    return false;
  }

  return m_collector->go();
}

ImgType MSPUBParser::imgTypeByBlipType(unsigned short type)
{
  switch (type)
  {
  case OFFICE_ART_BLIP_PNG:
    return PNG;
  case OFFICE_ART_BLIP_JPEG:
    return JPEG;
  case OFFICE_ART_BLIP_JPEGCMYK:
    return JPEGCMYK;
  case OFFICE_ART_BLIP_WMF:
    return WMF;
  case OFFICE_ART_BLIP_DIB:
    return DIB;
  case OFFICE_ART_BLIP_EMF:
    return EMF;
  case OFFICE_ART_BLIP_TIFF:
    return TIFF;
  case OFFICE_ART_BLIP_PICT:
    return PICT;
  }
  return UNKNOWN;
}

int MSPUBParser::getStartOffset(ImgType type, unsigned short initial)
{
  bool oneUid = true;
  int offset = 0x11;
  unsigned short recInstance = initial >> 4;
  switch (type)
  {
  case WMF:
    oneUid = recInstance == 0x216;
    offset = 0x34;
    break;
  case EMF:
    oneUid = recInstance == 0x3D4;
    offset = 0x34;
    break;
  case PNG:
    oneUid = recInstance == 0x6E0;
    offset = 0x11;
    break;
  case JPEG:
    oneUid = recInstance == 0x46A || recInstance == 0x6E2;
    offset = 0x11;
    break;
  case JPEGCMYK:
    oneUid = recInstance == 0x46B || recInstance == 0x6E3;
    offset = 33;
    break;
  case DIB:
    oneUid = recInstance == 0x7A8;
    offset = 0x11;
    break;
  case TIFF:
    oneUid = recInstance == 0x6E4;
    offset = 0x11;
    break;
  default:
    break;
  }
  return offset + (oneUid ? 0 : 0x10);
}

bool MSPUBParser::parseEscherDelay(librevenge::RVNGInputStream *input)
{
  while (stillReading(input, (unsigned long)-1))
  {
    EscherContainerInfo info = parseEscherContainer(input);
    const ImgType imgType = imgTypeByBlipType(info.type);
    if (imgType != UNKNOWN)
    {
      librevenge::RVNGBinaryData img;
      unsigned long toRead = info.contentsLength;
      input->seek(input->tell() + getStartOffset(imgType, info.initial), librevenge::RVNG_SEEK_SET);
      while (toRead > 0 && stillReading(input, (unsigned long)-1))
      {
        unsigned long howManyRead = 0;
        const unsigned char *buf = input->read(toRead, howManyRead);
        img.append(buf, howManyRead);
        toRead -= howManyRead;
      }
      if (imgType == WMF || imgType == EMF)
      {
        img = inflateData(img);
      }
      else if (imgType == DIB)
      {
        // Reconstruct BMP header
        // cf. http://en.wikipedia.org/wiki/BMP_file_format , accessed 2012-5-31
        librevenge::RVNGInputStream *buf = img.getDataStream();
        if (img.size() < 0x2E + 4)
        {
          ++m_lastAddedImage;
          MSPUB_DEBUG_MSG(("Garbage DIB at index 0x%x\n", m_lastAddedImage));
          input->seek(info.contentsOffset + info.contentsLength, librevenge::RVNG_SEEK_SET);
          continue;
        }
        buf->seek(0x0E, librevenge::RVNG_SEEK_SET);
        unsigned short bitsPerPixel = readU16(buf);
        buf->seek(0x20, librevenge::RVNG_SEEK_SET);
        unsigned numPaletteColors = readU32(buf);
        if (numPaletteColors == 0 && bitsPerPixel <= 8)
        {
          numPaletteColors = 1;
          for (int i = 0; i < bitsPerPixel; ++i)
          {
            numPaletteColors *= 2;
          }
        }

        librevenge::RVNGBinaryData tmpImg;
        tmpImg.append((unsigned char)0x42);
        tmpImg.append((unsigned char)0x4d);

        tmpImg.append((unsigned char)((img.size() + 14) & 0x000000ff));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0x0000ff00) >> 8));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0x00ff0000) >> 16));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0xff000000) >> 24));

        tmpImg.append((unsigned char)0x00);
        tmpImg.append((unsigned char)0x00);
        tmpImg.append((unsigned char)0x00);
        tmpImg.append((unsigned char)0x00);

        tmpImg.append((unsigned char)(0x36 + 4 * numPaletteColors));
        tmpImg.append((unsigned char)0x00);
        tmpImg.append((unsigned char)0x00);
        tmpImg.append((unsigned char)0x00);
        tmpImg.append(img);
        img = tmpImg;
      }
      m_collector->addImage(++m_lastAddedImage, imgType, img);
    }
    else
    {
      ++m_lastAddedImage;
      MSPUB_DEBUG_MSG(("Image of unknown type at index 0x%x\n", m_lastAddedImage));
    }
    input->seek(info.contentsOffset + info.contentsLength, librevenge::RVNG_SEEK_SET);
  }
  return true;
}

bool MSPUBParser::parseContents(librevenge::RVNGInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseContents\n"));
  input->seek(0x1a, librevenge::RVNG_SEEK_SET);
  unsigned trailerOffset = readU32(input);
  MSPUB_DEBUG_MSG(("MSPUBParser: trailerOffset %.8x\n", trailerOffset));
  input->seek(trailerOffset, librevenge::RVNG_SEEK_SET);
  unsigned trailerLength = readU32(input);
  for (unsigned i=0; i<3; i++)
  {
    MSPUBBlockInfo trailerPart = parseBlock(input);
    MSPUB_DEBUG_MSG(("Trailer SubBlock %i, startPosition 0x%lx, id %i, type 0x%x, dataLength 0x%lx\n", i+1, trailerPart.startPosition, trailerPart.id, trailerPart.type, trailerPart.dataLength));
    if (trailerPart.type == TRAILER_DIRECTORY)
    {

      while (stillReading(input, trailerPart.dataOffset + trailerPart.dataLength))
      {
        m_blockInfo.push_back(parseBlock(input));
        ++m_lastSeenSeqNum;
        if (m_blockInfo.back().type == GENERAL_CONTAINER)
        {
          if (parseContentChunkReference(input, m_blockInfo.back()))
          {
            if (m_contentChunks.size() > 1)
            {
              m_contentChunks[m_contentChunks.size() - 2].end = m_contentChunks.back().offset;
            }
          }
        }
        else(skipBlock(input, m_blockInfo.back()));
      }
      if (!m_contentChunks.empty())
      {
        m_contentChunks.back().end = trailerPart.dataOffset + trailerPart.dataLength;
      }
      if (!m_documentChunkIndex)
      {
        return false;
      }
      const ContentChunkReference &documentChunk = m_contentChunks.at(m_documentChunkIndex.get());
      for (unsigned int paletteChunkIndex : m_paletteChunkIndices)
      {
        const ContentChunkReference &paletteChunk = m_contentChunks.at(paletteChunkIndex);
        input->seek(paletteChunk.offset, librevenge::RVNG_SEEK_SET);
        if (! parsePaletteChunk(input, paletteChunk))
        {
          return false;
        }
      }
      for (unsigned int borderArtChunkIndex : m_borderArtChunkIndices)
      {
        const ContentChunkReference &baChunk =
          m_contentChunks.at(borderArtChunkIndex);
        input->seek(baChunk.offset, librevenge::RVNG_SEEK_SET);
        if (!parseBorderArtChunk(input, baChunk))
        {
          return false;
        }
      }
      for (unsigned int shapeChunkIndex : m_shapeChunkIndices)
      {
        const ContentChunkReference &shapeChunk =
          m_contentChunks.at(shapeChunkIndex);
        input->seek(shapeChunk.offset, librevenge::RVNG_SEEK_SET);
        if (!parseShape(input, shapeChunk))
        {
          return false;
        }
      }
      for (unsigned int fontChunkIndex : m_fontChunkIndices)
      {
        const ContentChunkReference &fontChunk =
          m_contentChunks.at(fontChunkIndex);
        input->seek(fontChunk.offset, librevenge::RVNG_SEEK_SET);
        if (!parseFontChunk(input, fontChunk))
        {
          return false;
        }
      }
      input->seek(documentChunk.offset, librevenge::RVNG_SEEK_SET);
      if (!parseDocumentChunk(input, documentChunk))
      {
        return false;
      }
      for (unsigned int pageChunkIndex : m_pageChunkIndices)
      {
        const ContentChunkReference &pageChunk = m_contentChunks.at(pageChunkIndex);
        input->seek(pageChunk.offset, librevenge::RVNG_SEEK_SET);
        if (!parsePageChunk(input, pageChunk))
        {
          return false;
        }
      }
    }
  }
  input->seek(trailerOffset + trailerLength, librevenge::RVNG_SEEK_SET);

  return true;
}

#ifdef DEBUG
bool MSPUBParser::parseDocumentChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk)
#else
bool MSPUBParser::parseDocumentChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &)
#endif
{
  MSPUB_DEBUG_MSG(("parseDocumentChunk: offset 0x%lx, end 0x%lx\n", input->tell(), chunk.end));
  unsigned long begin = input->tell();
  unsigned long len = readU32(input);
  while (stillReading(input, begin + len))
  {
    MSPUBBlockInfo info = parseBlock(input);
    if (info.id == DOCUMENT_SIZE)
    {
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo subInfo = parseBlock(input, true);
        if (subInfo.id == DOCUMENT_WIDTH)
        {
          m_collector->setWidthInEmu(subInfo.data);
        }
        else if (subInfo.id == DOCUMENT_HEIGHT)
        {
          m_collector->setHeightInEmu(subInfo.data);
        }
      }
    }
    else if (info.id == DOCUMENT_PAGE_LIST)
    {
      input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo subInfo = parseBlock(input, true);
        if (subInfo.id == 0)
        {
          m_collector->setNextPage(subInfo.data);
        }
      }
    }
    else
    {
      skipBlock(input, info);
    }
  }
  return true; //FIXME: return false for failure
}

bool MSPUBParser::parseFontChunk(
  librevenge::RVNGInputStream *input, const ContentChunkReference &chunk)
{
  unsigned length = readU32(input);
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    if (info.id == FONT_CONTAINER_ARRAY)
    {
      input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo subInfo = parseBlock(input, true);
        if (subInfo.id == 0)
        {
          boost::optional<librevenge::RVNGString> name;
          boost::optional<unsigned> eotOffset;
          unsigned eotLength = 0;
          input->seek(subInfo.dataOffset + 4, librevenge::RVNG_SEEK_SET);
          while (stillReading(input, subInfo.dataOffset + subInfo.dataLength))
          {
            MSPUBBlockInfo subSubInfo = parseBlock(input, true);
            if (subSubInfo.id == EMBEDDED_FONT_NAME)
            {
              name = librevenge::RVNGString();
              // drop trailing 0
              // TODO: This could be a general problem. Check.
              const std::size_t len = subSubInfo.stringData.size();
              if ((len > 2) && (subSubInfo.stringData[len - 1] == 0) && (subSubInfo.stringData[len - 2] == 0))
              {
                subSubInfo.stringData.pop_back();
                subSubInfo.stringData.pop_back();
              }
              appendCharacters(name.get(), subSubInfo.stringData, "UTF-16LE");
            }
            else if (subSubInfo.id == EMBEDDED_EOT)
            {
              eotOffset = subSubInfo.dataOffset;
              eotLength = subSubInfo.dataLength;
            }
          }
          if (bool(name) && bool(eotOffset))
          {
            // skip length, we've already read that
            // TODO: Why do we not read the data as part of the block info?
            input->seek(eotOffset.get() + 4, librevenge::RVNG_SEEK_SET);
            librevenge::RVNGBinaryData data;
            unsigned long toRead = eotLength;
            while (toRead > 0 && stillReading(input, (unsigned long)-1))
            {
              unsigned long howManyRead = 0;
              const unsigned char *buf = input->read(toRead, howManyRead);
              data.append(buf, howManyRead);
              toRead -= howManyRead;
            }
            m_collector->addEOTFont(name.get(), data);
            input->seek(subInfo.dataOffset + subInfo.dataLength, librevenge::RVNG_SEEK_SET);
          }
        }
      }
    }
  }
  return true;
}

bool MSPUBParser::parseBorderArtChunk(
  librevenge::RVNGInputStream *input, const ContentChunkReference &chunk)
{
  unsigned length = readU32(input);
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    if (info.id == BA_ARRAY)
    {
      input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      unsigned i = 0;
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo entry = parseBlock(input, false);
        while (stillReading(input, entry.dataOffset + entry.dataLength))
        {
          MSPUBBlockInfo subRecord = parseBlock(input, true);
          if (subRecord.id == BA_IMAGE_ARRAY)
          {
            input->seek(subRecord.dataOffset + 4, librevenge::RVNG_SEEK_SET);
            while (stillReading(input, subRecord.dataOffset + subRecord.dataLength))
            {
              MSPUBBlockInfo subSubRecord = parseBlock(input, false);
              if (subSubRecord.id == BA_IMAGE_CONTAINER)
              {
                MSPUBBlockInfo imgRecord = parseBlock(input, false);
                if (imgRecord.id == BA_IMAGE)
                {
                  librevenge::RVNGBinaryData &img = *(m_collector->addBorderImage(
                                                        WMF, i));
                  unsigned long toRead = imgRecord.dataLength;
                  while (toRead > 0 && stillReading(input, (unsigned long)-1))
                  {
                    unsigned long howManyRead = 0;
                    const unsigned char *buf = input->read(toRead, howManyRead);
                    img.append(buf, howManyRead);
                    toRead -= howManyRead;
                  }
                }
              }
            }
          }
          else if (subRecord.id == BA_OFFSET_CONTAINER)
          {
            input->seek(subRecord.dataOffset + 4, librevenge::RVNG_SEEK_SET);
            while (stillReading(
                     input, subRecord.dataOffset + subRecord.dataLength))
            {
              MSPUBBlockInfo subSubRecord = parseBlock(input, true);
              if (subSubRecord.id == BA_OFFSET_ENTRY)
              {
                m_collector->setBorderImageOffset(i, subSubRecord.data);
              }
            }
          }
        }
        ++i;
        input->seek(entry.dataOffset + entry.dataLength, librevenge::RVNG_SEEK_SET);
      }
    }
  }
  return true;
}

bool MSPUBParser::parsePageChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk)
{
  MSPUB_DEBUG_MSG(("parsePageChunk: offset 0x%lx, end 0x%lx, seqnum 0x%x, parent 0x%x\n", input->tell(), chunk.end, chunk.seqNum, chunk.parentSeqNum));
  unsigned long length = readU32(input);
  PageType type = getPageTypeBySeqNum(chunk.seqNum);
  if (type == NORMAL)
  {
    m_collector->addPage(chunk.seqNum);
  }
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input);
    if (info.id == PAGE_BG_SHAPE)
    {
      m_collector->setPageBgShape(chunk.seqNum, info.data);
    }
    else if (info.id == PAGE_SHAPES)
    {
      parsePageShapeList(input, info, chunk.seqNum);
    }
    else if (info.id == THIS_MASTER_NAME)
    {
      for (unsigned char i : info.stringData)
      {
        if (i != 0)
        {
          m_collector->designateMasterPage(chunk.seqNum);
        }
      }
    }
    else if (info.id == APPLIED_MASTER_NAME)
    {
      m_collector->setMasterPage(chunk.seqNum, info.data);
    }
    else
    {
      skipBlock(input, info);
    }
  }
  return true;
}

bool MSPUBParser::parsePageShapeList(librevenge::RVNGInputStream *input, MSPUBBlockInfo info, unsigned pageSeqNum)
{
  MSPUB_DEBUG_MSG(("parsePageShapeList: page seqnum 0x%x\n", pageSeqNum));
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == SHAPE_SEQNUM)
    {
      m_collector->setShapePage(subInfo.data, pageSeqNum);
    }
  }
  return true;
}

bool MSPUBParser::parseShape(librevenge::RVNGInputStream *input,
                             const ContentChunkReference &chunk)
{
  MSPUB_DEBUG_MSG(("parseShape: seqNum 0x%x\n", chunk.seqNum));
  unsigned long pos = input->tell();
  unsigned length = readU32(input);
  unsigned width = 0;
  unsigned height = 0;
  bool isTable = chunk.type == TABLE;
  bool isGroup = chunk.type == GROUP || chunk.type == LOGO;
  if (isTable)
  {
    boost::optional<unsigned> cellsSeqNum;
    boost::optional<unsigned> numRows;
    boost::optional<unsigned> numCols;
    boost::optional<unsigned> rowcolArrayOffset;
    boost::optional<unsigned> textId;
    while (stillReading(input, pos + length))
    {
      MSPUBBlockInfo info = parseBlock(input, true);
      if (info.id == TABLE_WIDTH)
      {
        width = info.data;
      }
      else if (info.id == TABLE_HEIGHT)
      {
        height = info.data;
      }
      else if (info.id == TABLE_CELLS_SEQNUM)
      {
        cellsSeqNum = info.data;
      }
      else if (info.id == TABLE_NUM_ROWS)
      {
        numRows = info.data;
      }
      else if (info.id == TABLE_NUM_COLS)
      {
        numCols = info.data;
      }
      else if (info.id == TABLE_ROWCOL_ARRAY)
      {
        rowcolArrayOffset = info.dataOffset;
      }
      else if (info.id == SHAPE_TEXT_ID)
      {
        textId = info.data;
      }
    }
    if (bool(cellsSeqNum) && bool(numRows) && bool(numCols) && bool(rowcolArrayOffset))
    {
      unsigned nr = numRows.get();
      unsigned nc = numCols.get();
      unsigned rcao = rowcolArrayOffset.get();
      unsigned csn = cellsSeqNum.get();
      std::vector<unsigned> rowHeightsInEmu;
      std::vector<unsigned> columnWidthsInEmu;
      input->seek(rcao, librevenge::RVNG_SEEK_SET);
      unsigned arrayLength = readU32(input);
      while (stillReading(input, rcao + arrayLength))
      {
        MSPUBBlockInfo info = parseBlock(input, true);
        if (info.id == 0)
        {
          input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
          while (stillReading(input, info.dataOffset + info.dataLength))
          {
            MSPUBBlockInfo subInfo = parseBlock(input, true);
            if (subInfo.id == TABLE_ROWCOL_SIZE)
            {
              if (columnWidthsInEmu.size() < nc)
                columnWidthsInEmu.push_back(subInfo.data);
              else if (rowHeightsInEmu.size() < nr)
                rowHeightsInEmu.push_back(subInfo.data);
            }
          }
        }
      }
      if (rowHeightsInEmu.size() != nr || columnWidthsInEmu.size() != nc)
      {
        MSPUB_DEBUG_MSG(("ERROR: Wrong number of rows or columns found in table definition.\n"));
        return false;
      }
      boost::optional<unsigned> index;
      for (unsigned i = 0; i < m_cellsChunkIndices.size(); ++i)
      {
        if (m_contentChunks[m_cellsChunkIndices[i]].seqNum == csn)
        {
          index = i;
          break;
        }
      }

      TableInfo ti(nr, nc);
      ti.m_rowHeightsInEmu = rowHeightsInEmu;
      ti.m_columnWidthsInEmu = columnWidthsInEmu;

      if (!index)
      {
        MSPUB_DEBUG_MSG(("WARNING: Couldn't find cells of seqnum %u corresponding to table of seqnum %u.\n",
                         csn, chunk.seqNum));
        return false;
      }
      else
      {
        const ContentChunkReference &cellsChunk = m_contentChunks[m_cellsChunkIndices[get(index)]];
        input->seek(cellsChunk.offset, librevenge::RVNG_SEEK_SET);
        const unsigned cellsLength = readU32(input);
        boost::optional<unsigned> cellCount;
        while (stillReading(input, cellsChunk.offset + cellsLength))
        {
          MSPUBBlockInfo info = parseBlock(input, true);
          switch (info.id)
          {
          case 0x01:
            cellCount = info.data;
            break;
          case 0x02:
          {
            input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
            while (stillReading(input, info.dataOffset + info.dataLength))
            {
              const MSPUBBlockInfo itemInfo = parseBlock(input, true);
              if (itemInfo.id == 0)
              {
                input->seek(itemInfo.dataOffset + 4, librevenge::RVNG_SEEK_SET);
                CellInfo currentCell;
                while (stillReading(input, itemInfo.dataOffset + itemInfo.dataLength))
                {
                  const MSPUBBlockInfo subInfo = parseBlock(input, true);
                  switch (subInfo.id)
                  {
                  case 0x01:
                    currentCell.m_startRow = subInfo.data;
                    break;
                  case 0x02:
                    currentCell.m_endRow = subInfo.data;
                    break;
                  case 0x03:
                    currentCell.m_startColumn = subInfo.data;
                    break;
                  case 0x04:
                    currentCell.m_endColumn = subInfo.data;
                    break;
                  // TODO: 0x09 - 0x0e: width/height of content + margins?
                  default:
                    break;
                  }
                }
                ti.m_cells.push_back(currentCell);
              }
            }

            break;
          }
          default:
            break;
          }
        }

        if (bool(cellCount) && (get(cellCount) != ti.m_cells.size()))
        {
          MSPUB_DEBUG_MSG(("%u cell records expected, but read %u\n", get(cellCount), unsigned(ti.m_cells.size())));
        }
      }

      m_collector->setShapeTableInfo(chunk.seqNum, ti);
      if (bool(textId))
        m_collector->addTextShape(get(textId), chunk.seqNum);
      return true;
    }
    return false;
  }
  else
  {
    bool isText = false;
    bool shouldStretchBorderArt = true;
    unsigned textId = 0;
    while (stillReading(input, pos + length))
    {
      MSPUBBlockInfo info = parseBlock(input, true);
      if (info.id == SHAPE_WIDTH)
      {
        width = info.data;
      }
      else if (info.id == SHAPE_HEIGHT)
      {
        height = info.data;
      }
      else if (info.id == SHAPE_BORDER_IMAGE_ID)
      {
        m_collector->setShapeBorderImageId(chunk.seqNum, info.data);
      }
      else if (info.id == SHAPE_DONT_STRETCH_BA)
      {
        shouldStretchBorderArt = false;
      }
      else if (info.id == SHAPE_TEXT_ID)
      {
        textId = info.data;
        isText = true;
      }
      else if (info.id == SHAPE_VALIGN)
      {
        m_collector->setShapeVerticalTextAlign(chunk.seqNum,
                                               static_cast<VerticalAlign>(info.data));
      }
      else if (info.id == SHAPE_CROP && info.data != 0)
      {
        m_collector->setShapeCropType(chunk.seqNum,
                                      static_cast<ShapeType>(info.data));
      }
    }
    if (shouldStretchBorderArt)
    {
      m_collector->setShapeStretchBorderArt(chunk.seqNum);
    }
    bool parseWithoutDimensions = true; //FIXME: Should we ever ignore if height and width not given?
    if (isGroup || (height > 0 && width > 0) || parseWithoutDimensions)
    {
      if (! isGroup)
      {
        if (isText)
        {
          m_collector->addTextShape(textId, chunk.seqNum);
        }
      }
    }
    else
    {
      MSPUB_DEBUG_MSG(("Height and width not both specified, ignoring. (Height: 0x%x, Width: 0x%x)\n", height, width));
    }
    return true;
  }
}

QuillChunkReference MSPUBParser::parseQuillChunkReference(librevenge::RVNGInputStream *input)
{
  QuillChunkReference ret;
  readU16(input); //FIXME: Can we do something sensible if this is not 0x18 ?
  char name[5];
  for (int i = 0; i < 4; ++i)
  {
    name[i] = (char)readU8(input);
  }
  name[4] = '\0';
  ret.name = name;
  ret.id = readU16(input);
  input->seek(input->tell() + 4, librevenge::RVNG_SEEK_SET); //Seek past what is normally 0x01000000. We don't know what this represents.
  char name2[5];
  for (int i = 0; i < 4; ++i)
  {
    name2[i] = (char)readU8(input);
  }
  name2[4] = '\0';
  ret.name2 = name2;
  ret.offset = readU32(input);
  ret.length = readU32(input);
  return ret;
}

std::vector<unsigned> MSPUBParser::parseTableCellDefinitions(
  librevenge::RVNGInputStream *input, const QuillChunkReference &chunk)
{
  std::vector<unsigned> ret;
  unsigned numElements = readU32(input) + 1;
  input->seek(chunk.offset + 0xC, librevenge::RVNG_SEEK_SET);
  for (unsigned i = 0; i < numElements; ++i)
  {
    ret.push_back(readU32(input));
    // compensate for all but the last offset not including the terminating 0x0D00
    if (i != numElements - 1)
    {
      ret.back() += 2;
    }
  }
  return ret;
}

bool MSPUBParser::parseQuill(librevenge::RVNGInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseQuill\n"));
  unsigned chunkReferenceListOffset = 0x18;
  std::list<QuillChunkReference> chunkReferences;
  std::set<unsigned> readChunks; // guard against cycle in the chunk list
  while (chunkReferenceListOffset != 0xffffffff)
  {
    input->seek(chunkReferenceListOffset + 2, librevenge::RVNG_SEEK_SET);
    unsigned short numChunks = readU16(input);
    chunkReferenceListOffset = readU32(input);
    if (readChunks.find(chunkReferenceListOffset) != readChunks.end())
    {
      MSPUB_DEBUG_MSG(("Found a cycle in chunk reference list: a broken file!\n"));
      break;
    }
    readChunks.insert(chunkReferenceListOffset);
    for (unsigned i = 0; i < numChunks; ++i)
    {
      QuillChunkReference quillChunkReference = parseQuillChunkReference(input);
      chunkReferences.push_back(quillChunkReference);
    }
  }
  MSPUB_DEBUG_MSG(("Found %u Quill chunks\n", (unsigned)chunkReferences.size()));
  //Make sure we parse the STRS chunk before the TEXT chunk
  std::list<QuillChunkReference>::const_iterator textChunkReference = chunkReferences.end();
  bool parsedStrs = false;
  bool parsedSyid = false;
  bool parsedFdpc = false;
  bool parsedFdpp = false;
  bool parsedStsh = false;
  bool parsedFont = false;
  std::vector<unsigned> textLengths;
  std::vector<unsigned> textIDs;
  std::vector<unsigned> textOffsets;
  std::map<unsigned, std::vector<unsigned> > tableCellTextEnds;
  unsigned textOffsetAccum = 0;
  std::vector<TextSpanReference> spans;
  std::vector<TextParagraphReference> paras;
  unsigned whichStsh = 0;
  for (std::list<QuillChunkReference>::const_iterator i = chunkReferences.begin(); i != chunkReferences.end(); ++i)
  {
    if (i->name == "TEXT")
    {
      textChunkReference = i;
    }
    else if (i->name == "STRS")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      unsigned numLengths = readU32(input); //Assuming the first DWORD is the number of children and that the next is the remaining length before children start. We are unsure that this is correct.
      input->seek(4 + i->offset + readU32(input), librevenge::RVNG_SEEK_SET);
      for (unsigned j = 0; j < numLengths; ++j)
      {
        unsigned length = readU32(input);
        textLengths.push_back(length);
        textOffsets.push_back(textOffsetAccum);
        textOffsetAccum += length * 2;
      }
      parsedStrs = true;
    }
    else if (i->name == "SYID")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      readU32(input); // Don't know what the first DWORD means.
      unsigned numIDs = readU32(input);
      for (unsigned j = 0; j < numIDs; ++j)
      {
        textIDs.push_back(readU32(input));
      }
      parsedSyid = true;
    }
    else if (i->name == "PL  ")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      parseColors(input, *i);
    }
    else if (i->name == "FDPC")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      std::vector<TextSpanReference> thisBlockSpans = parseCharacterStyles(input, *i);
      spans.insert(spans.end(), thisBlockSpans.begin(), thisBlockSpans.end());
      parsedFdpc |= !thisBlockSpans.empty();
    }
    else if (i->name == "FDPP")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      std::vector<TextParagraphReference> thisBlockParas = parseParagraphStyles(input, *i);
      paras.insert(paras.end(), thisBlockParas.begin(), thisBlockParas.end());
      parsedFdpp |= !thisBlockParas.empty();
    }
    else if (i->name == "STSH")
    {
      if (whichStsh++ == 1)
      {
        input->seek(i->offset, librevenge::RVNG_SEEK_SET);
        parseDefaultStyle(input, *i);
        parsedStsh = true;
      }
    }
    else if (i->name == "FONT")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      parseFonts(input, *i);
      parsedFont = true;
    }
    else if (i->name == "TCD ")
    {
      input->seek(i->offset, librevenge::RVNG_SEEK_SET);
      tableCellTextEnds[i->id] = parseTableCellDefinitions(input, *i);
    }
  }
  if (parsedStrs && parsedSyid && parsedFdpc && parsedFdpp && parsedStsh && parsedFont && textChunkReference != chunkReferences.end())
  {
    input->seek(textChunkReference->offset, librevenge::RVNG_SEEK_SET);
    unsigned bytesRead = 0;
    auto currentTextSpan = spans.begin();
    auto currentTextPara = paras.begin();
    for (unsigned j = 0; j < textIDs.size() && j < textLengths.size(); ++j)
    {
      MSPUB_DEBUG_MSG(("Parsing a text block.\n"));
      std::vector<TextParagraph> readParas;
      std::vector<TextSpan> readSpans;
      std::vector<unsigned char> text;
      for (unsigned k = 0; k < textLengths[j] && currentTextPara != paras.end() && currentTextSpan != spans.end(); ++k)
      {
        text.push_back(readU8(input));
        text.push_back(readU8(input));
        bytesRead += 2;
        if (bytesRead >= currentTextSpan->last - textChunkReference->offset)
        {
          if (!text.empty())
          {
            readSpans.push_back(TextSpan(text, currentTextSpan->charStyle));
            MSPUB_DEBUG_MSG(("Saw text span %d in the current text paragraph.\n", (unsigned)readSpans.size()));
          }
          ++currentTextSpan;
          text.clear();
        }
        if (bytesRead >= currentTextPara->last - textChunkReference->offset)
        {
          if (!text.empty())
          {
            readSpans.push_back(TextSpan(text, currentTextSpan->charStyle));
            MSPUB_DEBUG_MSG(("Saw text span %d in the current text paragraph.\n", (unsigned)readSpans.size()));
          }
          text.clear();
          if (!readSpans.empty())
          {
            readParas.push_back(TextParagraph(readSpans, currentTextPara->paraStyle));
            MSPUB_DEBUG_MSG(("Saw paragraph %d in the current text block.\n", (unsigned)readParas.size()));
          }
          ++currentTextPara;
          readSpans.clear();
        }
      }
      if (!text.empty() && currentTextSpan != spans.end())
      {
        readSpans.push_back(TextSpan(text, currentTextSpan->charStyle));
        MSPUB_DEBUG_MSG(("Saw text span %d in the current text paragraph.\n", (unsigned)readSpans.size()));
      }
      text.clear();
      if (!readSpans.empty() && currentTextPara != paras.end())
      {
        readParas.push_back(TextParagraph(readSpans, currentTextPara->paraStyle));
        MSPUB_DEBUG_MSG(("Saw paragraph %d in the current text block.\n", (unsigned)readParas.size()));
      }
      m_collector->addTextString(readParas, textIDs[j]);
      m_collector->setTextStringOffset(textIDs[j], textOffsets[j]);
      const std::map<unsigned, std::vector<unsigned> >::const_iterator it = tableCellTextEnds.find(j);
      if (it != tableCellTextEnds.end())
        m_collector->setTableCellTextEnds(textIDs[j], it->second);
    }
    textChunkReference = chunkReferences.end();
  }
  return true;
}

void MSPUBParser::parseFonts(librevenge::RVNGInputStream *input, const QuillChunkReference &)
{
  readU32(input);
  unsigned numElements = readU32(input);
  input->seek(input->tell() + 12 + 4 * numElements, librevenge::RVNG_SEEK_SET);
  for (unsigned i = 0; i < numElements; ++i)
  {
    unsigned short nameLength = readU16(input);
    if (nameLength > 0)
    {
      std::vector<unsigned char> name;
      readNBytes(input, nameLength * 2, name);
      m_collector->addFont(name);
    }
    readU32(input);
  }
}

void MSPUBParser::parseDefaultStyle(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk)
{
  readU32(input);
  unsigned numElements = std::min(readU32(input), m_length);
  input->seek(input->tell() + 12, librevenge::RVNG_SEEK_SET);
  std::vector<unsigned> offsets;
  offsets.reserve(numElements);
  for (unsigned i = 0; i < numElements; ++i)
  {
    offsets.push_back(readU32(input));
  }
  for (unsigned i = 0; i < numElements; ++i)
  {
    input->seek(chunk.offset + 20 + offsets[i], librevenge::RVNG_SEEK_SET);
    readU16(input);
    if (i % 2 == 0)
    {
      //FIXME: Does STSH2 hold information for associating style indices in FDPP to indices in STSH1 ?
      m_collector->addDefaultCharacterStyle(getCharacterStyle(input));
    }
    else
    {
      m_collector->addDefaultParagraphStyle(getParagraphStyle(input));
    }
  }
}


void MSPUBParser::parseColors(librevenge::RVNGInputStream *input, const QuillChunkReference &)
{
  unsigned numEntries = readU32(input);
  input->seek(input->tell() + 8, librevenge::RVNG_SEEK_SET);
  for (unsigned i = 0; i < numEntries; ++i)
  {
    unsigned blocksOffset = input->tell();
    unsigned len = readU32(input);
    while (stillReading(input, blocksOffset + len))
    {
      MSPUBBlockInfo info = parseBlock(input, true);
      if (info.id == 0x01)
      {
        m_collector->addTextColor(ColorReference(info.data));
      }
    }
  }
}

std::vector<MSPUBParser::TextParagraphReference> MSPUBParser::parseParagraphStyles(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk)
{
  std::vector<TextParagraphReference> ret;
  unsigned short numEntries = readU16(input);
  input->seek(input->tell() + 6, librevenge::RVNG_SEEK_SET);
  std::vector<unsigned> textOffsets;
  textOffsets.reserve(numEntries);
  std::vector<unsigned short> chunkOffsets;
  textOffsets.reserve(numEntries);
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    textOffsets.push_back(readU32(input));
  }
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    chunkOffsets.push_back(readU16(input));
  }
  unsigned currentSpanBegin = 0;
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    input->seek(chunk.offset + chunkOffsets[i], librevenge::RVNG_SEEK_SET);
    ParagraphStyle style = getParagraphStyle(input);
    ret.push_back(TextParagraphReference(currentSpanBegin, textOffsets[i], style));
    currentSpanBegin = textOffsets[i] + 1;
  }
  return ret;
}

std::vector<MSPUBParser::TextSpanReference> MSPUBParser::parseCharacterStyles(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk)
{
  unsigned short numEntries = readU16(input);
  input->seek(input->tell() + 6, librevenge::RVNG_SEEK_SET);
  std::vector<unsigned> textOffsets;
  textOffsets.reserve(numEntries);
  std::vector<unsigned short> chunkOffsets;
  chunkOffsets.reserve(numEntries);
  std::vector<TextSpanReference> ret;
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    textOffsets.push_back(readU32(input));
  }
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    chunkOffsets.push_back(readU16(input));
  }
  unsigned currentSpanBegin = 0;
  for (unsigned short i = 0; i < numEntries; ++i)
  {
    input->seek(chunk.offset + chunkOffsets[i], librevenge::RVNG_SEEK_SET);
    CharacterStyle style = getCharacterStyle(input);
    currentSpanBegin = textOffsets[i] + 1;
    ret.push_back(TextSpanReference(currentSpanBegin, textOffsets[i], style));
  }
  return ret;
}
ParagraphStyle MSPUBParser::getParagraphStyle(librevenge::RVNGInputStream *input)
{
  ParagraphStyle ret;

  bool isList = false;
  unsigned bulletChar = 0;
  NumberingType numberingType = STANDARD_WESTERN;
  NumberingDelimiter numberingDelimiter = NO_DELIMITER;
  boost::optional<unsigned> numberIfRestarted;

  unsigned offset = input->tell();
  unsigned len = readU32(input);
  while (stillReading(input, offset + len))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    switch (info.id)
    {
    case PARAGRAPH_ALIGNMENT:
      ret.m_align = (Alignment)(info.data & 0xFF); // Is this correct?
      break;
    case PARAGRAPH_DEFAULT_CHAR_STYLE:
      ret.m_defaultCharStyleIndex = info.data;
      break;
    case PARAGRAPH_LINE_SPACING:
      if (info.data & 1)
      {
        // line spacing expressed in points in the UI,
        // in eighths of an emu in the file format.
        // (WTF??)
        ret.m_lineSpacing = LineSpacingInfo(LINE_SPACING_PT,
                                            static_cast<double>(info.data - 1) / 8 * 72 / EMUS_IN_INCH);
      }
      else if (info.data & 2)
      {
        // line spacing expressed in SP in the UI,
        // in what would be EMUs if font size were 96pt in the file format
        // (WTF??)
        ret.m_lineSpacing = LineSpacingInfo(LINE_SPACING_SP,
                                            static_cast<double>(info.data - 2) / EMUS_IN_INCH * 72 / 96);
      }
      break;
    case PARAGRAPH_SPACE_BEFORE:
      ret.m_spaceBeforeEmu = info.data;
      break;
    case PARAGRAPH_SPACE_AFTER:
      ret.m_spaceAfterEmu = info.data;
      break;
    case PARAGRAPH_FIRST_LINE_INDENT:
      ret.m_firstLineIndentEmu = (int)info.data;
      break;
    case PARAGRAPH_LEFT_INDENT:
      ret.m_leftIndentEmu = info.data;
      break;
    case PARAGRAPH_RIGHT_INDENT:
      ret.m_rightIndentEmu = info.data;
      break;
    case PARAGRAPH_TABS:
      input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo tabArrayInfo = parseBlock(input, true);
        if (tabArrayInfo.id == TAB_ARRAY)
        {
          input->seek(tabArrayInfo.dataOffset + 4, librevenge::RVNG_SEEK_SET);
          while (stillReading(input, tabArrayInfo.dataOffset + tabArrayInfo.dataLength))
          {
            MSPUBBlockInfo tabEntryInfo = parseBlock(input, true);
            if (tabEntryInfo.type == GENERAL_CONTAINER)
            {
              input->seek(tabEntryInfo.dataOffset + 4, librevenge::RVNG_SEEK_SET);
              MSPUBBlockInfo tabInfo = parseBlock(input, true);
              if (tabInfo.id == TAB_AMOUNT)
              {
                ret.m_tabStopsInEmu.push_back(tabInfo.data);
              }
            }
          }
        }
      }
      break;
    case PARAGRAPH_LIST_INFO:
    {
      isList = true;
      input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo listSubInfo = parseBlock(input, true);
        switch (listSubInfo.id)
        {
        case PARAGRAPH_LIST_NUMBERING_TYPE:
          numberingType = static_cast<NumberingType>(info.data);
          break;
        case PARAGRAPH_LIST_BULLET_CHAR:
          bulletChar = info.data;
          break;
        default:
          break;
        }
      }
      break;
    }
    case PARAGRAPH_LIST_NUMBER_RESTART:
      numberIfRestarted = info.data;
      break;
    case PARAGRAPH_DROP_CAP_LINES:
      ret.m_dropCapLines = info.data;
      break;
    case PARAGRAPH_DROP_CAP_LETTERS:
      ret.m_dropCapLetters = info.data;
      break;
    default:
      break;
    }
  }
  if (isList)
  {
    if (bulletChar)
    {
      ret.m_listInfo = ListInfo(bulletChar);
    }
    else
    {
      ret.m_listInfo = ListInfo(numberIfRestarted, numberingType,
                                numberingDelimiter);
    }
  }

  return ret;
}

CharacterStyle MSPUBParser::getCharacterStyle(librevenge::RVNGInputStream *input)
{
  bool seenUnderline = false, seenBold1 = false, seenBold2 = false, seenItalic1 = false, seenItalic2 = false;
  int textSize1 = -1, /* textSize2 = -1,*/ colorIndex = -1;
  boost::optional<unsigned> fontIndex;
  SuperSubType sst = NO_SUPER_SUB;
  unsigned offset = input->tell();
  unsigned len = readU32(input);
  while (stillReading(input, offset + len))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    switch (info.id)
    {
    case BOLD_1_ID:
      seenBold1 = true;
      break;
    case BOLD_2_ID:
      seenBold2 = true;
      break;
    case ITALIC_1_ID:
      seenItalic1 = true;
      break;
    case ITALIC_2_ID:
      seenItalic2 = true;
      break;
    case UNDERLINE_ID:
      seenUnderline = true;
      break;
    case TEXT_SIZE_1_ID:
      textSize1 = info.data;
      break;
    case TEXT_SIZE_2_ID:
      // textSize2 = info.data;
      break;
    case BARE_COLOR_INDEX_ID:
      colorIndex = info.data;
      break;
    case COLOR_INDEX_CONTAINER_ID:
      colorIndex = getColorIndex(input, info);
      break;
    case FONT_INDEX_CONTAINER_ID:
      fontIndex = getFontIndex(input, info);
      break;
    case SUPER_SUB_TYPE_ID:
      sst = static_cast<SuperSubType>(info.data);
      break;
    default:
      break;
    }
  }
  //FIXME: Figure out what textSize2 is used for. Can we find a document where it differs from textSize1 ?
  // textSize2 = textSize1;
  boost::optional<double> dTextSize;
  if (textSize1 != -1)
  {
    dTextSize = textSize1 * (double(POINTS_IN_INCH) / EMUS_IN_INCH);
  }
  return CharacterStyle(seenUnderline, seenItalic1 && seenItalic2, seenBold1 && seenBold2, dTextSize, getColorIndexByQuillEntry(colorIndex), fontIndex, sst);
}

unsigned MSPUBParser::getFontIndex(librevenge::RVNGInputStream *input, const MSPUBBlockInfo &info)
{
  MSPUB_DEBUG_MSG(("In getFontIndex\n"));
  input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == GENERAL_CONTAINER)
    {
      input->seek(subInfo.dataOffset + 4, librevenge::RVNG_SEEK_SET);
      if (stillReading(input, subInfo.dataOffset + subInfo.dataLength))
      {
        MSPUBBlockInfo subSubInfo = parseBlock(input, true);
        skipBlock(input, info);
        return subSubInfo.data;
      }
    }
  }
  return 0;
}

int MSPUBParser::getColorIndex(librevenge::RVNGInputStream *input, const MSPUBBlockInfo &info)
{
  input->seek(info.dataOffset + 4, librevenge::RVNG_SEEK_SET);
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.id == COLOR_INDEX_ID)
    {
      skipBlock(input, info);
      MSPUB_DEBUG_MSG(("Found color index 0x%x\n", (unsigned)subInfo.data));
      return subInfo.data;
    }
  }
  MSPUB_DEBUG_MSG(("Failed to find color index!\n"));
  return -1;
}

bool MSPUBParser::parseEscher(librevenge::RVNGInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseEscher\n"));
  EscherContainerInfo fakeroot;
  fakeroot.initial = 0;
  fakeroot.type = 0;
  fakeroot.contentsOffset = input->tell();
  fakeroot.contentsLength = (unsigned long)-1; //FIXME: Get the actual length
  EscherContainerInfo dg, dgg;
  //Note: this assumes that dgg comes before any dg with images.
  if (findEscherContainer(input, fakeroot, dgg, OFFICE_ART_DGG_CONTAINER))
  {
    EscherContainerInfo bsc;
    if (findEscherContainer(input, fakeroot, bsc, OFFICE_ART_B_STORE_CONTAINER))
    {
      unsigned short currentDelayIndex = 1;
      while (stillReading(input, bsc.contentsOffset + bsc.contentsLength))
      {
        unsigned begin = input->tell();
        input->seek(begin + 10, librevenge::RVNG_SEEK_SET);
        if (!(readU32(input) == 0 && readU32(input) == 0 && readU32(input) == 0 && readU32(input) == 0))
        {
          m_escherDelayIndices.push_back(currentDelayIndex++);
        }
        else
        {
          m_escherDelayIndices.push_back(-1);
        }
        input->seek(begin + 44, librevenge::RVNG_SEEK_SET);
      }
    }
    input->seek(dgg.contentsOffset + dgg.contentsLength + getEscherElementTailLength(OFFICE_ART_DGG_CONTAINER), librevenge::RVNG_SEEK_SET);
  }
  while (findEscherContainer(input, fakeroot, dg, OFFICE_ART_DG_CONTAINER))
  {
    EscherContainerInfo spgr;
    while (findEscherContainer(input, dg, spgr, OFFICE_ART_SPGR_CONTAINER))
    {
      Coordinate c1, c2;
      parseShapeGroup(input, spgr, c1, c2);
    }
    input->seek(input->tell() + getEscherElementTailLength(OFFICE_ART_DG_CONTAINER), librevenge::RVNG_SEEK_SET);
  }
  return true;
}

void MSPUBParser::parseShapeGroup(librevenge::RVNGInputStream *input, const EscherContainerInfo &spgr, Coordinate parentCoordinateSystem, Coordinate parentGroupAbsoluteCoord)
{
  EscherContainerInfo shapeOrGroup;
  std::set<unsigned short> types;
  types.insert(OFFICE_ART_SPGR_CONTAINER);
  types.insert(OFFICE_ART_SP_CONTAINER);
  while (findEscherContainerWithTypeInSet(input, spgr, shapeOrGroup, types))
  {
    switch (shapeOrGroup.type)
    {
    case OFFICE_ART_SPGR_CONTAINER:
      m_collector->beginGroup();
      parseShapeGroup(input, shapeOrGroup, parentCoordinateSystem, parentGroupAbsoluteCoord);
      m_collector->endGroup();
      break;
    case OFFICE_ART_SP_CONTAINER:
      parseEscherShape(input, shapeOrGroup, parentCoordinateSystem, parentGroupAbsoluteCoord);
      break;
    }
    input->seek(shapeOrGroup.contentsOffset + shapeOrGroup.contentsLength + getEscherElementTailLength(shapeOrGroup.type), librevenge::RVNG_SEEK_SET);
  }
}

void MSPUBParser::parseEscherShape(librevenge::RVNGInputStream *input, const EscherContainerInfo &sp, Coordinate &parentCoordinateSystem, Coordinate &parentGroupAbsoluteCoord)
{
  Coordinate thisParentCoordinateSystem = parentCoordinateSystem;
  bool definesRelativeCoordinates = false;
  EscherContainerInfo cData;
  EscherContainerInfo cAnchor;
  EscherContainerInfo cFopt;
  EscherContainerInfo cTertiaryFopt;
  EscherContainerInfo cFsp;
  EscherContainerInfo cFspgr;
  unsigned shapeFlags = 0;
  bool isGroupLeader = false;
  ShapeType st = RECTANGLE;
  if (findEscherContainer(input, sp, cFspgr, OFFICE_ART_FSPGR))
  {
    input->seek(cFspgr.contentsOffset, librevenge::RVNG_SEEK_SET);
    parentCoordinateSystem.m_xs = readU32(input);
    parentCoordinateSystem.m_ys = readU32(input);
    parentCoordinateSystem.m_xe = readU32(input);
    parentCoordinateSystem.m_ye = readU32(input);
    definesRelativeCoordinates = true;
  }
  input->seek(sp.contentsOffset, librevenge::RVNG_SEEK_SET);
  if (findEscherContainer(input, sp, cFsp, OFFICE_ART_FSP))
  {
    st = (ShapeType)(cFsp.initial >> 4);
    std::map<unsigned short, unsigned> fspData = extractEscherValues(input, cFsp);
    input->seek(cFsp.contentsOffset + 4, librevenge::RVNG_SEEK_SET);
    shapeFlags = readU32(input);
    isGroupLeader = shapeFlags & SF_GROUP;
  }
  input->seek(sp.contentsOffset, librevenge::RVNG_SEEK_SET);
  if (findEscherContainer(input, sp, cData, OFFICE_ART_CLIENT_DATA))
  {
    std::map<unsigned short, unsigned> dataValues = extractEscherValues(input, cData);
    unsigned *shapeSeqNum = getIfExists(dataValues, FIELDID_SHAPE_ID);
    if (shapeSeqNum)
    {
      m_collector->setShapeType(*shapeSeqNum, st);
      m_collector->setShapeFlip(*shapeSeqNum, shapeFlags & SF_FLIP_V, shapeFlags & SF_FLIP_H);
      input->seek(sp.contentsOffset, librevenge::RVNG_SEEK_SET);
      if (isGroupLeader)
      {
        m_collector->setCurrentGroupSeqNum(*shapeSeqNum);
      }
      else
      {
        m_collector->setShapeOrder(*shapeSeqNum);
      }
      std::set<unsigned short> anchorTypes;
      anchorTypes.insert(OFFICE_ART_CLIENT_ANCHOR);
      anchorTypes.insert(OFFICE_ART_CHILD_ANCHOR);
      bool foundAnchor;
      if ((foundAnchor = findEscherContainerWithTypeInSet(input, sp, cAnchor, anchorTypes)) || isGroupLeader)
      {
        bool rotated90 = false;
        MSPUB_DEBUG_MSG(("Found Escher data for %s of seqnum 0x%x\n", isGroupLeader ? "group" : "shape", *shapeSeqNum));
        boost::optional<std::map<unsigned short, unsigned> > maybe_tertiaryFoptValues;
        input->seek(sp.contentsOffset, librevenge::RVNG_SEEK_SET);
        if (findEscherContainer(input, sp, cTertiaryFopt, OFFICE_ART_TERTIARY_FOPT))
        {
          maybe_tertiaryFoptValues = extractEscherValues(input, cTertiaryFopt);
        }
        if (bool(maybe_tertiaryFoptValues))
        {
          const std::map<unsigned short, unsigned> &tertiaryFoptValues =
            maybe_tertiaryFoptValues.get();
          const unsigned *ptr_pictureRecolor = getIfExists_const(tertiaryFoptValues,
                                                                 FIELDID_PICTURE_RECOLOR);
          if (ptr_pictureRecolor)
          {
            m_collector->setShapePictureRecolor(*shapeSeqNum,
                                                ColorReference(*ptr_pictureRecolor));
          }
        }
        input->seek(sp.contentsOffset, librevenge::RVNG_SEEK_SET);
        if (findEscherContainer(input, sp, cFopt, OFFICE_ART_FOPT))
        {
          FOPTValues foptValues = extractFOPTValues(input, cFopt);
          unsigned *pxId = getIfExists(foptValues.m_scalarValues, FIELDID_PXID);
          if (pxId)
          {
            MSPUB_DEBUG_MSG(("Current Escher shape has pxId %d\n", *pxId));
            if (*pxId > 0 && *pxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*pxId - 1] >= 0)
            {
              m_collector->setShapeImgIndex(*shapeSeqNum, m_escherDelayIndices[*pxId - 1]);
            }
            else
            {
              MSPUB_DEBUG_MSG(("Couldn't find corresponding escherDelay index\n"));
            }
            unsigned *ptr_pictureBrightness = getIfExists(foptValues.m_scalarValues, FIELDID_PICTURE_BRIGHTNESS);
            if (ptr_pictureBrightness)
            {
              m_collector->setShapePictureBrightness(*shapeSeqNum, (int)(*ptr_pictureBrightness));
            }
            unsigned *ptr_pictureContrast = getIfExists(foptValues.m_scalarValues, FIELDID_PICTURE_CONTRAST);
            if (ptr_pictureContrast)
            {
              m_collector->setShapePictureContrast(*shapeSeqNum, (int)(*ptr_pictureContrast));
            }
          }
          unsigned *ptr_lineBackColor =
            getIfExists(foptValues.m_scalarValues, FIELDID_LINE_BACK_COLOR);
          if (ptr_lineBackColor &&
              static_cast<int>(*ptr_lineBackColor) != -1)
          {
            m_collector->setShapeLineBackColor(
              *shapeSeqNum, ColorReference(*ptr_lineBackColor));
          }
          unsigned *ptr_lineColor = getIfExists(foptValues.m_scalarValues, FIELDID_LINE_COLOR);
          unsigned *ptr_lineFlags = getIfExists(foptValues.m_scalarValues, FIELDID_LINE_STYLE_BOOL_PROPS);
          unsigned *ptr_geomFlags = getIfExists(
                                      foptValues.m_scalarValues, FIELDID_GEOM_BOOL_PROPS);
          bool useLine = lineExistsByFlagPointer(
                           ptr_lineFlags, ptr_geomFlags);
          bool skipIfNotBg = false;
          std::shared_ptr<Fill> ptr_fill = getNewFill(foptValues.m_scalarValues, skipIfNotBg, foptValues.m_complexValues);
          unsigned lineWidth = 0;
          if (useLine)
          {
            if (ptr_lineColor)
            {
              unsigned *ptr_lineWidth = getIfExists(foptValues.m_scalarValues, FIELDID_LINE_WIDTH);
              lineWidth = ptr_lineWidth ? *ptr_lineWidth : 9525;
              m_collector->addShapeLine(*shapeSeqNum, Line(ColorReference(*ptr_lineColor), lineWidth, true));
            }
            else
            {
              if (bool(maybe_tertiaryFoptValues))
              {
                std::map<unsigned short, unsigned> &tertiaryFoptValues =
                  maybe_tertiaryFoptValues.get();
                unsigned *ptr_tertiaryLineFlags = getIfExists(tertiaryFoptValues, FIELDID_LINE_STYLE_BOOL_PROPS);
                if (lineExistsByFlagPointer(ptr_tertiaryLineFlags))
                {
                  unsigned *ptr_topColor = getIfExists(tertiaryFoptValues, FIELDID_LINE_TOP_COLOR);
                  unsigned *ptr_topWidth = getIfExists(tertiaryFoptValues, FIELDID_LINE_TOP_WIDTH);
                  unsigned *ptr_topFlags = getIfExists(tertiaryFoptValues, FIELDID_LINE_TOP_BOOL_PROPS);
                  unsigned *ptr_rightColor = getIfExists(tertiaryFoptValues, FIELDID_LINE_RIGHT_COLOR);
                  unsigned *ptr_rightWidth = getIfExists(tertiaryFoptValues, FIELDID_LINE_RIGHT_WIDTH);
                  unsigned *ptr_rightFlags = getIfExists(tertiaryFoptValues, FIELDID_LINE_RIGHT_BOOL_PROPS);
                  unsigned *ptr_bottomColor = getIfExists(tertiaryFoptValues, FIELDID_LINE_BOTTOM_COLOR);
                  unsigned *ptr_bottomWidth = getIfExists(tertiaryFoptValues, FIELDID_LINE_BOTTOM_WIDTH);
                  unsigned *ptr_bottomFlags = getIfExists(tertiaryFoptValues, FIELDID_LINE_BOTTOM_BOOL_PROPS);
                  unsigned *ptr_leftColor = getIfExists(tertiaryFoptValues, FIELDID_LINE_LEFT_COLOR);
                  unsigned *ptr_leftWidth = getIfExists(tertiaryFoptValues, FIELDID_LINE_LEFT_WIDTH);
                  unsigned *ptr_leftFlags = getIfExists(tertiaryFoptValues, FIELDID_LINE_LEFT_BOOL_PROPS);

                  bool topExists = ptr_topColor && lineExistsByFlagPointer(ptr_topFlags);
                  bool rightExists = ptr_rightColor && lineExistsByFlagPointer(ptr_rightFlags);
                  bool bottomExists = ptr_bottomColor && lineExistsByFlagPointer(ptr_bottomFlags);
                  bool leftExists = ptr_leftColor && lineExistsByFlagPointer(ptr_leftFlags);
                  if (ptr_topWidth)
                  {
                    lineWidth = *ptr_topWidth;
                  }

                  m_collector->addShapeLine(*shapeSeqNum,
                                            topExists ? Line(ColorReference(*ptr_topColor), ptr_topWidth ? *ptr_topWidth : 9525, true) :
                                            Line(ColorReference(0), 0, false));
                  m_collector->addShapeLine(*shapeSeqNum,
                                            rightExists ? Line(ColorReference(*ptr_rightColor), ptr_rightWidth ? *ptr_rightWidth : 9525, true) :
                                            Line(ColorReference(0), 0, false));
                  m_collector->addShapeLine(*shapeSeqNum,
                                            bottomExists ? Line(ColorReference(*ptr_bottomColor), ptr_bottomWidth ? *ptr_bottomWidth : 9525, true) :
                                            Line(ColorReference(0), 0, false));
                  m_collector->addShapeLine(*shapeSeqNum,
                                            leftExists ? Line(ColorReference(*ptr_leftColor), ptr_leftWidth ? *ptr_leftWidth : 9525, true) :
                                            Line(ColorReference(0), 0, false));

                  // Amazing feat of Microsoft engineering:
                  // The detailed interaction of four flags describes ONE true/false property!

                  if (ptr_leftFlags &&
                      (*ptr_leftFlags & FLAG_USE_LEFT_INSET_PEN) &&
                      (!(*ptr_leftFlags & FLAG_USE_LEFT_INSET_PEN_OK) || (*ptr_leftFlags & FLAG_LEFT_INSET_PEN_OK)) &&
                      (*ptr_leftFlags & FLAG_LEFT_INSET_PEN))
                  {
                    m_collector->setShapeBorderPosition(*shapeSeqNum, INSIDE_SHAPE);
                  }
                  else
                  {
                    m_collector->setShapeBorderPosition(*shapeSeqNum, HALF_INSIDE_SHAPE);
                  }
                }
              }
            }
          }
          if (ptr_fill)
          {
            m_collector->setShapeFill(*shapeSeqNum, ptr_fill, skipIfNotBg);
          }
          int *ptr_adjust1 = (int *)getIfExists(foptValues.m_scalarValues, FIELDID_ADJUST_VALUE_1);
          int *ptr_adjust2 = (int *)getIfExists(foptValues.m_scalarValues, FIELDID_ADJUST_VALUE_2);
          int *ptr_adjust3 = (int *)getIfExists(foptValues.m_scalarValues, FIELDID_ADJUST_VALUE_3);
          if (ptr_adjust1)
          {
            m_collector->setAdjustValue(*shapeSeqNum, 0, *ptr_adjust1);
          }
          if (ptr_adjust2)
          {
            m_collector->setAdjustValue(*shapeSeqNum, 1, *ptr_adjust2);
          }
          if (ptr_adjust3)
          {
            m_collector->setAdjustValue(*shapeSeqNum, 2, *ptr_adjust3);
          }
          int *ptr_rotation = (int *)getIfExists(foptValues.m_scalarValues, FIELDID_ROTATION);
          if (ptr_rotation)
          {
            double rotation = doubleModulo(toFixedPoint(*ptr_rotation), 360);
            m_collector->setShapeRotation(*shapeSeqNum, short(rotation));
            //FIXME : make MSPUBCollector handle double shape rotations
            rotated90 = (rotation >= 45 && rotation < 135) || (rotation >= 225 && rotation < 315);

          }
          unsigned *ptr_left = getIfExists(foptValues.m_scalarValues, FIELDID_DY_TEXT_LEFT);
          unsigned *ptr_top = getIfExists(foptValues.m_scalarValues, FIELDID_DY_TEXT_TOP);
          unsigned *ptr_right = getIfExists(foptValues.m_scalarValues, FIELDID_DY_TEXT_RIGHT);
          unsigned *ptr_bottom = getIfExists(foptValues.m_scalarValues, FIELDID_DY_TEXT_BOTTOM);
          m_collector->setShapeMargins(*shapeSeqNum, ptr_left ? *ptr_left : DEFAULT_MARGIN,
                                       ptr_top ? *ptr_top : DEFAULT_MARGIN,
                                       ptr_right ? *ptr_right : DEFAULT_MARGIN,
                                       ptr_bottom ? *ptr_bottom : DEFAULT_MARGIN);
          unsigned *ptr_lineDashing = getIfExists(foptValues.m_scalarValues, FIELDID_LINE_DASHING);
          unsigned *ptr_lineEndcapStyle = getIfExists(foptValues.m_scalarValues, FIELDID_LINE_ENDCAP_STYLE);
          DotStyle dotStyle = RECT_DOT;
          if (ptr_lineEndcapStyle)
          {
            switch (*ptr_lineEndcapStyle)
            {
            case 0:
              dotStyle = ROUND_DOT;
              break;
            default:
              break;
            }
          }
          if (ptr_lineDashing)
          {
            m_collector->setShapeDash(*shapeSeqNum, getDash(
                                        static_cast<MSPUBDashStyle>(*ptr_lineDashing), lineWidth,
                                        dotStyle));
          }

          if (bool(maybe_tertiaryFoptValues))
          {
            std::map<unsigned short, unsigned> &tertiaryFoptValues = maybe_tertiaryFoptValues.get();
            unsigned *ptr_numColumns = getIfExists(tertiaryFoptValues, FIELDID_NUM_COLUMNS);
            if (ptr_numColumns)
            {
              m_collector->setShapeNumColumns(*shapeSeqNum, *ptr_numColumns);
            }
            unsigned *ptr_columnSpacing = getIfExists(tertiaryFoptValues, FIELDID_COLUMN_SPACING);
            if (ptr_columnSpacing)
            {
              m_collector->setShapeColumnSpacing(*shapeSeqNum, *ptr_columnSpacing);
            }
          }
          unsigned *ptr_beginArrowStyle = getIfExists(foptValues.m_scalarValues,
                                                      FIELDID_BEGIN_ARROW_STYLE);
          unsigned *ptr_beginArrowWidth = getIfExists(foptValues.m_scalarValues,
                                                      FIELDID_BEGIN_ARROW_WIDTH);
          unsigned *ptr_beginArrowHeight = getIfExists(foptValues.m_scalarValues,
                                                       FIELDID_BEGIN_ARROW_HEIGHT);
          m_collector->setShapeBeginArrow(*shapeSeqNum, Arrow(
                                            ptr_beginArrowStyle ? (ArrowStyle)(*ptr_beginArrowStyle) :
                                            NO_ARROW,
                                            ptr_beginArrowWidth ? (ArrowSize)(*ptr_beginArrowWidth) :
                                            MEDIUM,
                                            ptr_beginArrowHeight ? (ArrowSize)(*ptr_beginArrowHeight) :
                                            MEDIUM));
          unsigned *ptr_endArrowStyle = getIfExists(foptValues.m_scalarValues,
                                                    FIELDID_END_ARROW_STYLE);
          unsigned *ptr_endArrowWidth = getIfExists(foptValues.m_scalarValues,
                                                    FIELDID_END_ARROW_WIDTH);
          unsigned *ptr_endArrowHeight = getIfExists(foptValues.m_scalarValues,
                                                     FIELDID_END_ARROW_HEIGHT);
          m_collector->setShapeEndArrow(*shapeSeqNum, Arrow(
                                          ptr_endArrowStyle ? (ArrowStyle)(*ptr_endArrowStyle) :
                                          NO_ARROW,
                                          ptr_endArrowWidth ? (ArrowSize)(*ptr_endArrowWidth) :
                                          MEDIUM,
                                          ptr_endArrowHeight ? (ArrowSize)(*ptr_endArrowHeight) :
                                          MEDIUM));

          unsigned *shadowBoolProps = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_BOOL_PROPS);
          if (shadowBoolProps)
          {
            unsigned shadowProps = *shadowBoolProps;
            if ((shadowProps & FLAG_USE_FSHADOW) && (shadowProps & FLAG_USE_SHADOW))
            {
              unsigned *ptr_shadowType = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_TYPE);
              auto shadowType = static_cast<ShadowType>(ptr_shadowType ? *ptr_shadowType : 0);
              unsigned *shadowColor = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_COLOR);
              unsigned *shadowHColor = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_HIGHLIGHT);
              unsigned *shadowOpacity = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_OPACITY);
              unsigned *shadowOffsetX = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_OFFSET_X);
              unsigned *shadowOffsetY = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_OFFSET_Y);
              unsigned *shadowOffsetX2 = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_SECOND_OFFSET_X);
              unsigned *shadowOffsetY2 = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_SECOND_OFFSET_Y);
              unsigned *shadowOriginX = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_ORIGIN_X);
              unsigned *shadowOriginY = getIfExists(foptValues.m_scalarValues, FIELDID_SHADOW_ORIGIN_Y);
              m_collector->setShapeShadow(*shapeSeqNum, Shadow(shadowType,
                                                               shadowOffsetX ? static_cast<int>(*shadowOffsetX) : 0x6338,
                                                               shadowOffsetY ? static_cast<int>(*shadowOffsetY) : 0x6338,
                                                               shadowOffsetX2 ? static_cast<int>(*shadowOffsetX2) : 0,
                                                               shadowOffsetY2 ? static_cast<int>(*shadowOffsetY2) : 0,
                                                               shadowOriginX ? toFixedPoint(static_cast<int>(*shadowOriginX)) : 0,
                                                               shadowOriginY ? toFixedPoint(static_cast<int>(*shadowOriginY)) : 0,
                                                               toFixedPoint(shadowOpacity ? static_cast<int>(*shadowOpacity) : 0x10000),
                                                               ColorReference(shadowColor ? *shadowColor : 0x00808080),
                                                               ColorReference(shadowHColor ? *shadowHColor : 0x00CBCBCB)
                                                              ));

            }
          }

          const std::vector<unsigned char> vertexData = foptValues.m_complexValues[FIELDID_P_VERTICES];
          if (!vertexData.empty())
          {
            unsigned *p_geoRight = getIfExists(foptValues.m_scalarValues,
                                               FIELDID_GEO_RIGHT);
            unsigned *p_geoBottom = getIfExists(foptValues.m_scalarValues,
                                                FIELDID_GEO_BOTTOM);
            const std::vector<unsigned char> segmentData = foptValues.m_complexValues[FIELDID_P_SEGMENTS];
            const std::vector<unsigned char> guideData = foptValues.m_complexValues[FIELDID_P_GUIDES];
            m_collector->setShapeCustomPath(*shapeSeqNum, getDynamicCustomShape(vertexData, segmentData,
                                                                                guideData, p_geoRight ? *p_geoRight : 21600,
                                                                                p_geoBottom ? *p_geoBottom : 21600));
          }
          const std::vector<unsigned char> wrapVertexData = foptValues.m_complexValues[FIELDID_P_WRAPPOLYGONVERTICES];
          if (!wrapVertexData.empty())
          {
            std::vector<Vertex> ret = parseVertices(wrapVertexData);
            m_collector->setShapeClipPath(*shapeSeqNum, ret);
          }
        }
        if (foundAnchor)
        {
          Coordinate absolute;
          if (cAnchor.type == OFFICE_ART_CLIENT_ANCHOR)
          {
            std::map<unsigned short, unsigned> anchorData = extractEscherValues(input, cAnchor);
            absolute = Coordinate(anchorData[FIELDID_XS],
                                  anchorData[FIELDID_YS], anchorData[FIELDID_XE],
                                  anchorData[FIELDID_YE]);
          }
          else if (cAnchor.type == OFFICE_ART_CHILD_ANCHOR)
          {
            input->seek(cAnchor.contentsOffset, librevenge::RVNG_SEEK_SET);
            int coordSystemWidth = thisParentCoordinateSystem.m_xe - thisParentCoordinateSystem.m_xs;
            int coordSystemHeight = thisParentCoordinateSystem.m_ye - thisParentCoordinateSystem.m_ys;
            int groupWidth = parentGroupAbsoluteCoord.m_xe - parentGroupAbsoluteCoord.m_xs;
            int groupHeight = parentGroupAbsoluteCoord.m_ye - parentGroupAbsoluteCoord.m_ys;
            double widthScale = (double)groupWidth / coordSystemWidth;
            double heightScale = (double)groupHeight / coordSystemHeight;
            int xs = (readU32(input) - thisParentCoordinateSystem.m_xs) * widthScale + parentGroupAbsoluteCoord.m_xs;
            int ys = (readU32(input) - thisParentCoordinateSystem.m_ys) * heightScale + parentGroupAbsoluteCoord.m_ys;
            int xe = (readU32(input) - thisParentCoordinateSystem.m_xs) * widthScale + parentGroupAbsoluteCoord.m_xs;
            int ye = (readU32(input) - thisParentCoordinateSystem.m_ys) * heightScale + parentGroupAbsoluteCoord.m_ys;

            absolute = Coordinate(xs, ys, xe, ye);
          }
          if (rotated90)
          {
            int initialX = absolute.m_xs;
            int initialY = absolute.m_ys;
            int initialWidth = absolute.m_xe - absolute.m_xs;
            int initialHeight = absolute.m_ye - absolute.m_ys;
            int centerX = initialX + initialWidth / 2;
            int centerY = initialY + initialHeight / 2;
            int xs = centerX - initialHeight / 2;
            int ys = centerY - initialWidth / 2;
            int xe = xs + initialHeight;
            int ye = ys + initialWidth;
            absolute = Coordinate(xs, ys, xe, ye);
          }
          m_collector->setShapeCoordinatesInEmu(*shapeSeqNum,
                                                absolute.m_xs,
                                                absolute.m_ys,
                                                absolute.m_xe,
                                                absolute.m_ye);
          if (definesRelativeCoordinates)
          {
            parentGroupAbsoluteCoord = absolute;
          }
        }
      }
    }
  }
}

std::shared_ptr<Fill> MSPUBParser::getNewFill(const std::map<unsigned short, unsigned> &foptProperties,
                                              bool &skipIfNotBg, std::map<unsigned short, std::vector<unsigned char> > &foptValues)
{
  const FillType *ptr_fillType = (FillType *)getIfExists_const(foptProperties, FIELDID_FILL_TYPE);
  FillType fillType = ptr_fillType ? *ptr_fillType : SOLID;
  switch (fillType)
  {
  case SOLID:
  {
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fieldStyleProps = getIfExists_const(foptProperties, FIELDID_FIELD_STYLE_BOOL_PROPS);
    skipIfNotBg = ptr_fieldStyleProps && (*ptr_fieldStyleProps & 0xF0) == 0;
    if (ptr_fillColor && !skipIfNotBg)
    {
      const unsigned *ptr_fillOpacity = getIfExists_const(foptProperties, FIELDID_FILL_OPACITY);
      return std::shared_ptr<Fill>(new SolidFill(ColorReference(*ptr_fillColor), ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1, m_collector));
    }
    return std::shared_ptr<Fill>();
  }
  case SHADE_SHAPE:
  case SHADE_CENTER:
  case SHADE:
  case SHADE_SCALE:
  {
    int angle;
    const int *ptr_angle = (const int *)getIfExists_const(foptProperties, FIELDID_FILL_ANGLE);
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fillBackColor = getIfExists_const(foptProperties, FIELDID_FILL_BACK_COLOR);
    unsigned fill = ptr_fillColor ? *ptr_fillColor : 0x00FFFFFFF;
    unsigned fillBack = ptr_fillBackColor ? *ptr_fillBackColor : 0x00FFFFFF;
    ColorReference firstColor(fill, fill);
    ColorReference secondColor(fill, fillBack);
    const unsigned *ptr_fillOpacity = getIfExists_const(foptProperties, FIELDID_FILL_OPACITY);
    const unsigned *ptr_fillBackOpacity = getIfExists_const(foptProperties, FIELDID_FILL_BACK_OPACITY);
    const unsigned *ptr_fillFocus = getIfExists_const(foptProperties, FIELDID_FILL_FOCUS);
    short fillFocus = ptr_fillFocus ? ((int)(*ptr_fillFocus) << 16) >> 16 : 0;
    angle = ptr_angle ? *ptr_angle : 0;
    angle >>= 16; //it's actually only 16 bits
    // Don't try to figure out what sense the following switch statement makes.
    // The angles are just offset by 90 degrees in the file format in some cases.
    // It seems totally arbitrary -- maybe an MS bug ?
    switch (angle)
    {
    case -135:
      angle = -45;
      break;
    case -45:
      angle = 225;
      break;
    default:
      break;
    }
    double fillLeftVal = 0.0;
    const unsigned *ptr_fillLeft = getIfExists_const(foptProperties, FIELDID_FILL_TO_LEFT);
    if (ptr_fillLeft)
      fillLeftVal = toFixedPoint(*ptr_fillLeft);
    double fillTopVal = 0.0;
    const unsigned *ptr_fillTop = getIfExists_const(foptProperties, FIELDID_FILL_TO_TOP);
    if (ptr_fillTop)
      fillTopVal = toFixedPoint(*ptr_fillTop);
    double fillRightVal = 0.0;
    const unsigned *ptr_fillRight = getIfExists_const(foptProperties, FIELDID_FILL_TO_RIGHT);
    if (ptr_fillRight)
      fillRightVal = toFixedPoint(*ptr_fillRight);
    double fillBottomVal = 0.0;
    const unsigned *ptr_fillBottom = getIfExists_const(foptProperties, FIELDID_FILL_TO_BOTTOM);
    if (ptr_fillBottom)
      fillBottomVal = toFixedPoint(*ptr_fillBottom);

    std::shared_ptr<GradientFill> ret(new GradientFill(m_collector, angle, (int)fillType));
    ret->setFillCenter(fillLeftVal, fillTopVal, fillRightVal, fillBottomVal);

    const unsigned *ptr_fillGrad = getIfExists_const(foptProperties, FIELDID_FILL_SHADE_COMPLEX);
    if (ptr_fillGrad)
    {
      const std::vector<unsigned char> gradientData = foptValues[FIELDID_FILL_SHADE_COMPLEX];
      if (gradientData.size() > 6)
      {
        unsigned short numEntries = gradientData[0] | (gradientData[1] << 8);
        unsigned offs = 6;
        for (unsigned i = 0; i < numEntries; ++i)
        {
          unsigned color = gradientData[offs] | (unsigned(gradientData[offs + 1]) << 8) | (unsigned(gradientData[offs + 2]) << 16) | (unsigned(gradientData[offs + 3]) << 24);
          offs += 4;
          auto posi = (int)(toFixedPoint(gradientData[offs] | (unsigned(gradientData[offs + 1]) << 8) | (unsigned(gradientData[offs + 2]) << 16) | (unsigned(gradientData[offs + 3]) << 24)) * 100);
          offs += 4;
          ColorReference sColor(color, color);
          if (fillFocus ==  0)
            ret->addColor(sColor, posi, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
          else if (fillFocus == 100)
            ret->addColorReverse(sColor, 100 - posi, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
          else if (fillFocus > 0)
            ret->addColor(sColor, posi / 2, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
          else if (fillFocus < 0)
            ret->addColorReverse(sColor, (100 - posi) / 2, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
        }
        if ((fillFocus < 0) || ((fillFocus > 0) && (fillFocus < 100)))
          ret->completeComplexFill();
      }
    }
    else
    {
      if (fillFocus ==  0)
      {
        ret->addColor(firstColor, 0, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
        ret->addColor(secondColor, 100, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
      }
      else if (fillFocus == 100)
      {
        ret->addColor(secondColor, 0, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
        ret->addColor(firstColor, 100, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
      }
      else if (fillFocus > 0)
      {
        ret->addColor(secondColor, 0, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
        ret->addColor(firstColor, fillFocus, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
        ret->addColor(secondColor, 100, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);

//        ret->addColor(firstColor, 0, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
//        ret->addColor(secondColor, fillFocus, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
//        ret->addColor(firstColor, 100, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
      }
      else if (fillFocus < 0)
      {
        ret->addColor(firstColor, 0, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
        ret->addColor(secondColor, 100 + fillFocus, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
        ret->addColor(firstColor, 100, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);

//        ret->addColor(secondColor, 0, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
//        ret->addColor(firstColor, 100 + fillFocus, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
//        ret->addColor(secondColor, 100, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
      }
    }
    return ret;
  }
  case TEXTURE:
  case BITMAP:
  {
    // in the case the shape is rotated we must rotate the image too
    int rotation = 0;
    const int *ptr_rotation = (const int *)getIfExists_const(foptProperties, FIELDID_ROTATION);
    if (ptr_rotation)
      rotation = (int)doubleModulo(toFixedPoint(*ptr_rotation), 360);
    const unsigned *ptr_bgPxId = getIfExists_const(foptProperties, FIELDID_BG_PXID);
    if (ptr_bgPxId && *ptr_bgPxId > 0 && *ptr_bgPxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*ptr_bgPxId - 1] >= 0)
    {
      return std::shared_ptr<Fill>(new ImgFill(m_escherDelayIndices[*ptr_bgPxId - 1], m_collector, fillType == TEXTURE, rotation));
    }
    return std::shared_ptr<Fill>();
  }
  case PATTERN:
  {
    const unsigned *ptr_bgPxId = getIfExists_const(foptProperties, FIELDID_BG_PXID);
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fillBackColor = getIfExists_const(foptProperties, FIELDID_FILL_BACK_COLOR);
    ColorReference fill = ptr_fillColor ? ColorReference(*ptr_fillColor) : ColorReference(0x00FFFFFF);
//    ColorReference back = ptr_fillBackColor ? ColorReference(*ptr_fillBackColor) : ColorReference(0x08000000);
    ColorReference back = ptr_fillBackColor ? ColorReference(*ptr_fillBackColor) : ColorReference(0x00FFFFFF);
    if (ptr_bgPxId && *ptr_bgPxId > 0 && *ptr_bgPxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*ptr_bgPxId - 1] >= 0)
    {
      return std::shared_ptr<Fill>(new PatternFill(m_escherDelayIndices[*ptr_bgPxId - 1], m_collector, fill, back));
    }
    return std::shared_ptr<Fill>();
  }
  case BACKGROUND:
  default:
    return std::shared_ptr<Fill>();
  }
}

DynamicCustomShape MSPUBParser::getDynamicCustomShape(
  const std::vector<unsigned char> &vertexData, const std::vector<unsigned char> &segmentData,
  const std::vector<unsigned char> &guideData, unsigned geoWidth,
  unsigned geoHeight)
{
  DynamicCustomShape ret(geoWidth, geoHeight);
  ret.m_vertices = parseVertices(vertexData);
  ret.m_elements = parseSegments(segmentData);
  ret.m_calculations = parseGuides(guideData);
  return ret;
}

std::vector<unsigned short> MSPUBParser::parseSegments(
  const std::vector<unsigned char> &segmentData)
{
  std::vector<unsigned short> ret;
  if (segmentData.size() < 6)
  {
    return ret;
  }
  // assume that the entry size is 2.
  unsigned short numEntries = segmentData[0] | (segmentData[1] << 8);
  unsigned offset = 6;
  for (unsigned i = 0; i < numEntries; ++i)
  {
    if (offset + 2 > segmentData.size())
    {
      break;
    }
    ret.push_back(segmentData[offset] | (segmentData[offset + 1] << 8));
    offset += 2;
  }
  return ret;
}

std::vector<Calculation> MSPUBParser::parseGuides(
  const std::vector<unsigned char> &/* guideData */)
{
  std::vector<Calculation> ret;

  //FIXME : implement this function.

  return ret;
}

std::vector<Vertex> MSPUBParser::parseVertices(
  const std::vector<unsigned char> &vertexData)
{
  std::vector<Vertex> ret;
  if (vertexData.size() < 6)
  {
    return ret;
  }
  unsigned short numVertices = vertexData[0] | (vertexData[1] << 8);
  unsigned short entrySize = vertexData[4] | (vertexData[5] << 8);
  if (entrySize == 0xFFF0)
  {
    entrySize = 4;
  }
  if (!(entrySize == 2 || entrySize == 4 || entrySize == 8))
  {
    MSPUB_DEBUG_MSG(("Incomprehensible entry size %u in vertex complex data!\n", entrySize));
    return ret;
  }
  unsigned offset = 6;
  ret.reserve(numVertices);
  for (unsigned i = 0; i < numVertices; ++i)
  {
    if (offset + entrySize > vertexData.size())
    {
      break;
    }
    int x, y;
    switch (entrySize)
    {
    case 2:
      x = vertexData[offset];
      y = vertexData[offset + 1];
      break;
    case 4:
      x = vertexData[offset] | (unsigned(vertexData[offset + 1]) << 8);
      y = vertexData[offset + 2] | (unsigned(vertexData[offset + 3]) << 8);
      break;
    case 8:
      x = vertexData[offset] | (unsigned(vertexData[offset + 1]) << 8) |
          (unsigned(vertexData[offset + 2]) << 16) | (unsigned(vertexData[offset + 3]) << 24);
      y = vertexData[offset + 4] | (unsigned(vertexData[offset + 5]) << 8) |
          (unsigned(vertexData[offset + 6]) << 16) | (unsigned(vertexData[offset + 7]) << 24);
      break;
    default: // logically shouldn't be able to get here.
      x = 0;
      y = 0;
      break;
    }
    Vertex v = {x, y};
    ret.push_back(v);
    offset += entrySize;
  }
  return ret;
}

unsigned MSPUBParser::getEscherElementTailLength(unsigned short type)
{
  switch (type)
  {
  case OFFICE_ART_DGG_CONTAINER:
  case OFFICE_ART_DG_CONTAINER:
    return 4;
  default:
    return 0;
  }
}

unsigned MSPUBParser::getEscherElementAdditionalHeaderLength(unsigned short type)
{
  switch (type)
  {
  case OFFICE_ART_CLIENT_ANCHOR:
  case OFFICE_ART_CLIENT_DATA: //account for the fact that the length appears twice, for whatever reason
    return 4;
  }
  return 0;
}

bool MSPUBParser::findEscherContainerWithTypeInSet(librevenge::RVNGInputStream *input, const EscherContainerInfo &parent, EscherContainerInfo &out, std::set<unsigned short> types)
{
  while (stillReading(input, parent.contentsOffset + parent.contentsLength))
  {
    EscherContainerInfo next = parseEscherContainer(input);
    if (types.find(next.type) != types.end())
    {
      out = next;
      return true;
    }
    input->seek(next.contentsOffset + next.contentsLength + getEscherElementTailLength(next.type), librevenge::RVNG_SEEK_SET);
  }
  return false;
}

bool MSPUBParser::findEscherContainer(librevenge::RVNGInputStream *input, const EscherContainerInfo &parent, EscherContainerInfo &out, unsigned short desiredType)
{
  MSPUB_DEBUG_MSG(("At offset 0x%lx, attempting to find escher container of type 0x%x\n", input->tell(), desiredType));
  while (stillReading(input, parent.contentsOffset + parent.contentsLength))
  {
    EscherContainerInfo next = parseEscherContainer(input);
    if (next.type == desiredType)
    {
      out = next;
      return true;
    }
    input->seek(next.contentsOffset + next.contentsLength + getEscherElementTailLength(next.type), librevenge::RVNG_SEEK_SET);
  }
  return false;
}

FOPTValues MSPUBParser::extractFOPTValues(librevenge::RVNGInputStream *input, const EscherContainerInfo &record)
{
  FOPTValues ret;
  input->seek(record.contentsOffset, librevenge::RVNG_SEEK_SET);
  unsigned short numValues = record.initial >> 4;
  std::vector<unsigned short> complexIds;
  for (unsigned short i = 0; i < numValues; ++i)
  {
    if (!stillReading(input, record.contentsOffset + record.contentsLength))
    {
      break;
    }
    unsigned short id = readU16(input);
    unsigned value  = readU32(input);
    ret.m_scalarValues[id] = value;
    bool complex = id & 0x8000;
    if (complex)
    {
      complexIds.push_back(id);
    }
  }
  for (unsigned short id : complexIds)
  {
    if (!stillReading(input, record.contentsOffset + record.contentsLength))
    {
      break;
    }
    unsigned length = ret.m_scalarValues[id];
    if (!length)
    {
      continue;
    }
    unsigned short numEntries = readU16(input);
    input->seek(2, librevenge::RVNG_SEEK_CUR);
    unsigned short entryLength = readU16(input);
    if (entryLength == 0xFFF0)
    {
      entryLength = 4;
    }
    input->seek(-6, librevenge::RVNG_SEEK_CUR);
    readNBytes(input, static_cast<unsigned long>(entryLength) * numEntries + 6, ret.m_complexValues[id]);
  }
  return ret;
}

std::map<unsigned short, unsigned> MSPUBParser::extractEscherValues(librevenge::RVNGInputStream *input, const EscherContainerInfo &record)
{
  std::map<unsigned short, unsigned> ret;
  input->seek(record.contentsOffset + getEscherElementAdditionalHeaderLength(record.type), librevenge::RVNG_SEEK_SET);
  while (stillReading(input, record.contentsOffset + record.contentsLength))
  {
    unsigned short id = readU16(input);
    if (id == 0)
    {
      if (!stillReading(input, record.contentsOffset + record.contentsLength))
        break;
      MSPUB_DEBUG_MSG(("found escher value with ID 0!\n"));
    }
    unsigned value = readU32(input);
    ret[id] = value;
  }
  return ret;
}


bool MSPUBParser::parseContentChunkReference(librevenge::RVNGInputStream *input, const MSPUBBlockInfo block)
{
  //input should be at block.dataOffset + 4 , that is, at the beginning of the list of sub-blocks
  MSPUB_DEBUG_MSG(("Parsing chunk reference 0x%x\n", m_lastSeenSeqNum));
  auto type = (MSPUBContentChunkType)0;
  unsigned long offset = 0;
  unsigned parentSeqNum = 0;
  bool seenType = false;
  bool seenOffset = false;
  bool seenParentSeqNum = false;
  while (stillReading(input, block.dataOffset + block.dataLength))
  {
    MSPUBBlockInfo subBlock = parseBlock(input, true);
    //FIXME: Warn if multiple of these blocks seen.
    if (subBlock.id == CHUNK_TYPE)
    {
      type = (MSPUBContentChunkType)subBlock.data;
      seenType = true;
    }
    else if (subBlock.id == CHUNK_OFFSET)
    {
      offset = subBlock.data;
      seenOffset = true;
    }
    else if (subBlock.id == CHUNK_PARENT_SEQNUM)
    {
      parentSeqNum = subBlock.data;
      seenParentSeqNum = true;
    }
  }
  if (seenType && seenOffset) //FIXME: What if there is an offset, but not a type? Should we still set the end of the preceding chunk to that offset?
  {
    if (type == PAGE)
    {
      MSPUB_DEBUG_MSG(("page chunk: offset 0x%lx, seqnum 0x%x\n", offset, m_lastSeenSeqNum));
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_pageChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
      return true;
    }
    else if (type == DOCUMENT)
    {
      MSPUB_DEBUG_MSG(("document chunk: offset 0x%lx, seqnum 0x%x\n", offset, m_lastSeenSeqNum));
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_documentChunkIndex = unsigned(m_contentChunks.size() - 1);
      return true;
    }
    else if (type == SHAPE || type == ALTSHAPE || type == GROUP || type == TABLE || type == LOGO)
    {
      MSPUB_DEBUG_MSG(("shape chunk: offset 0x%lx, seqnum 0x%x, parent seqnum: 0x%x\n", offset, m_lastSeenSeqNum, parentSeqNum));
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_shapeChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
      if (type == ALTSHAPE)
      {
        m_alternateShapeSeqNums.push_back(m_lastSeenSeqNum);
      }
      return true;
    }
    else if (type == CELLS)
    {
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_cellsChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
      return true;
    }
    else if (type == PALETTE)
    {
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_paletteChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
      return true;
    }
    else if (type == BORDER_ART)
    {
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0,
                                                      m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      m_borderArtChunkIndices.push_back(
        unsigned(m_contentChunks.size() - 1));
      return true;
    }
    else if (type == FONT)
    {
      m_contentChunks.push_back(ContentChunkReference(type, offset, 0,
                                                      m_lastSeenSeqNum,
                                                      seenParentSeqNum ? parentSeqNum : 0));
      m_fontChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
      return true;
    }
    m_contentChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
    m_unknownChunkIndices.push_back(unsigned(m_contentChunks.size() - 1));
  }
  return false;
}

bool MSPUBParser::isBlockDataString(unsigned type)
{
  return type == STRING_CONTAINER;
}
void MSPUBParser::skipBlock(librevenge::RVNGInputStream *input, MSPUBBlockInfo block)
{
  input->seek(block.dataOffset + block.dataLength, librevenge::RVNG_SEEK_SET);
}

EscherContainerInfo MSPUBParser::parseEscherContainer(librevenge::RVNGInputStream *input)
{
  EscherContainerInfo info;
  info.initial = readU16(input);
  info.type = readU16(input);
  info.contentsLength = readU32(input);
  info.contentsOffset = input->tell();
  MSPUB_DEBUG_MSG(("Parsed escher container: type 0x%x, contentsOffset 0x%lx, contentsLength 0x%lx\n", info.type, info.contentsOffset, info.contentsLength));
  return info;
}

MSPUBBlockInfo MSPUBParser::parseBlock(librevenge::RVNGInputStream *input, bool skipHierarchicalData)
{
  MSPUBBlockInfo info;
  info.startPosition = input->tell();
  info.id = (MSPUBBlockID)readU8(input);
  info.type = (MSPUBBlockType)readU8(input);
  info.dataOffset = input->tell();
  int len = getBlockDataLength(info.type);
  bool varLen = len < 0;
  if (varLen)
  {
    info.dataLength = readU32(input);
    if (isBlockDataString(info.type))
    {
      info.stringData = std::vector<unsigned char>();
      readNBytes(input, info.dataLength - 4, info.stringData);
    }
    else if (skipHierarchicalData)
    {
      skipBlock(input, info);
    }
    info.data = 0;
  }
  else
  {
    info.dataLength = len;
    switch (info.dataLength)
    {
    case 1:
      info.data = readU8(input);
      break;
    case 2:
      info.data = readU16(input);
      break;
    case 4:
      info.data = readU32(input);
      break;
    case 8:
    case 16:
    case 24:
      //FIXME: Not doing anything with this data for now.
      skipBlock(input, info);
    // fall-through intended
    default:
      info.data = 0;
    }
  }
  MSPUB_DEBUG_MSG(("parseBlock dataOffset 0x%lx, id 0x%x, type 0x%x, dataLength 0x%lx, integral data 0x%x\n", info.dataOffset, info.id, info.type, info.dataLength, info.data));
  return info;
}

PageType MSPUBParser::getPageTypeBySeqNum(unsigned seqNum)
{
  switch (seqNum)
  {
  case 0x10d:
  case 0x110:
  case 0x113:
  case 0x117:
    return DUMMY_PAGE;
  default:
    return NORMAL;
  }
}

bool MSPUBParser::parsePaletteChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk)
{
  unsigned length = readU32(input);
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input);
    if (info.type == 0xA0)
    {
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo subInfo = parseBlock(input);
        if (subInfo.type == GENERAL_CONTAINER)
        {
          parsePaletteEntry(input, subInfo);
        }
        else if (subInfo.type == DUMMY)
        {
          m_collector->addPaletteColor(Color());
        }
        skipBlock(input, subInfo);
      }
    }
    skipBlock(input, info);
  }
  return true;
}

void MSPUBParser::parsePaletteEntry(librevenge::RVNGInputStream *input, MSPUBBlockInfo info)
{
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.id == 0x01)
    {
      m_collector->addPaletteColor(Color(subInfo.data & 0xFF, (subInfo.data >> 8) & 0xFF, (subInfo.data >> 16) & 0xFF));
    }
  }
}

bool MSPUBParser::parseMetaData()
{
  m_input->seek(0, librevenge::RVNG_SEEK_SET);
  MSPUBMetaData metaData;

  std::unique_ptr<librevenge::RVNGInputStream> sumaryInfo(m_input->getSubStreamByName("\x05SummaryInformation"));
  if (sumaryInfo)
  {
    metaData.parse(sumaryInfo.get());
  }

  std::unique_ptr<librevenge::RVNGInputStream> docSumaryInfo(m_input->getSubStreamByName("\005DocumentSummaryInformation"));
  if (docSumaryInfo)
  {
    metaData.parse(docSumaryInfo.get());
  }

  m_input->seek(0, librevenge::RVNG_SEEK_SET);
  metaData.parseTimes(m_input);
  m_collector->collectMetaData(metaData.getMetaData());

  return true;
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
