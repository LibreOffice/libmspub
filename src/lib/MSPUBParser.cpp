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
#include <string.h>
#include <libwpd-stream/WPXStream.h>
#include "MSPUBParser.h"
#include "MSPUBCollector.h"
#include "MSPUBBlockID.h"
#include "MSPUBBlockType.h"
#include "MSPUBChunkType.h"
#include "libmspub_utils.h"

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector), m_blockInfo(), m_lastSeenSeqNum(-1)
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
    return 2;
  case 0x20:
  case 0x22:
  case 0x28:
  case 0x58:
  case 0x68:
  case 0x70:
  case 0xb8:
    return 4;
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
  return 0;
}

bool libmspub::MSPUBParser::parse()
{
  MSPUB_DEBUG_MSG(("***NOTE***: Where applicable, the meanings of block/chunk IDs and Types printed below may be found in:\n\t***MSPUBBlockType.h\n\t***MSPUBBlockID.h\n\t***MSPUBChunkType.h\n*****\n"));
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
  WPXInputStream *escher = m_input->getDocumentOLEStream("Escher/EscherStm");
  if (!escher)
    return false;
  if (!parseEscher(escher))
  {
    delete escher;
    return false;
  }
  delete escher;
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
    return false;
  if (!parseContents(contents))
  {
    delete contents;
    return false;
  }
  delete contents;
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
      while (input->tell() >= 0 && (unsigned)input->tell() < trailerPart.dataOffset + trailerPart.dataLength)
      {
        m_blockInfo.push_back(parseBlock(input));
        ++m_lastSeenSeqNum;
        if (m_blockInfo.back().type == GENERAL_CONTAINER)
        {
          parseChunkReference(input, m_blockInfo.back());
        }
      }
      for (MSPUBCollector::cr_iterator_t i = m_collector->getChunkReferences().begin(); i != m_collector->getChunkReferences().end(); ++i)
      {
        if (i->type == DOCUMENT)
        {
          input->seek(i->offset, WPX_SEEK_SET);
          if (!parseDocumentChunk(input, *i))
          {
            return false;
          }
        }
        else if (i->type == PAGE)
        {
          input->seek(i->offset, WPX_SEEK_SET);
          if (!parsePageChunk(input, *i))
          {
            return false;
          }
        }
      }
      m_collector->pagesOver();
    }
  }
  input->seek(trailerOffset + trailerLength, WPX_SEEK_SET);

  return true;
}

bool libmspub::MSPUBParser::parseDocumentChunk(WPXInputStream *input, const ChunkReference &chunk)
{
  readU32(input); //FIXME: This is the length. Might want to verify that it matches the length we got from subtracting the start of this chunk from the start of the next one.
  while (input->tell() >= 0 && (unsigned)input->tell() < chunk.end)
  {
    libmspub::MSPUBBlockInfo info = parseBlock(input);
    if (info.id == DOCUMENT_SIZE)
    {
      while (input->tell() >= 0 && (unsigned)input->tell() < info.dataOffset + info.dataLength)
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


bool libmspub::MSPUBParser::parsePageChunk(WPXInputStream *input, const ChunkReference &chunk)
{
  MSPUB_DEBUG_MSG(("parsePageChunk: offset 0x%lx, end 0x%lx, seqnum 0x%x, parent 0x%x\n", input->tell(), chunk.end, chunk.seqNum, chunk.parentSeqNum));
  if (getPageTypeBySeqNum(chunk.seqNum) == NORMAL)
  {
    m_collector->addPage();
  }
  return true;
}

bool libmspub::MSPUBParser::parseQuill(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseQuill\n"));
  return true;
}

bool libmspub::MSPUBParser::parseEscher(WPXInputStream *input)
{
  MSPUB_DEBUG_MSG(("MSPUBParser::parseEscher\n"));
  return true;
}

bool libmspub::MSPUBParser::parseChunkReference(WPXInputStream *input, const libmspub::MSPUBBlockInfo block)
{
  //input should be at block.dataOffset + 4 , that is, at the beginning of the list of sub-blocks
  libmspub::MSPUBChunkType type = (libmspub::MSPUBChunkType)0;
  unsigned long offset = 0;
  unsigned parentSeqNum = 0;
  bool seenType = false;
  bool seenOffset = false;
  bool seenParentSeqNum = false;
  while (input->tell() >= 0 && (unsigned)input->tell() < block.dataOffset + block.dataLength)
  {
    libmspub::MSPUBBlockInfo subBlock = parseBlock(input);
    //FIXME: Warn if multiple of these blocks seen.
    if (subBlock.id == CHUNK_TYPE)
    {
      type = (libmspub::MSPUBChunkType)subBlock.data;
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
  if (seenType && seenOffset)
  {
    m_collector->addChunkReference(type, offset, m_lastSeenSeqNum, seenParentSeqNum ? parentSeqNum : 0);
    return true;
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
      info.stringData = std::vector<char>(info.dataLength - 1);
      for (unsigned i = 0; i < info.dataLength - 1; ++i)
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
    default:
      info.data = 0;
    }
  }
  MSPUB_DEBUG_MSG(("parseBlock dataOffset 0x%lx, id 0x%x, type 0x%x, dataLength %lu, integral data 0x%lx\n", info.dataOffset, info.id, info.type, info.dataLength, info.data));
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
