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
#include <string.h>
#include <boost/scoped_ptr.hpp>
#include <libmspub/libmspub.h>

#include "MSPUBCollector.h"
#include "MSPUBParser.h"
#include "MSPUBParser2k.h"
#include "MSPUBParser97.h"
#include "libmspub_utils.h"

namespace
{

enum MSPUBVersion
{
  MSPUB_UNKNOWN_VERSION = 0,
  MSPUB_2K,
  MSPUB_2K2
};

MSPUBVersion getVersion(librevenge::RVNGInputStream *input)
{
  try
  {
    if (!input->isStructured())
      return MSPUB_UNKNOWN_VERSION;

    boost::scoped_ptr<librevenge::RVNGInputStream> contentsStream(input->getSubStreamByName("Contents"));
    if (!contentsStream)
      return MSPUB_UNKNOWN_VERSION;

    if (0xe8 != libmspub::readU8(contentsStream.get()) || 0xac != libmspub::readU8(contentsStream.get()))
      return MSPUB_UNKNOWN_VERSION;

    unsigned char magicVersionByte = libmspub::readU8(contentsStream.get());

    if (0x00 != libmspub::readU8(contentsStream.get()))
      return MSPUB_UNKNOWN_VERSION;

    MSPUBVersion version = MSPUB_UNKNOWN_VERSION;
    switch (magicVersionByte)
    {
    case 0x2C:
      version = MSPUB_2K2;
      break;
    case 0x22:
      version =  MSPUB_2K;
      break;
    default:
      break;
    }
    return version;
  }
  catch (...)
  {
    return MSPUB_UNKNOWN_VERSION;
  }

}

} // anonymous namespace



/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a Microsoft Publisher Document that libmspub is able to parse
*/
PUBAPI bool libmspub::MSPUBDocument::isSupported(librevenge::RVNGInputStream *input)
{
  try
  {
    MSPUBVersion version = getVersion(input);
    if (version == MSPUB_UNKNOWN_VERSION)
      return false;

    if (version == MSPUB_2K2)
    {
      boost::scoped_ptr<librevenge::RVNGInputStream> escherStream(input->getSubStreamByName("Escher/EscherStm"));
      if (!escherStream)
        return false;
      boost::scoped_ptr<librevenge::RVNGInputStream> quillStream(input->getSubStreamByName("Quill/QuillSub/CONTENTS"));
      if (!quillStream)
        return false;
    }
    return true;
  }
  catch (...)
  {
    return false;
  }
}

/**
Parses the input stream content. It will make callbacks to the functions provided by a
RVNGDrawingInterface class implementation when needed. This is often commonly called the
'main parsing routine'.
\param input The input stream
\param painter A MSPUBPainterInterface implementation
\return A value that indicates whether the parsing was successful
*/
PUBAPI bool libmspub::MSPUBDocument::parse(librevenge::RVNGInputStream *input, librevenge::RVNGDrawingInterface *painter)
{
  try
  {
    MSPUBCollector collector(painter);
    input->seek(0, librevenge::RVNG_SEEK_SET);
    boost::scoped_ptr<MSPUBParser> parser;
    switch (getVersion(input))
    {
    case MSPUB_2K:
    {
      boost::scoped_ptr<librevenge::RVNGInputStream> quillStream(input->getSubStreamByName("Quill/QuillSub/CONTENTS"));
      if (!quillStream)
        parser.reset(new MSPUBParser97(input, &collector));
      else
        parser.reset(new MSPUBParser2k(input, &collector));
      break;
    }
    case MSPUB_2K2:
    {
      parser.reset(new MSPUBParser(input, &collector));
      break;
    }
    default:
      return false;
    }
    if (parser)
    {
      return parser->parse();
    }
    return false;
  }
  catch (...)
  {
    return false;
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
