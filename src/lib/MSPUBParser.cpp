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
#include <string.h> //why?
#include <libwpd-stream/WPXStream.h>
#include "MSPUBParser.h"
#include "MSPUBCollector.h"
#include "MSPUBBlockID.h"
#include "MSPUBBlockType.h"
#include "MSPUBContentChunkType.h"
#include "EscherContainerType.h"
#include "EscherContainerOffsets.h"
#include "libmspub_utils.h"

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector), m_blockInfo(), m_pageChunks(), m_shapeChunks(), m_unknownChunks(), m_documentChunk(), m_lastSeenSeqNum(-1), m_seenDocumentChunk(false)
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
  case 0x07: //ask Valek about this
    return 2;
  case 0x20:
  case 0x22:
  case 0x58:
  case 0x68:
  case 0x70:
  case 0xb8:
    return 4;
  case 0x28: //ask Valek about this
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
    return false;
  if (!parseQuill(quill))
  {
    delete quill;
    return false;
  }
  delete quill;
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
    return false;
  if (!parseContents(contents))
  {
    delete contents;
    return false;
  }
  delete contents;
  WPXInputStream *escher = m_input->getDocumentOLEStream("Escher/EscherStm");
  if (!escher)
    return false;
  if (!parseEscher(escher))
  {
    delete escher;
    return false;
  }
  delete escher;

  return m_collector->go();
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

      //while (input->tell() >= 0 && (unsigned)input->tell() < trailerPart.dataOffset + trailerPart.dataLength)
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
      input->seek(m_documentChunk.offset, WPX_SEEK_SET);
      if (!parseDocumentChunk(input, m_documentChunk))
      {
        return false;
      }
      for (ccr_iterator_t i = m_pageChunks.begin(); i != m_pageChunks.end(); ++i)
      {
        input->seek(i->offset, WPX_SEEK_SET);
        if (!parsePageChunk(input, *i))
        {
          return false;
        }
      }
    }
  }
  input->seek(trailerOffset + trailerLength, WPX_SEEK_SET);

  return true;
}

bool libmspub::MSPUBParser::parseDocumentChunk(WPXInputStream *input, const ContentChunkReference &chunk)
{
  MSPUB_DEBUG_MSG(("parseDocumentChunk: offset 0x%lx, end 0x%lx\n", input->tell(), chunk.end));
  unsigned long begin = input->tell();
  unsigned long len = readU32(input); 
  //while (input->tell() >= 0 && (unsigned)input->tell() < begin + len)
  while (stillReading(input, begin + len))
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input);
    if (info.id == DOCUMENT_SIZE)
    {
      //while (input->tell() >= 0 && (unsigned)input->tell() < info.dataOffset + info.dataLength)
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
  //while (input->tell() >= 0 && (unsigned)input->tell() < chunk.offset + length)
  while (stillReading(input, chunk.offset + length))
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input);
    if (info.id == PAGE_SHAPES)
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
  FindBySeqNum(unsigned seqNum) : seqNum(seqNum) { }
  bool operator()(const libmspub::ContentChunkReference &ref)
  {
    return ref.seqNum == seqNum;
  }
};

bool libmspub::MSPUBParser::parseShapes(WPXInputStream *input, libmspub::MSPUBBlockInfo info, unsigned pageSeqNum)
{
  MSPUB_DEBUG_MSG(("parseShapes: page seqnum 0x%x\n", pageSeqNum));
  //while (input->tell() >= 0 && (unsigned)input->tell() < info.dataOffset + info.dataLength)
  while (stillReading(input, info.dataOffset + info.dataLength))
  {
    libmspub::MSPUBBlockInfo subInfo = parseBlock(input, true);
    if (subInfo.type == SHAPE_SEQNUM)
    {
      std::vector<libmspub::ContentChunkReference>::const_iterator ref = std::find_if(m_shapeChunks.begin(), m_shapeChunks.end(), FindBySeqNum(subInfo.data));
      if (ref == m_shapeChunks.end())
      {
        MSPUB_DEBUG_MSG(("Shape of seqnum 0x%lx not found!\n", subInfo.data));
      }
      else
      {
        MSPUB_DEBUG_MSG(("Shape of seqnum 0x%lx found\n", subInfo.data));
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
  // need more info on ESCHER to do anything useful with this.
  unsigned long pos = input->tell();
  unsigned length = readU32(input);
  unsigned width = 0;
  unsigned height = 0;
  bool isText = false;
  unsigned textId = 0;
  //while (input->tell() >= 0 && (unsigned)input->tell() < pos + length)
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
    else
    {
      MSPUB_DEBUG_MSG(("Non-text shape seen: we are not handling this yet.\n"));
    }
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
  readU16(input); //FIXME: Can we do something sensibile if this is not 0x18 ?
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
  input->seek(0x1a, WPX_SEEK_SET);
  unsigned short numChunks = readU16(input);
  input->seek(0x20, WPX_SEEK_SET);
  std::list<QuillChunkReference> chunkReferences;
  for (unsigned i = 0; i < numChunks; ++i)
  {
    libmspub::QuillChunkReference quillChunkReference = parseQuillChunkReference(input);
    chunkReferences.push_back(quillChunkReference);
  }
  //Make sure we parse the STRS chunk before the TEXT chunk
  std::list<QuillChunkReference>::const_iterator textChunkReference = chunkReferences.end();
  bool parsedStrs = false;
  bool parsedSyid = false;
  std::list<unsigned> textLengths;
  std::list<unsigned> textIDs;
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
      for (unsigned i = 0; i < numLengths; ++i)
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
      for (unsigned i = 0; i < numIDs; ++i)
      {
        textIDs.push_back(readU32(input));
      }
      parsedSyid = true;
    }
    if (parsedStrs && parsedSyid && textChunkReference != chunkReferences.end())
    {
      input->seek(textChunkReference->offset, WPX_SEEK_SET);
      for (std::list<unsigned>::const_iterator i = textLengths.begin(), id = textIDs.begin(); i != textLengths.end() && id != textIDs.end(); ++i, ++id)
      {
        std::vector<unsigned char> text(2 * *i);
        for (unsigned j = 0; j < *i; ++j)
        {
          text[2*j] = readU8(input);
          text[2*j+1] = readU8(input);
        }
        m_collector->addTextString(text, *id);
      }
      textChunkReference = chunkReferences.end();
    }
  }
  return true;
}

bool libmspub::MSPUBParser::parseEscher(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseEscher\n"));
  libmspub::EscherContainerInfo fakeroot;
  fakeroot.initial = 0;
  fakeroot.type = 0;
  fakeroot.contentsOffset = input->tell();
  fakeroot.contentsLength = (unsigned)-1; //FIXME: Get the actual length
  libmspub::EscherContainerInfo dg;
  while (findEscherContainer(input, fakeroot, &dg, OFFICE_ART_DG_CONTAINER))
  {
    libmspub::EscherContainerInfo spgr;
    while (findEscherContainer(input, dg, &spgr, OFFICE_ART_SPGR_CONTAINER))
    {
      libmspub::EscherContainerInfo sp;
      while (findEscherContainer(input, spgr, &sp, OFFICE_ART_SP_CONTAINER))
      {
        libmspub::EscherContainerInfo cData;
        libmspub::EscherContainerInfo cAnchor;
        if (findEscherContainer(input, sp, &cData, OFFICE_ART_CLIENT_DATA))
        {
          unsigned shapeSeqNum = 0;
          if (extractEscherValue32(input, cData, (int*)&shapeSeqNum, OFFSET_SHAPE_ID))
          {
            input->seek(sp.contentsOffset, WPX_SEEK_SET);
            if (findEscherContainer(input, sp, &cAnchor, OFFICE_ART_CLIENT_ANCHOR))
            {
              MSPUB_DEBUG_MSG(("Found Escher data for shape of seqnum 0x%x\n", shapeSeqNum));
              int Xs = 0, Ys = 0, Xe = 0, Ye = 0;
              extractEscherValue32(input, cAnchor, &Xs, OFFSET_XS);
              extractEscherValue32(input, cAnchor, &Ys, OFFSET_YS);
              extractEscherValue32(input, cAnchor, &Xe, OFFSET_XE);
              extractEscherValue32(input, cAnchor, &Ye, OFFSET_YE);
              m_collector->setShapeCoordinatesInEmu(shapeSeqNum, Xs, Ys, Xe, Ye);
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

bool libmspub::MSPUBParser::findEscherContainer(WPXInputStream *input, const libmspub::EscherContainerInfo &parent, libmspub::EscherContainerInfo *out, unsigned short desiredType)
{
  MSPUB_DEBUG_MSG(("At offset 0x%lx, attempting to find escher container of type 0x%x\n", input->tell(), desiredType));
  //while (input->tell() >= 0 && (unsigned)input->tell() < parent.contentsOffset + parent.contentsLength)
  while (stillReading(input, parent.contentsOffset + parent.contentsLength))
  {
    libmspub::EscherContainerInfo next = parseEscherContainer(input);
    if (next.type == desiredType)
    {
      *out = next;
      return true;
    }
    input->seek(next.contentsOffset + next.contentsLength + getEscherElementTailLength(next.type), WPX_SEEK_SET);
  }
  return false;
}

bool libmspub::MSPUBParser::extractEscherValue32(WPXInputStream *input, const libmspub::EscherContainerInfo &record, int *out, unsigned short offset)
{
  if (record.contentsLength >= (unsigned)(offset + 4))
  {
    input->seek(record.contentsOffset + offset, WPX_SEEK_SET);
    *out = readS32(input);
    return true;
  }
  return false;
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
  //while (input->tell() >= 0 && (unsigned)input->tell() < block.dataOffset + block.dataLength)
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
    if (type == DOCUMENT)
    {
      MSPUB_DEBUG_MSG(("document chunk: offset 0x%lx, seqnum 0x%x\n", offset, m_lastSeenSeqNum));
      m_documentChunk = ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0);
      m_seenDocumentChunk = true;
      return &m_documentChunk;
    }
    if (type == SHAPE)
    {
      MSPUB_DEBUG_MSG(("shape chunk: offset 0x%lx, seqnum 0x%x, parent seqnum: 0x%x\n", offset, m_lastSeenSeqNum, parentSeqNum));
      m_shapeChunks.push_back(ContentChunkReference(type, offset, 0, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0));
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
      info.stringData = std::vector<unsigned char>(info.dataLength - 4);
      for (unsigned i = 0; i < info.dataLength - 4; ++i)
      {
        info.stringData[i] = readU8(input);
      }
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
  MSPUB_DEBUG_MSG(("parseBlock dataOffset 0x%lx, id 0x%x, type 0x%x, dataLength 0x%lx, integral data 0x%lx\n", info.dataOffset, info.id, info.type, info.dataLength, info.data));
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


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
