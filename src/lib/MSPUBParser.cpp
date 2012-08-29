/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* libmspub
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2012 Brennan Vincent <brennanv@email.arizona.edu>
 * Copyright (C) 2012 Fridrich Strba <fridrich.strba@bluewin.ch>
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include <sstream>
#include <string>
#include <algorithm>
#include <string.h>
#include <libwpd-stream/libwpd-stream.h>
#include <boost/cstdint.hpp>
#include <zlib.h>
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

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector),
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

libmspub::MSPUBParser::~MSPUBParser()
{
}

bool libmspub::MSPUBParser::lineExistsByFlagPointer(unsigned *flags,
    unsigned *geomFlags)
{
  return flags &&
         !(((*flags) & FLAG_USE_LINE) && !((*flags) & FLAG_LINE)) &&
         ((!geomFlags) || !((*geomFlags) & FLAG_GEOM_USE_LINE_OK)
          || ((*geomFlags) & FLAG_GEOM_LINE_OK));

}

unsigned libmspub::MSPUBParser::getColorIndexByQuillEntry(unsigned entry)
{
  return entry;
}

short libmspub::MSPUBParser::getBlockDataLength(unsigned type) // -1 for variable-length block with the data length as the first DWORD
{
  switch(type)
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

bool libmspub::MSPUBParser::parse()
{
  MSPUB_DEBUG_MSG(("***NOTE***: Where applicable, the meanings of block/chunk IDs and Types printed below may be found in:\n\t***MSPUBBlockType.h\n\t***MSPUBBlockID.h\n\t***MSPUBContentChunkType.h\n*****\n"));
  if (!m_input->isOLEStream())
    return false;
  WPXInputStream *quill = m_input->getDocumentOLEStream("Quill/QuillSub/CONTENTS");
  if (!quill)
  {
    MSPUB_DEBUG_MSG(("Couldn't get quill stream.\n"));
    return false;
  }
  if (!parseQuill(quill))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse quill stream.\n"));
    delete quill;
    return false;
  }
  delete quill;
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
  {
    MSPUB_DEBUG_MSG(("Couldn't get contents stream.\n"));
    return false;
  }
  if (!parseContents(contents))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse contents stream.\n"));
    delete contents;
    return false;
  }
  delete contents;
  WPXInputStream *escherDelay = m_input->getDocumentOLEStream("Escher/EscherDelayStm");
  if (escherDelay)
  {
    parseEscherDelay(escherDelay);
    delete escherDelay;
  }
  WPXInputStream *escher = m_input->getDocumentOLEStream("Escher/EscherStm");
  if (!escher)
  {
    MSPUB_DEBUG_MSG(("Couldn't get escher stream.\n"));
    return false;
  }
  if (!parseEscher(escher))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse escher stream.\n"));
    delete escher;
    return false;
  }
  delete escher;

  return m_collector->go();
}

libmspub::ImgType libmspub::MSPUBParser::imgTypeByBlipType(unsigned short type)
{
  switch (type)
  {
  case OFFICE_ART_BLIP_PNG:
    return PNG;
  case OFFICE_ART_BLIP_JPEG:
    return JPEG;
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

int libmspub::MSPUBParser::getStartOffset(ImgType type, unsigned short initial)
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
  case DIB:
    oneUid = recInstance == 0x7A8;
    offset = 0x11;
    break;
  default:
    break;
  }
  return offset + (oneUid ? 0 : 0x10);
}

bool libmspub::MSPUBParser::parseEscherDelay(WPXInputStream *input)
{
  while (stillReading (input, (unsigned long)-1))
  {
    EscherContainerInfo info = parseEscherContainer(input);
    if (imgTypeByBlipType(info.type) != UNKNOWN)
    {
      WPXBinaryData img;
      unsigned long toRead = info.contentsLength;
      input->seek(input->tell() + getStartOffset(imgTypeByBlipType(info.type), info.initial), WPX_SEEK_SET);
      while (toRead > 0 && stillReading(input, (unsigned long)-1))
      {
        unsigned long howManyRead = 0;
        const unsigned char *buf = input->read(toRead, howManyRead);
        img.append(buf, howManyRead);
        toRead -= howManyRead;
      }
      if (imgTypeByBlipType(info.type) == WMF || imgTypeByBlipType(info.type) == EMF)
      {
        img = inflateData(img);
      }
      else if (imgTypeByBlipType(info.type) == DIB)
      {
        // Reconstruct BMP header
        // cf. http://en.wikipedia.org/wiki/BMP_file_format , accessed 2012-5-31
        const unsigned char *buf = img.getDataBuffer();
        if (img.size() < 0x2E + 4)
        {
          ++m_lastAddedImage;
          MSPUB_DEBUG_MSG(("Garbage DIB at index 0x%x\n", m_lastAddedImage));
          input->seek(info.contentsOffset + info.contentsLength, WPX_SEEK_SET);
          continue;
        }
        unsigned short bitsPerPixel = readU16(buf, 0x0E);
        unsigned numPaletteColors = readU32(buf, 0x20);
        if (numPaletteColors == 0 && bitsPerPixel <= 8)
        {
          numPaletteColors = 1;
          for (int i = 0; i < bitsPerPixel; ++i)
          {
            numPaletteColors *= 2;
          }
        }

        WPXBinaryData tmpImg;
        tmpImg.append(0x42);
        tmpImg.append(0x4d);

        tmpImg.append((unsigned char)((img.size() + 14) & 0x000000ff));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0x0000ff00) >> 8));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0x00ff0000) >> 16));
        tmpImg.append((unsigned char)(((img.size() + 14) & 0xff000000) >> 24));

        tmpImg.append(0x00);
        tmpImg.append(0x00);
        tmpImg.append(0x00);
        tmpImg.append(0x00);

        tmpImg.append(0x36 + 4 * numPaletteColors);
        tmpImg.append(0x00);
        tmpImg.append(0x00);
        tmpImg.append(0x00);
        tmpImg.append(img);
        img = tmpImg;
      }
      m_collector->addImage(++m_lastAddedImage, imgTypeByBlipType(info.type), img);
    }
    else
    {
      ++m_lastAddedImage;
      MSPUB_DEBUG_MSG(("Image of unknown type at index 0x%x\n", m_lastAddedImage));
    }
    input->seek(info.contentsOffset + info.contentsLength, WPX_SEEK_SET);
  }
  return true;
}

bool libmspub::MSPUBParser::parseContents(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseContents\n"));
  input->seek(0x1a, WPX_SEEK_SET);
  unsigned trailerOffset = readU32(input);
  MSPUB_DEBUG_MSG(("MSPUBParser: trailerOffset %.8x\n", trailerOffset));
  input->seek(trailerOffset, WPX_SEEK_SET);
  unsigned trailerLength = readU32(input);
  for (unsigned i=0; i<3; i++)
  {
    libmspub::MSPUBBlockInfo trailerPart = parseBlock(input);
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
              m_contentChunks[m_contentChunks.size() - 2].end
              = m_contentChunks.back().offset;
            }
          }
        }
        else(skipBlock(input, m_blockInfo.back()));
      }
      if (!m_contentChunks.empty())
      {
        m_contentChunks.back().end = trailerPart.dataOffset + trailerPart.dataLength;
      }
      if (!m_documentChunkIndex.is_initialized())
      {
        return false;
      }
      const ContentChunkReference &documentChunk = m_contentChunks.at(m_documentChunkIndex.get());
      for (unsigned i_pal = 0; i_pal < m_paletteChunkIndices.size(); ++i_pal)
      {
        const ContentChunkReference &paletteChunk = m_contentChunks.at(m_paletteChunkIndices[i_pal]);
        input->seek(paletteChunk.offset, WPX_SEEK_SET);
        if (! parsePaletteChunk(input, paletteChunk))
        {
          return false;
        }
      }
      for (unsigned i_ba = 0; i_ba < m_borderArtChunkIndices.size();
           ++i_ba)
      {
        const ContentChunkReference &baChunk =
          m_contentChunks.at(m_borderArtChunkIndices[i_ba]);
        input->seek(baChunk.offset, WPX_SEEK_SET);
        if (!parseBorderArtChunk(input, baChunk))
        {
          return false;
        }
      }
      for (unsigned i_shape = 0; i_shape < m_shapeChunkIndices.size();
           ++i_shape)
      {
        const ContentChunkReference &shapeChunk =
          m_contentChunks.at(m_shapeChunkIndices[i_shape]);
        input->seek(shapeChunk.offset, WPX_SEEK_SET);
        if (!parseShape(input, shapeChunk))
        {
          return false;
        }
      }
      for (unsigned i_font = 0; i_font < m_fontChunkIndices.size();
           ++i_font)
      {
        const ContentChunkReference &fontChunk =
          m_contentChunks.at(m_fontChunkIndices[i_font]);
        input->seek(fontChunk.offset, WPX_SEEK_SET);
        if (!parseFontChunk(input, fontChunk))
        {
          return false;
        }
      }
      input->seek(documentChunk.offset, WPX_SEEK_SET);
      if (!parseDocumentChunk(input, documentChunk))
      {
        return false;
      }
      for (unsigned i_page = 0; i_page < m_pageChunkIndices.size(); ++i_page)
      {
        const ContentChunkReference &pageChunk = m_contentChunks.at(m_pageChunkIndices[i_page]);
        input->seek(pageChunk.offset, WPX_SEEK_SET);
        if (!parsePageChunk(input, pageChunk))
        {
          return false;
        }
      }
    }
  }
  input->seek(trailerOffset + trailerLength, WPX_SEEK_SET);

  return true;
}

#ifdef DEBUG
bool libmspub::MSPUBParser::parseDocumentChunk(WPXInputStream *input, const ContentChunkReference &chunk)
#else
bool libmspub::MSPUBParser::parseDocumentChunk(WPXInputStream *input, const ContentChunkReference &)
#endif
{
  MSPUB_DEBUG_MSG(("parseDocumentChunk: offset 0x%lx, end 0x%lx\n", input->tell(), chunk.end));
  unsigned long begin = input->tell();
  unsigned long len = readU32(input);
  while (stillReading(input, begin + len))
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input);
    if (info.id == DOCUMENT_SIZE)
    {
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        libmspub::MSPUBBlockInfo subInfo = parseBlock(input, true);
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
      input->seek(info.dataOffset + 4, WPX_SEEK_SET);
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

bool libmspub::MSPUBParser::parseFontChunk(
  WPXInputStream *input, const ContentChunkReference &chunk)
{
  unsigned length = readU32(input);
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    if (info.id == FONT_CONTAINER_ARRAY)
    {
      input->seek(info.dataOffset + 4, WPX_SEEK_SET);
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo subInfo = parseBlock(input, true);
        if (subInfo.id == 0)
        {
          boost::optional<WPXString> name;
          boost::optional<unsigned> eotOffset;
          input->seek(subInfo.dataOffset + 4, WPX_SEEK_SET);
          while (stillReading(input, subInfo.dataOffset + subInfo.dataLength))
          {
            MSPUBBlockInfo subSubInfo = parseBlock(input, true);
            if (subSubInfo.id == EMBEDDED_FONT_NAME)
            {
              name = WPXString();
              appendCharacters(name.get(), subSubInfo.stringData, UTF_16);
            }
            else if (subSubInfo.id == EMBEDDED_EOT)
            {
              eotOffset = subSubInfo.dataOffset;
            }
          }
          if (name.is_initialized() && eotOffset.is_initialized())
          {
            input->seek(eotOffset.get(), WPX_SEEK_SET);
            MSPUBBlockInfo eotRecord = parseBlock(input, true);
            WPXBinaryData &data = m_collector->addEOTFont(name.get());
            unsigned long toRead = eotRecord.dataLength;
            while (toRead > 0 && stillReading(input, (unsigned long)-1))
            {
              unsigned long howManyRead = 0;
              const unsigned char *buf = input->read(toRead, howManyRead);
              data.append(buf, howManyRead);
              toRead -= howManyRead;
            }
            input->seek(subInfo.dataOffset + subInfo.dataLength, WPX_SEEK_SET);
          }
        }
      }
    }
  }
  return true;
}

bool libmspub::MSPUBParser::parseBorderArtChunk(
  WPXInputStream *input, const ContentChunkReference &chunk)
{
  unsigned length = readU32(input);
  while (stillReading(input, chunk.offset + length))
  {
    MSPUBBlockInfo info = parseBlock(input, true);
    if (info.id == BA_ARRAY)
    {
      input->seek(info.dataOffset + 4, WPX_SEEK_SET);
      unsigned i = 0;
      while (stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo entry = parseBlock(input, false);
        while (stillReading(input, entry.dataOffset + entry.dataLength))
        {
          MSPUBBlockInfo subRecord = parseBlock(input, true);
          if (subRecord.id == BA_IMAGE_ARRAY)
          {
            input->seek(subRecord.dataOffset + 4, WPX_SEEK_SET);
            while (stillReading(input, subRecord.dataOffset + subRecord.dataLength))
            {
              MSPUBBlockInfo subSubRecord = parseBlock(input, false);
              if (subSubRecord.id == BA_IMAGE_CONTAINER)
              {
                MSPUBBlockInfo imgRecord = parseBlock(input, false);
                if (imgRecord.id == BA_IMAGE)
                {
                  WPXBinaryData &img = *(m_collector->addBorderImage(
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
            input->seek(subRecord.dataOffset + 4, WPX_SEEK_SET);
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
        input->seek(entry.dataOffset + entry.dataLength, WPX_SEEK_SET);
      }
    }
  }
  return true;
}

bool libmspub::MSPUBParser::parsePageChunk(WPXInputStream *input, const ContentChunkReference &chunk)
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
    libmspub::MSPUBBlockInfo info = parseBlock(input);
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
      for (unsigned i = 0; i < info.stringData.size(); ++i)
      {
        if (info.stringData[i] != 0)
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

bool libmspub::MSPUBParser::parsePageShapeList(WPXInputStream *input, libmspub::MSPUBBlockInfo info, unsigned pageSeqNum)
{
  MSPUB_DEBUG_MSG(("parsePageShapeList: page seqnum 0x%x\n", pageSeqNum));
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    libmspub::MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == SHAPE_SEQNUM)
    {
      m_collector->setShapePage(subInfo.data, pageSeqNum);
    }
  }
  return true;
}

bool libmspub::MSPUBParser::parseShape(WPXInputStream *input,
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
    while (stillReading(input, pos + length))
    {
      libmspub::MSPUBBlockInfo info = parseBlock(input, true);
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
    }
    if (cellsSeqNum.is_initialized() && numRows.is_initialized() &&
        numCols.is_initialized() && rowcolArrayOffset.is_initialized())
    {
      unsigned nr = numRows.get();
      unsigned nc = numCols.get();
      unsigned rcao = rowcolArrayOffset.get();
      unsigned csn = cellsSeqNum.get();
      std::vector<unsigned> rowOffsetsInEmu;
      std::vector<unsigned> columnOffsetsInEmu;
      unsigned rowFirstOffset = 0;
      unsigned columnFirstOffset = 0;
      input->seek(rcao, WPX_SEEK_SET);
      unsigned arrayLength = readU32(input);
      while(stillReading(input, rcao + arrayLength))
      {
        MSPUBBlockInfo info = parseBlock(input, true);
        if (info.id == 0)
        {
          input->seek(info.dataOffset + 4, WPX_SEEK_SET);
          while (stillReading(input, info.dataOffset + info.dataLength))
          {
            MSPUBBlockInfo subInfo = parseBlock(input, true);
            if (subInfo.id == TABLE_ROWCOL_OFFSET)
            {
              unsigned rowcolOffset = readU32(input);
              if (columnOffsetsInEmu.size() < nc)
              {
                if (columnOffsetsInEmu.empty())
                {
                  columnFirstOffset = rowcolOffset;
                }
                columnOffsetsInEmu.push_back(rowcolOffset - columnFirstOffset);
              }
              else if (rowOffsetsInEmu.size() < nr)
              {
                if (rowOffsetsInEmu.empty())
                {
                  rowFirstOffset = rowcolOffset;
                }
                rowOffsetsInEmu.push_back(rowcolOffset - rowFirstOffset);
              }
            }
          }
        }
      }
      if (rowOffsetsInEmu.size() != nr || columnOffsetsInEmu.size() != nc)
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
      if (!index.is_initialized())
      {
        MSPUB_DEBUG_MSG(("WARNING: Couldn't find cells of seqnum %d corresponding to table of seqnum %d.\n",
                         csn, chunk.seqNum));
        return false;
      }
      else
      {
        // Currently do nothing with the cells chunk.
      }
      TableInfo ti(nr, nc);
      ti.m_rowOffsetsInEmu = rowOffsetsInEmu;
      ti.m_columnOffsetsInEmu = columnOffsetsInEmu;
      m_collector->setShapeTableInfo(chunk.seqNum, ti);
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
      libmspub::MSPUBBlockInfo info = parseBlock(input, true);
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

libmspub::QuillChunkReference libmspub::MSPUBParser::parseQuillChunkReference(WPXInputStream *input)
{
  libmspub::QuillChunkReference ret;
  readU16(input); //FIXME: Can we do something sensible if this is not 0x18 ?
  char name[5];
  for (int i = 0; i < 4; ++i)
  {
    name[i] = (char)readU8(input);
  }
  name[4] = '\0';
  ret.name = name;
  ret.id = readU16(input);
  input->seek(input->tell() + 4, WPX_SEEK_SET); //Seek past what is normally 0x01000000. We don't know what this represents.
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

std::vector<unsigned> libmspub::MSPUBParser::parseTableCellDefinitions(
  WPXInputStream *input, const QuillChunkReference &chunk)
{
  std::vector<unsigned> ret;
  unsigned numElements = readU32(input) + 1;
  input->seek(chunk.offset + 0xC, WPX_SEEK_SET);
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

bool libmspub::MSPUBParser::parseQuill(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseQuill\n"));
  unsigned chunkReferenceListOffset = 0x18;
  std::list<QuillChunkReference> chunkReferences;
  while (chunkReferenceListOffset != 0xffffffff)
  {
    input->seek(chunkReferenceListOffset + 2, WPX_SEEK_SET);
    unsigned short numChunks = readU16(input);
    chunkReferenceListOffset = readU32(input);
    for (unsigned i = 0; i < numChunks; ++i)
    {
      libmspub::QuillChunkReference quillChunkReference = parseQuillChunkReference(input);
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
      input->seek(i->offset, WPX_SEEK_SET);
      unsigned numLengths = readU32(input); //Assuming the first DWORD is the number of children and that the next is the remaining length before children start. We are unsure that this is correct.
      input->seek(4 + i->offset + readU32(input), WPX_SEEK_SET);
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
      input->seek(i->offset, WPX_SEEK_SET);
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
      input->seek(i->offset, WPX_SEEK_SET);
      parseColors(input, *i);
    }
    else if (i->name == "FDPC")
    {
      input->seek(i->offset, WPX_SEEK_SET);
      std::vector<TextSpanReference> thisBlockSpans = parseCharacterStyles(input, *i);
      spans.insert(spans.end(), thisBlockSpans.begin(), thisBlockSpans.end());
      parsedFdpc = true;
    }
    else if (i->name == "FDPP")
    {
      input->seek(i->offset, WPX_SEEK_SET);
      std::vector<TextParagraphReference> thisBlockParas = parseParagraphStyles(input, *i);
      paras.insert(paras.end(), thisBlockParas.begin(), thisBlockParas.end());
      parsedFdpp = true;
    }
    else if (i->name == "STSH")
    {
      if (whichStsh++ == 1)
      {
        input->seek(i->offset, WPX_SEEK_SET);
        parseDefaultStyle(input, *i);
        parsedStsh = true;
      }
    }
    else if (i->name == "FONT")
    {
      input->seek(i->offset, WPX_SEEK_SET);
      parseFonts(input, *i);
      parsedFont = true;
    }
    else if (i->name == "TCD ")
    {
      input->seek(i->offset, WPX_SEEK_SET);
      std::vector<unsigned> ends = parseTableCellDefinitions(input, *i);
      m_collector->setNextTableCellTextEnds(ends);
    }
  }
  if (parsedStrs && parsedSyid && parsedFdpc && parsedFdpp && parsedStsh && parsedFont && textChunkReference != chunkReferences.end())
  {
    input->seek(textChunkReference->offset, WPX_SEEK_SET);
    unsigned bytesRead = 0;
    std::vector<TextSpanReference>::iterator currentTextSpan = spans.begin();
    std::vector<TextParagraphReference>::iterator currentTextPara = paras.begin();
    for (unsigned j = 0; j < textIDs.size() && j < textLengths.size(); ++j)
    {
      MSPUB_DEBUG_MSG(("Parsing a text block.\n"));
      std::vector<TextParagraph> readParas;
      std::vector<TextSpan> readSpans;
      std::vector<unsigned char> text;
      for (unsigned k = 0; k < textLengths[j]; ++k)
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
      if (!readSpans.empty())
      {
        if (!text.empty())
        {
          readSpans.push_back(TextSpan(text, currentTextSpan->charStyle));
          MSPUB_DEBUG_MSG(("Saw text span %d in the current text paragraph.\n", (unsigned)readSpans.size()));
        }
        text.clear();
        readParas.push_back(TextParagraph(readSpans, currentTextPara->paraStyle));
        MSPUB_DEBUG_MSG(("Saw paragraph %d in the current text block.\n", (unsigned)readParas.size()));
      }
      m_collector->addTextString(readParas, textIDs[j]);
      m_collector->setTextStringOffset(textIDs[j], textOffsets[j]);
    }
    textChunkReference = chunkReferences.end();
  }
  return true;
}

void libmspub::MSPUBParser::parseFonts(WPXInputStream *input, const QuillChunkReference &)
{
  readU32(input);
  unsigned numElements = readU32(input);
  input->seek(input->tell() + 12 + 4 * numElements, WPX_SEEK_SET);
  for (unsigned i = 0; i < numElements; ++i)
  {
    unsigned short nameLength = readU16(input);
    std::vector<unsigned char> name;
    readNBytes(input, nameLength * 2, name);
    m_collector->addFont(name);
    readU32(input);
  }
}

void libmspub::MSPUBParser::parseDefaultStyle(WPXInputStream *input, const QuillChunkReference &chunk)
{
  readU32(input);
  unsigned numElements = readU32(input);
  input->seek(input->tell() + 12, WPX_SEEK_SET);
  std::vector<unsigned> offsets;
  offsets.reserve(numElements);
  for (unsigned i = 0; i < numElements; ++i)
  {
    offsets.push_back(readU32(input));
  }
  for (unsigned i = 0; i < numElements; ++i)
  {
    input->seek(chunk.offset + 20 + offsets[i], WPX_SEEK_SET);
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


void libmspub::MSPUBParser::parseColors(WPXInputStream *input, const QuillChunkReference &)
{
  unsigned numEntries = readU32(input);
  input->seek(input->tell() + 8, WPX_SEEK_SET);
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

std::vector<libmspub::MSPUBParser::TextParagraphReference> libmspub::MSPUBParser::parseParagraphStyles(WPXInputStream *input, const QuillChunkReference &chunk)
{
  std::vector<TextParagraphReference> ret;
  unsigned short numEntries = readU16(input);
  input->seek(input->tell() + 6, WPX_SEEK_SET);
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
    input->seek(chunk.offset + chunkOffsets[i], WPX_SEEK_SET);
    ParagraphStyle style = getParagraphStyle(input);
    ret.push_back(TextParagraphReference(currentSpanBegin, textOffsets[i], style));
    currentSpanBegin = textOffsets[i] + 1;
  }
  return ret;
}

std::vector<libmspub::MSPUBParser::TextSpanReference> libmspub::MSPUBParser::parseCharacterStyles(WPXInputStream *input, const QuillChunkReference &chunk)
{
  unsigned short numEntries = readU16(input);
  input->seek(input->tell() + 6, WPX_SEEK_SET);
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
    input->seek(chunk.offset + chunkOffsets[i], WPX_SEEK_SET);
    CharacterStyle style = getCharacterStyle(input);
    currentSpanBegin = textOffsets[i] + 1;
    ret.push_back(TextSpanReference(currentSpanBegin, textOffsets[i], style));
  }
  return ret;
}
libmspub::ParagraphStyle libmspub::MSPUBParser::getParagraphStyle(WPXInputStream *input)
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
    switch(info.id)
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
      input->seek(info.dataOffset + 4, WPX_SEEK_SET);
      while(stillReading(input, info.dataOffset + info.dataLength))
      {
        MSPUBBlockInfo tabArrayInfo = parseBlock(input, true);
        if (tabArrayInfo.id == TAB_ARRAY)
        {
          input->seek(tabArrayInfo.dataOffset + 4, WPX_SEEK_SET);
          while (stillReading(input, tabArrayInfo.dataOffset + tabArrayInfo.dataLength))
          {
            MSPUBBlockInfo tabEntryInfo = parseBlock(input, true);
            if (tabEntryInfo.type == GENERAL_CONTAINER)
            {
              input->seek(tabEntryInfo.dataOffset + 4, WPX_SEEK_SET);
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
      input->seek(info.dataOffset + 4, WPX_SEEK_SET);
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

libmspub::CharacterStyle libmspub::MSPUBParser::getCharacterStyle(WPXInputStream *input)
{
  bool seenUnderline = false, seenBold1 = false, seenBold2 = false, seenItalic1 = false, seenItalic2 = false;
  int textSize1 = -1, /* textSize2 = -1,*/ colorIndex = -1;
  boost::optional<unsigned> fontIndex;
  SuperSubType sst = NO_SUPER_SUB;
  unsigned offset = input->tell();
  unsigned len = readU32(input);
  while (stillReading(input, offset + len))
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input, true);
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
    dTextSize = (double)(textSize1 * POINTS_IN_INCH) / EMUS_IN_INCH;
  }
  return CharacterStyle(seenUnderline, seenItalic1 && seenItalic2, seenBold1 && seenBold2, dTextSize, getColorIndexByQuillEntry(colorIndex), fontIndex, sst);
}

unsigned libmspub::MSPUBParser::getFontIndex(WPXInputStream *input, const MSPUBBlockInfo &info)
{
  MSPUB_DEBUG_MSG(("In getFontIndex\n"));
  input->seek(info.dataOffset + 4, WPX_SEEK_SET);
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == GENERAL_CONTAINER)
    {
      input->seek(subInfo.dataOffset + 4, WPX_SEEK_SET);
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

int libmspub::MSPUBParser::getColorIndex(WPXInputStream *input, const MSPUBBlockInfo &info)
{
  input->seek(info.dataOffset + 4, WPX_SEEK_SET);
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

bool libmspub::MSPUBParser::parseEscher(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseEscher\n"));
  libmspub::EscherContainerInfo fakeroot;
  fakeroot.initial = 0;
  fakeroot.type = 0;
  fakeroot.contentsOffset = input->tell();
  fakeroot.contentsLength = (unsigned long)-1; //FIXME: Get the actual length
  libmspub::EscherContainerInfo dg, dgg;
  //Note: this assumes that dgg comes before any dg with images.
  if (findEscherContainer(input, fakeroot, dgg, OFFICE_ART_DGG_CONTAINER))
  {
    libmspub::EscherContainerInfo bsc;
    if (findEscherContainer(input, fakeroot, bsc, OFFICE_ART_B_STORE_CONTAINER))
    {
      unsigned short currentDelayIndex = 1;
      while (stillReading(input, bsc.contentsOffset + bsc.contentsLength))
      {
        unsigned begin = input->tell();
        input->seek(begin + 10, WPX_SEEK_SET);
        if (! (readU32(input) == 0 && readU32(input) == 0 && readU32(input) == 0 && readU32(input) == 0))
        {
          m_escherDelayIndices.push_back(currentDelayIndex++);
        }
        else
        {
          m_escherDelayIndices.push_back(-1);
        }
        input->seek(begin + 44, WPX_SEEK_SET);
      }
    }
    input->seek(dgg.contentsOffset + dgg.contentsLength + getEscherElementTailLength(OFFICE_ART_DGG_CONTAINER), WPX_SEEK_SET);
  }
  while (findEscherContainer(input, fakeroot, dg, OFFICE_ART_DG_CONTAINER))
  {
    libmspub::EscherContainerInfo spgr;
    while (findEscherContainer(input, dg, spgr, OFFICE_ART_SPGR_CONTAINER))
    {
      Coordinate c1, c2;
      parseShapeGroup(input, spgr, c1, c2);
    }
    input->seek(input->tell() + getEscherElementTailLength(OFFICE_ART_DG_CONTAINER), WPX_SEEK_SET);
  }
  return true;
}

void libmspub::MSPUBParser::parseShapeGroup(WPXInputStream *input, const EscherContainerInfo &spgr, Coordinate parentCoordinateSystem, Coordinate parentGroupAbsoluteCoord)
{
  libmspub::EscherContainerInfo shapeOrGroup;
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
    input->seek(shapeOrGroup.contentsOffset + shapeOrGroup.contentsLength + getEscherElementTailLength(shapeOrGroup.type), WPX_SEEK_SET);
  }
}

void libmspub::MSPUBParser::parseEscherShape(WPXInputStream *input, const EscherContainerInfo &sp, Coordinate &parentCoordinateSystem, Coordinate &parentGroupAbsoluteCoord)
{
  Coordinate thisParentCoordinateSystem = parentCoordinateSystem;
  bool definesRelativeCoordinates = false;
  libmspub::EscherContainerInfo cData;
  libmspub::EscherContainerInfo cAnchor;
  libmspub::EscherContainerInfo cFopt;
  libmspub::EscherContainerInfo cTertiaryFopt;
  libmspub::EscherContainerInfo cFsp;
  libmspub::EscherContainerInfo cFspgr;
  unsigned shapeFlags = 0;
  bool isGroupLeader = false;
  ShapeType st = RECTANGLE;
  if (findEscherContainer(input, sp, cFspgr, OFFICE_ART_FSPGR))
  {
    input->seek(cFspgr.contentsOffset, WPX_SEEK_SET);
    parentCoordinateSystem.m_xs = readU32(input);
    parentCoordinateSystem.m_ys = readU32(input);
    parentCoordinateSystem.m_xe = readU32(input);
    parentCoordinateSystem.m_ye = readU32(input);
    definesRelativeCoordinates = true;
  }
  input->seek(sp.contentsOffset, WPX_SEEK_SET);
  if (findEscherContainer(input, sp, cFsp, OFFICE_ART_FSP))
  {
    st = (ShapeType)(cFsp.initial >> 4);
    std::map<unsigned short, unsigned> fspData = extractEscherValues(input, cFsp);
    input->seek(cFsp.contentsOffset + 4, WPX_SEEK_SET);
    shapeFlags = readU32(input);
    isGroupLeader = shapeFlags & SF_GROUP;
  }
  input->seek(sp.contentsOffset, WPX_SEEK_SET);
  if (findEscherContainer(input, sp, cData, OFFICE_ART_CLIENT_DATA))
  {
    std::map<unsigned short, unsigned> dataValues = extractEscherValues(input, cData);
    unsigned *shapeSeqNum = getIfExists(dataValues, FIELDID_SHAPE_ID);
    if (shapeSeqNum)
    {
      m_collector->setShapeType(*shapeSeqNum, st);
      m_collector->setShapeFlip(*shapeSeqNum, shapeFlags & SF_FLIP_V, shapeFlags & SF_FLIP_H);
      input->seek(sp.contentsOffset, WPX_SEEK_SET);
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
        input->seek(sp.contentsOffset, WPX_SEEK_SET);
        if (findEscherContainer(input, sp, cTertiaryFopt, OFFICE_ART_TERTIARY_FOPT))
        {
          maybe_tertiaryFoptValues = extractEscherValues(input, cTertiaryFopt);
        }
        if (maybe_tertiaryFoptValues.is_initialized())
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
        input->seek(sp.contentsOffset, WPX_SEEK_SET);
        if (findEscherContainer(input, sp, cFopt, OFFICE_ART_FOPT))
        {
          FOPTValues foptValues = extractFOPTValues(input, cFopt);
          unsigned *pxId = getIfExists(foptValues.m_scalarValues, FIELDID_PXID);
          if (pxId)
          {
            MSPUB_DEBUG_MSG(("Current Escher shape has pxId %d\n", *pxId));
            if (*pxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*pxId - 1] >= 0)
            {
              m_collector->setShapeImgIndex(*shapeSeqNum, m_escherDelayIndices[*pxId - 1]);
            }
            else
            {
              MSPUB_DEBUG_MSG(("Couldn't find corresponding escherDelay index\n"));
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
          boost::shared_ptr<Fill> ptr_fill = getNewFill(foptValues.m_scalarValues, skipIfNotBg);
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
              if (maybe_tertiaryFoptValues.is_initialized())
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

          unsigned *ptr_numColumns = getIfExists(foptValues.m_scalarValues,
                                                 FIELDID_NUM_COLUMNS);
          if (ptr_numColumns)
          {
            m_collector->setShapeNumColumns(*shapeSeqNum, *ptr_numColumns);
          }
          unsigned *ptr_columnSpacing = getIfExists(foptValues.m_scalarValues,
                                        FIELDID_COLUMN_SPACING);
          if (ptr_columnSpacing)
          {
            m_collector->setShapeColumnSpacing(*shapeSeqNum, *ptr_columnSpacing);
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

          unsigned *ptr_shadowType = getIfExists(foptValues.m_scalarValues,
                                                 FIELDID_SHADOW_TYPE);
          if (ptr_shadowType)
          {
            ShadowType shadowType = static_cast<ShadowType>(*ptr_shadowType);
            unsigned *shadowColor = getIfExists(foptValues.m_scalarValues,
                                                FIELDID_SHADOW_COLOR);
            unsigned *shadowOpacity = getIfExists(foptValues.m_scalarValues,
                                                  FIELDID_SHADOW_OPACITY);
            unsigned *shadowOffsetX = getIfExists(foptValues.m_scalarValues,
                                                  FIELDID_SHADOW_OFFSET_X);
            unsigned *shadowOffsetY = getIfExists(foptValues.m_scalarValues,
                                                  FIELDID_SHADOW_OFFSET_Y);
            unsigned *shadowOriginX = getIfExists(foptValues.m_scalarValues,
                                                  FIELDID_SHADOW_ORIGIN_X);
            unsigned *shadowOriginY = getIfExists(foptValues.m_scalarValues,
                                                  FIELDID_SHADOW_ORIGIN_Y);
            /* unsigned *shadowBoolProps = getIfExists(foptValues.m_scalarValues,
                                        FIELDID_SHADOW_BOOL_PROPS); */
            m_collector->setShapeShadow(*shapeSeqNum, Shadow(shadowType,
                                        shadowOffsetX ? static_cast<int>(*shadowOffsetX) : 0x6338,
                                        shadowOffsetY ? static_cast<int>(*shadowOffsetY) : 0x6338,
                                        shadowOriginX ? toFixedPoint(static_cast<int>(*shadowOriginX)) : 0,
                                        shadowOriginY ? toFixedPoint(static_cast<int>(*shadowOriginY)) : 0,
                                        toFixedPoint(shadowOpacity ? static_cast<int>(*shadowOpacity)
                                            : 0x10000),
                                        ColorReference(shadowColor ? *shadowColor : 0)));


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
            input->seek(cAnchor.contentsOffset, WPX_SEEK_SET);
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

boost::shared_ptr<libmspub::Fill> libmspub::MSPUBParser::getNewFill(const std::map<unsigned short, unsigned> &foptProperties,
    bool &skipIfNotBg)
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
      return boost::shared_ptr<Fill>(new SolidFill(ColorReference(*ptr_fillColor), ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1, m_collector));
    }
    return boost::shared_ptr<Fill>();
  }
  case GRADIENT: //FIXME: The handling of multi-color gradients here is quite bad.
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

    boost::shared_ptr<GradientFill> ret(new GradientFill(m_collector, angle));
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
      ret->addColor(firstColor, 0, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
      ret->addColor(secondColor, fillFocus, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
      ret->addColor(firstColor, 100, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
    }
    else if (fillFocus < 0)
    {
      ret->addColor(secondColor, 0, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
      ret->addColor(firstColor, 100 + fillFocus, ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1);
      ret->addColor(secondColor, 100, ptr_fillBackOpacity ? (double)(*ptr_fillBackOpacity) / 0xFFFF : 1);
    }
    return ret;
  }
  case TEXTURE:
  case BITMAP:
  {
    const unsigned *ptr_bgPxId = getIfExists_const(foptProperties, FIELDID_BG_PXID);
    if (ptr_bgPxId && *ptr_bgPxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*ptr_bgPxId - 1] >= 0)
    {
      return boost::shared_ptr<Fill>(new ImgFill(m_escherDelayIndices[*ptr_bgPxId - 1], m_collector, fillType == TEXTURE));
    }
    return boost::shared_ptr<Fill>();
  }
  case PATTERN:
  {
    const unsigned *ptr_bgPxId = getIfExists_const(foptProperties, FIELDID_BG_PXID);
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fillBackColor = getIfExists_const(foptProperties, FIELDID_FILL_BACK_COLOR);
    ColorReference fill = ptr_fillColor ? ColorReference(*ptr_fillColor) : ColorReference(0x00FFFFFF);
    ColorReference back = ptr_fillBackColor ? ColorReference(*ptr_fillBackColor) : ColorReference(0x08000000);
    if (ptr_bgPxId && *ptr_bgPxId <= m_escherDelayIndices.size() && m_escherDelayIndices[*ptr_bgPxId - 1 ] >= 0)
    {
      return boost::shared_ptr<Fill>(new PatternFill(m_escherDelayIndices[*ptr_bgPxId - 1], m_collector, fill, back));
    }
  }
  default:
    return boost::shared_ptr<Fill>();
  }
}

libmspub::DynamicCustomShape libmspub::MSPUBParser::getDynamicCustomShape(
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

std::vector<unsigned short> libmspub::MSPUBParser::parseSegments(
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

std::vector<libmspub::Calculation> libmspub::MSPUBParser::parseGuides(
  const std::vector<unsigned char> &/* guideData */)
{
  std::vector<Calculation> ret;

  //FIXME : implement this function.

  return ret;
}

std::vector<libmspub::Vertex> libmspub::MSPUBParser::parseVertices(
  const std::vector<unsigned char> &vertexData)
{
  std::vector<libmspub::Vertex> ret;
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
  if (! (entrySize == 2 || entrySize == 4 || entrySize == 8))
  {
    MSPUB_DEBUG_MSG(("Incomprehensible entry size %d in vertex complex data!\n", entrySize));
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
    libmspub::Vertex v = {x, y};
    ret.push_back(v);
    offset += entrySize;
  }
  return ret;
}

unsigned libmspub::MSPUBParser::getEscherElementTailLength(unsigned short type)
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

unsigned libmspub::MSPUBParser::getEscherElementAdditionalHeaderLength(unsigned short type)
{
  switch (type)
  {
  case OFFICE_ART_CLIENT_ANCHOR:
  case OFFICE_ART_CLIENT_DATA: //account for the fact that the length appears twice, for whatever reason
    return 4;
  }
  return 0;
}

bool libmspub::MSPUBParser::findEscherContainerWithTypeInSet(WPXInputStream *input, const libmspub::EscherContainerInfo &parent, libmspub::EscherContainerInfo &out, std::set<unsigned short> types)
{
  while (stillReading(input, parent.contentsOffset + parent.contentsLength))
  {
    libmspub::EscherContainerInfo next = parseEscherContainer(input);
    if (types.find(next.type) != types.end())
    {
      out = next;
      return true;
    }
    input->seek(next.contentsOffset + next.contentsLength + getEscherElementTailLength(next.type), WPX_SEEK_SET);
  }
  return false;
}

bool libmspub::MSPUBParser::findEscherContainer(WPXInputStream *input, const libmspub::EscherContainerInfo &parent, libmspub::EscherContainerInfo &out, unsigned short desiredType)
{
  MSPUB_DEBUG_MSG(("At offset 0x%lx, attempting to find escher container of type 0x%x\n", input->tell(), desiredType));
  while (stillReading(input, parent.contentsOffset + parent.contentsLength))
  {
    libmspub::EscherContainerInfo next = parseEscherContainer(input);
    if (next.type == desiredType)
    {
      out = next;
      return true;
    }
    input->seek(next.contentsOffset + next.contentsLength + getEscherElementTailLength(next.type), WPX_SEEK_SET);
  }
  return false;
}

libmspub::FOPTValues libmspub::MSPUBParser::extractFOPTValues(WPXInputStream *input, const libmspub::EscherContainerInfo &record)
{
  FOPTValues ret;
  input->seek(record.contentsOffset, WPX_SEEK_SET);
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
  for (unsigned i = 0; i < complexIds.size(); ++i)
  {
    if (!stillReading(input, record.contentsOffset + record.contentsLength))
    {
      break;
    }
    unsigned short id = complexIds[i];
    unsigned length = ret.m_scalarValues[id];
    if (!length)
    {
      continue;
    }
    unsigned short numEntries = readU16(input);
    input->seek(2, WPX_SEEK_CUR);
    unsigned short entryLength = readU16(input);
    if (entryLength == 0xFFF0)
    {
      entryLength = 4;
    }
    input->seek(-6, WPX_SEEK_CUR);
    readNBytes(input, entryLength * numEntries + 6, ret.m_complexValues[id]);
  }
  return ret;
}

std::map<unsigned short, unsigned> libmspub::MSPUBParser::extractEscherValues(WPXInputStream *input, const libmspub::EscherContainerInfo &record)
{
  std::map<unsigned short, unsigned> ret;
  input->seek(record.contentsOffset + getEscherElementAdditionalHeaderLength(record.type), WPX_SEEK_SET);
  while (stillReading(input, record.contentsOffset + record.contentsLength))
  {
    unsigned short id = readU16(input);
    unsigned value = readU32(input);
    ret[id] = value;
  }
  return ret;
}


bool libmspub::MSPUBParser::parseContentChunkReference(WPXInputStream *input, const libmspub::MSPUBBlockInfo block)
{
  //input should be at block.dataOffset + 4 , that is, at the beginning of the list of sub-blocks
  MSPUB_DEBUG_MSG(("Parsing chunk reference 0x%x\n", m_lastSeenSeqNum));
  libmspub::MSPUBContentChunkType type = (libmspub::MSPUBContentChunkType)0;
  unsigned long offset = 0;
  unsigned parentSeqNum = 0;
  bool seenType = false;
  bool seenOffset = false;
  bool seenParentSeqNum = false;
  while (stillReading(input, block.dataOffset + block.dataLength))
  {
    libmspub::MSPUBBlockInfo subBlock = parseBlock(input, true);
    //FIXME: Warn if multiple of these blocks seen.
    if (subBlock.id == CHUNK_TYPE)
    {
      type = (libmspub::MSPUBContentChunkType)subBlock.data;
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

bool libmspub::MSPUBParser::isBlockDataString(unsigned type)
{
  return type == STRING_CONTAINER;
}
void libmspub::MSPUBParser::skipBlock(WPXInputStream *input, libmspub::MSPUBBlockInfo block)
{
  input->seek(block.dataOffset + block.dataLength, WPX_SEEK_SET);
}

libmspub::EscherContainerInfo libmspub::MSPUBParser::parseEscherContainer(WPXInputStream *input)
{
  libmspub::EscherContainerInfo info;
  info.initial = readU16(input);
  info.type = readU16(input);
  info.contentsLength = readU32(input);
  info.contentsOffset = input->tell();
  MSPUB_DEBUG_MSG(("Parsed escher container: type 0x%x, contentsOffset 0x%lx, contentsLength 0x%lx\n", info.type, info.contentsOffset, info.contentsLength));
  return info;
}

libmspub::MSPUBBlockInfo libmspub::MSPUBParser::parseBlock(WPXInputStream *input, bool skipHierarchicalData)
{
  libmspub::MSPUBBlockInfo info;
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
    default:
      info.data = 0;
    }
  }
  MSPUB_DEBUG_MSG(("parseBlock dataOffset 0x%lx, id 0x%x, type 0x%x, dataLength 0x%lx, integral data 0x%x\n", info.dataOffset, info.id, info.type, info.dataLength, info.data));
  return info;
}

libmspub::PageType libmspub::MSPUBParser::getPageTypeBySeqNum(unsigned seqNum)
{
  switch(seqNum)
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

bool libmspub::MSPUBParser::parsePaletteChunk(WPXInputStream *input, const ContentChunkReference &chunk)
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

void libmspub::MSPUBParser::parsePaletteEntry(WPXInputStream *input, MSPUBBlockInfo info)
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

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
