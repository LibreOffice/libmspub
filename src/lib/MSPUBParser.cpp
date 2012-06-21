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
#include <libwpd-stream/WPXStream.h>
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
#include "Shapes.h"
#include "Fill.h"
#include "FillType.h"

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector), m_blockInfo(), m_pageChunks(), m_shapeChunks(),
    m_paletteChunks(), m_unknownChunks(),
    m_documentChunk(), m_lastSeenSeqNum(-1),
    m_lastAddedImage(0), m_seenDocumentChunk(false)
{
}

libmspub::MSPUBParser::~MSPUBParser()
{
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
      ContentChunkReference *lastSeen = NULL;

      while (stillReading(input, trailerPart.dataOffset + trailerPart.dataLength))
      {
        m_blockInfo.push_back(parseBlock(input));
        ++m_lastSeenSeqNum;
        if (m_blockInfo.back().type == GENERAL_CONTAINER)
        {
          ContentChunkReference *next = parseContentChunkReference(input, m_blockInfo.back());
          if (next)
          {
            if (lastSeen)
            {
              lastSeen->end = next->offset;
            }
            lastSeen = next;
          }
        }
        else(skipBlock(input, m_blockInfo.back()));
      }
      if (lastSeen)
      {
        lastSeen->end = trailerPart.dataOffset + trailerPart.dataLength;
      }
      if (!m_seenDocumentChunk)
      {
        return false;
      }
      for (ccr_iterator_t iter1 = m_paletteChunks.begin(); iter1 != m_paletteChunks.end(); ++iter1)
      {
        input->seek(iter1->offset, WPX_SEEK_SET);
        if (! parsePaletteChunk(input, *iter1))
        {
          return false;
        }
      }
      input->seek(m_documentChunk.offset, WPX_SEEK_SET);
      if (!parseDocumentChunk(input, m_documentChunk))
      {
        return false;
      }
      for (ccr_iterator_t iter2 = m_pageChunks.begin(); iter2 != m_pageChunks.end(); ++iter2)
      {
        input->seek(iter2->offset, WPX_SEEK_SET);
        if (!parsePageChunk(input, *iter2))
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
    else
    {
      skipBlock(input, info);
    }
  }
  return true; //FIXME: return false for failure
}


bool libmspub::MSPUBParser::parsePageChunk(WPXInputStream *input, const ContentChunkReference &chunk)
{
  MSPUB_DEBUG_MSG(("parsePageChunk: offset 0x%lx, end 0x%lx, seqnum 0x%x, parent 0x%x\n", input->tell(), chunk.end, chunk.seqNum, chunk.parentSeqNum));
  unsigned long length = readU32(input);
  if (getPageTypeBySeqNum(chunk.seqNum) == NORMAL)
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
      parseShapes(input, info, chunk.seqNum);
    }
    else
    {
      skipBlock(input, info);
    }
  }
  return true;
}

class FindBySeqNum
{
  unsigned seqNum;
public:
  FindBySeqNum(unsigned sn) : seqNum(sn) { }
  bool operator()(const libmspub::ContentChunkReference &ref)
  {
    return ref.seqNum == seqNum;
  }
};

bool libmspub::MSPUBParser::parseShapes(WPXInputStream *input, libmspub::MSPUBBlockInfo info, unsigned pageSeqNum)
{
  MSPUB_DEBUG_MSG(("parseShapes: page seqnum 0x%x\n", pageSeqNum));
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    libmspub::MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == SHAPE_SEQNUM)
    {
      std::vector<libmspub::ContentChunkReference>::const_iterator ref = std::find_if(m_shapeChunks.begin(), m_shapeChunks.end(), FindBySeqNum(subInfo.data));
      if (ref == m_shapeChunks.end())
      {
        MSPUB_DEBUG_MSG(("Shape of seqnum 0x%x not found!\n", subInfo.data));
      }
      else
      {
        MSPUB_DEBUG_MSG(("Shape of seqnum 0x%x found\n", subInfo.data));
        unsigned long pos = input->tell();
        input->seek(ref->offset, WPX_SEEK_SET);
        parseShape(input, subInfo.data, pageSeqNum);
        input->seek(pos, WPX_SEEK_SET);
      }
    }
  }
  return true;
}

bool libmspub::MSPUBParser::parseShape(WPXInputStream *input, unsigned seqNum, unsigned pageSeqNum)
{
  MSPUB_DEBUG_MSG(("parseShape: pageSeqNum 0x%x\n", pageSeqNum));
  unsigned long pos = input->tell();
  unsigned length = readU32(input);
  unsigned width = 0;
  unsigned height = 0;
  bool isText = false;
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
    else if (info.id == SHAPE_TEXT_ID)
    {
      textId = info.data;
      isText = true;
    }
  }
  if (height > 0 && width > 0)
  {
    if (isText)
    {
      //FIXME : Should we do something with this redundant height and width? Or at least assert that it is equal?
      m_collector->addTextShape(textId, seqNum, pageSeqNum);
    }
    //else
    //{
    m_collector->addShape(seqNum, pageSeqNum);
    //}
  }
  else
  {
    MSPUB_DEBUG_MSG(("Height and width not both specified, ignoring. (Height: 0x%x, Width: 0x%x)\n", height, width));
  }
  return true;
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
  std::list<unsigned> textLengths;
  std::list<unsigned> textIDs;
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
        textLengths.push_back(readU32(input));
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
    if (parsedStrs && parsedSyid && parsedFdpc && parsedFdpp && parsedStsh && parsedFont && textChunkReference != chunkReferences.end())
    {
      input->seek(textChunkReference->offset, WPX_SEEK_SET);
      unsigned bytesRead = 0;
      std::vector<TextSpanReference>::iterator currentTextSpan = spans.begin();
      std::vector<TextParagraphReference>::iterator currentTextPara = paras.begin();
      for (std::list<unsigned>::const_iterator iter = textLengths.begin(), id = textIDs.begin(); iter != textLengths.end() && id != textIDs.end(); ++iter, ++id)
      {
        MSPUB_DEBUG_MSG(("Parsing a text block.\n"));
        std::vector<TextParagraph> readParas;
        std::vector<TextSpan> readSpans;
        std::vector<unsigned char> text;
        for (unsigned j = 0; j < *iter; ++j)
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
        m_collector->addTextString(readParas, *id);
      }
      textChunkReference = chunkReferences.end();
    }
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
      m_collector->addDefaultCharacterStyle(getCharacterStyle(input, true));
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
  Alignment align = (Alignment)-1;
  unsigned defaultCharStyleIndex = 0;
  unsigned offset = input->tell();
  unsigned len = readU32(input);
  while (stillReading(input, offset + len))
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input, true);
    switch(info.id)
    {
    case PARAGRAPH_ALIGNMENT:
      align = (Alignment)(info.data & 0xFF); // Is this correct?
      break;
    case PARAGRAPH_DEFAULT_CHAR_STYLE:
      defaultCharStyleIndex = info.data;
      break;
    default:
      break;
    }
  }
  return ParagraphStyle(align, defaultCharStyleIndex);
}
libmspub::CharacterStyle libmspub::MSPUBParser::getCharacterStyle(WPXInputStream *input, bool inStsh)
{
  bool seenUnderline = false, seenBold1 = false, seenBold2 = false, seenItalic1 = false, seenItalic2 = false;
  int textSize1 = -1, textSize2 = -1, colorIndex = -1;
  unsigned fontIndex = 0;
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
      textSize2 = info.data;
      break;
    case COLOR_INDEX_CONTAINER_ID:
      colorIndex = getColorIndex(input, info);
      break;
    case FONT_INDEX_CONTAINER_ID:
      if (! inStsh)
      {
        fontIndex = getFontIndex(input, info);
      }
      break;
    default:
      break;
    }
  }
  return CharacterStyle(seenUnderline, seenItalic1 && seenItalic2, seenBold1 && seenBold2, textSize1 == textSize2 && textSize1 >= 0 ? (double)(textSize1 * POINTS_IN_INCH) / EMUS_IN_INCH : -1, colorIndex, fontIndex);
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
  std::vector<int> escherDelayIndices;
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
          escherDelayIndices.push_back(currentDelayIndex++);
        }
        else
        {
          escherDelayIndices.push_back(-1);
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
      libmspub::EscherContainerInfo sp;
      while (findEscherContainer(input, spgr, sp, OFFICE_ART_SP_CONTAINER))
      {
        libmspub::EscherContainerInfo cData;
        libmspub::EscherContainerInfo cAnchor;
        libmspub::EscherContainerInfo cFopt;
        libmspub::EscherContainerInfo cFsp;
        if (findEscherContainer(input, sp, cData, OFFICE_ART_CLIENT_DATA))
        {
          std::map<unsigned short, unsigned> dataValues = extractEscherValues(input, cData);
          unsigned *shapeSeqNum = getIfExists(dataValues, FIELDID_SHAPE_ID);
          if (shapeSeqNum)
          {
            input->seek(sp.contentsOffset, WPX_SEEK_SET);
            m_collector->setShapeOrder(*shapeSeqNum);
            if (findEscherContainer(input, sp, cAnchor, OFFICE_ART_CLIENT_ANCHOR))
            {
              MSPUB_DEBUG_MSG(("Found Escher data for shape of seqnum 0x%x\n", *shapeSeqNum));
              input->seek(sp.contentsOffset, WPX_SEEK_SET);
              if (findEscherContainer(input, sp, cFopt, OFFICE_ART_FOPT))
              {
                std::map<unsigned short, unsigned> foptValues = extractEscherValues(input, cFopt);
                unsigned *pxId = getIfExists(foptValues, FIELDID_PXID);
                if (pxId)
                {
                  MSPUB_DEBUG_MSG(("Current Escher shape has pxId %d\n", *pxId));
                  if (*pxId <= escherDelayIndices.size() && escherDelayIndices[*pxId - 1] >= 0)
                  {
                    m_collector->setShapeImgIndex(*shapeSeqNum, escherDelayIndices[*pxId - 1]);
                  }
                  else
                  {
                    MSPUB_DEBUG_MSG(("Couldn't find corresponding escherDelay index\n"));
                  }
                }
                bool useLine = true;
                unsigned *ptr_lineColor = getIfExists(foptValues, FIELDID_LINE_COLOR);
                unsigned *ptr_lineFlags = getIfExists(foptValues, FIELDID_LINE_STYLE_BOOL_PROPS);
                if (ptr_lineFlags)
                {
                  if (((*ptr_lineFlags) & FLAG_USE_LINE) && !((*ptr_lineFlags) & FLAG_LINE))
                  {
                    useLine = false;
                  }
                }
                bool skipIfNotBg = false;
                Fill *ptr_fill = getNewFill(foptValues, escherDelayIndices, skipIfNotBg);
                if (ptr_lineColor && useLine)
                {
                  m_collector->setShapeLineColor(*shapeSeqNum, ColorReference(*ptr_lineColor));
                }
                if (ptr_fill)
                {
                  m_collector->setShapeFill(*shapeSeqNum, ptr_fill, skipIfNotBg);
                }
                int *ptr_adjust1 = (int *)getIfExists(foptValues, FIELDID_ADJUST_VALUE_1);
                int *ptr_adjust2 = (int *)getIfExists(foptValues, FIELDID_ADJUST_VALUE_2);
                int *ptr_adjust3 = (int *)getIfExists(foptValues, FIELDID_ADJUST_VALUE_3);
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
                int *ptr_rotation = (int *)getIfExists(foptValues, FIELDID_ROTATION);
                if (ptr_rotation)
                {
                  m_collector->setShapeRotation(*shapeSeqNum, (short)((*ptr_rotation) >> 16));
                }
                unsigned *ptr_left = getIfExists(foptValues, FIELDID_DY_TEXT_LEFT);
                unsigned *ptr_top = getIfExists(foptValues, FIELDID_DY_TEXT_TOP);
                unsigned *ptr_right = getIfExists(foptValues, FIELDID_DY_TEXT_RIGHT);
                unsigned *ptr_bottom = getIfExists(foptValues, FIELDID_DY_TEXT_BOTTOM);
                m_collector->setShapeMargins(*shapeSeqNum, ptr_left ? *ptr_left : DEFAULT_MARGIN,
                                             ptr_top ? *ptr_top : DEFAULT_MARGIN,
                                             ptr_right ? *ptr_right : DEFAULT_MARGIN,
                                             ptr_bottom ? *ptr_bottom : DEFAULT_MARGIN);
              }
              input->seek(sp.contentsOffset, WPX_SEEK_SET);
              if (findEscherContainer(input, sp, cFsp, OFFICE_ART_FSP))
              {
                m_collector->setShapeType(*shapeSeqNum, (ShapeType)(cFsp.initial >> 4));
                std::map<unsigned short, unsigned> fspData = extractEscherValues(input, cFsp);
                input->seek(cFsp.contentsOffset + 4, WPX_SEEK_SET);
                unsigned flags = readU32(input);
                m_collector->setShapeFlip(*shapeSeqNum, flags & SF_FLIP_V, flags & SF_FLIP_H);
              }

              std::map<unsigned short, unsigned> anchorData = extractEscherValues(input, cAnchor);
              m_collector->setShapeCoordinatesInEmu(*shapeSeqNum, anchorData[FIELDID_XS], anchorData[FIELDID_YS], anchorData[FIELDID_XE], anchorData[FIELDID_YE]);
              input->seek(sp.contentsOffset + sp.contentsLength + getEscherElementTailLength(sp.type), WPX_SEEK_SET);
            }
          }
        }
      }
    }
    input->seek(input->tell() + getEscherElementTailLength(OFFICE_ART_DG_CONTAINER), WPX_SEEK_SET);
  }
  return true;
}

libmspub::Fill *libmspub::MSPUBParser::getNewFill(const std::map<unsigned short, unsigned> &foptProperties,
    const std::vector<int> &escherDelayIndices, bool &skipIfNotBg)
{
  // don't worry about memory leaks; everything created here is deleted when the Collector goes out of scope.
  const FillType *ptr_fillType = (FillType *)getIfExists_const(foptProperties, FIELDID_FILL_TYPE);
  FillType fillType = ptr_fillType ? *ptr_fillType : SOLID;
  switch (fillType)
  {
  case SOLID:
  {
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fieldStyleProps = getIfExists_const(foptProperties, FIELDID_FIELD_STYLE_BOOL_PROPS);
    skipIfNotBg = ptr_fieldStyleProps && (*ptr_fieldStyleProps & 0xF0) == 0;
    if (ptr_fillColor)
    {
      const unsigned *ptr_fillOpacity = getIfExists_const(foptProperties, FIELDID_FILL_OPACITY);
      return new SolidFill(ColorReference(*ptr_fillColor), ptr_fillOpacity ? (double)(*ptr_fillOpacity) / 0xFFFF : 1, m_collector);
    }
    return NULL;
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

    GradientFill *ret = new GradientFill(m_collector, angle);
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
    if (ptr_bgPxId && *ptr_bgPxId <= escherDelayIndices.size() && escherDelayIndices[*ptr_bgPxId - 1] >= 0)
    {
      return new ImgFill(escherDelayIndices[*ptr_bgPxId - 1], m_collector, fillType == TEXTURE);
    }
    return NULL;
  }
  case PATTERN:
  {
    const unsigned *ptr_bgPxId = getIfExists_const(foptProperties, FIELDID_BG_PXID);
    const unsigned *ptr_fillColor = getIfExists_const(foptProperties, FIELDID_FILL_COLOR);
    const unsigned *ptr_fillBackColor = getIfExists_const(foptProperties, FIELDID_FILL_BACK_COLOR);
    ColorReference fill = ptr_fillColor ? ColorReference(*ptr_fillColor) : ColorReference(0x00FFFFFF);
    ColorReference back = ptr_fillBackColor ? ColorReference(*ptr_fillBackColor) : ColorReference(0x08000000);
    if (ptr_bgPxId && *ptr_bgPxId <= escherDelayIndices.size() && escherDelayIndices[*ptr_bgPxId - 1 ] >= 0)
    {
      return new PatternFill(escherDelayIndices[*ptr_bgPxId - 1], m_collector, fill, back);
    }
  }
  default:
    return NULL;
  }
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


libmspub::ContentChunkReference *libmspub::MSPUBParser::parseContentChunkReference(WPXInputStream *input, const libmspub::MSPUBBlockInfo block)
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
      m_pageChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      return &m_pageChunks.back();
    }
    else if (type == DOCUMENT)
    {
      MSPUB_DEBUG_MSG(("document chunk: offset 0x%lx, seqnum 0x%x\n", offset, m_lastSeenSeqNum));
      m_documentChunk = ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0);
      m_seenDocumentChunk = true;
      return &m_documentChunk;
    }
    else if (type == SHAPE)
    {
      MSPUB_DEBUG_MSG(("shape chunk: offset 0x%lx, seqnum 0x%x, parent seqnum: 0x%x\n", offset, m_lastSeenSeqNum, parentSeqNum));
      m_shapeChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      return &m_shapeChunks.back();
    }
    else if (type == PALETTE)
    {
      m_paletteChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
      return &m_paletteChunks.back();
    }
    m_unknownChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
  }
  return NULL;
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
  case 0x107:
    return MASTER;
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
