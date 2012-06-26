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
#include "MSPUBDocument.h"
#include "MSPUBSVGGenerator.h"
#include "MSPUBCollector.h"
#include "MSPUBParser.h"
#include "libmspub_utils.h"

bool isOldVersion;

/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a Microsoft Publisher Document that libmspub is able to parse
*/
bool libmspub::MSPUBDocument::isSupported(WPXInputStream *input)
{
  WPXInputStream *tmpStream = 0;
  try
  {
    tmpStream = input->getDocumentOLEStream("Contents");
    if (!tmpStream)
      return false;
    tmpStream->seek(0, WPX_SEEK_SET);
    unsigned char magicVersionByte;
    // Check the magic signature at the beginning of the Contents stream
    if (! (0xe8 == readU8(tmpStream) && 0xac == readU8(tmpStream) && (0x2c == (magicVersionByte = readU8(tmpStream)) || 0x22 == magicVersionByte) && 0x00 == readU8(tmpStream)))
    {
      delete tmpStream;
      return false;
    }
    isOldVersion = magicVersionByte == 0x22;
    if (!input->isOLEStream())
      return false;
    tmpStream = input->getDocumentOLEStream("Quill/QuillSub/CONTENTS");
    if (!tmpStream)
      return false;
    delete tmpStream;
    if (! isOldVersion)
    {
      tmpStream = input->getDocumentOLEStream("Escher/EscherStm");
      if (!tmpStream)
        return false;
      delete tmpStream;
    }
    return true;
  }
  catch (...)
  {
    if (tmpStream)
      delete tmpStream;
    return false;
  }
}

/**
Parses the input stream content. It will make callbacks to the functions provided by a
WPGPaintInterface class implementation when needed. This is often commonly called the
'main parsing routine'.
\param input The input stream
\param painter A MSPUBPainterInterface implementation
\return A value that indicates whether the parsing was successful
*/
bool libmspub::MSPUBDocument::parse(::WPXInputStream *input, libwpg::WPGPaintInterface *painter)
{
  MSPUBCollector collector(painter);
  input->seek(0, WPX_SEEK_SET);
  MSPUBParser *parser = isOldVersion ? new MSPUBParser2k(input, &collector) : new MSPUBParser(input, &collector);
  bool result = parser->parse();
  delete parser;
  return result;
}

/**
Parses the input stream content and generates a valid Scalable Vector Graphics
Provided as a convenience function for applications that support SVG internally.
\param input The input stream
\param output The output string whose content is the resulting SVG
\return A value that indicates whether the SVG generation was successful.
*/
bool libmspub::MSPUBDocument::generateSVG(::WPXInputStream *input, libmspub::MSPUBStringVector &output)
{
  libmspub::MSPUBSVGGenerator generator(output);
  bool result = libmspub::MSPUBDocument::parse(input, &generator);
  return result;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
