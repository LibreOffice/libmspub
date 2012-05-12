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
#include "libmspub_utils.h"

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector), m_blockInfo()
{
}

libmspub::MSPUBParser::~MSPUBParser()
{
}

bool libmspub::MSPUBParser::parse()
{
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
    unsigned startPosition = input->tell();
#ifdef DEBUG
    unsigned char id = 
#endif
	readU8(input);
    unsigned char type = readU8(input);
    unsigned blockLength = readU32(input);
    MSPUB_DEBUG_MSG(("Trailer SubBlock %i, startPosition 0x%x, id %i, type 0x%x, length 0x%x\n", i+1, startPosition, id, type, blockLength));
    if (type == 0x90)
    {
      while (input->tell() >= 0 && (unsigned)input->tell() < startPosition + blockLength)
        m_blockInfo.push_back(parseBlock(input));
    }
  }
  input->seek(trailerOffset + trailerLength, WPX_SEEK_SET);

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

libmspub::MSPUBBlockInfo libmspub::MSPUBParser::parseBlock(WPXInputStream *input)
{
  libmspub::MSPUBBlockInfo info;
  info.id = readU8(input);
  info.type = readU8(input);
  unsigned startPosition = input->tell();
  if (info.id != 0 || info.type != 0x78)
    info.length=readU32(input);
  else
    info.length=0;
  MSPUB_DEBUG_MSG(("parseBlock startPosition 0x%x, id 0x%x, type 0x%x, length 0x%x\n", startPosition, info.id, info.type, info.length));
  if (info.length)
  {
    input->seek(startPosition+info.length, WPX_SEEK_SET);
  }
  return info;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
