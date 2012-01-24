/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* libmspub
 * Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02111-1301 USA
 */

#include <sstream>
#include <string>
#include <string.h>
#include "MSPUBDocument.h"
#include "MSPUBSVGGenerator.h"
#include "libmspub_utils.h"

/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a Microsoft Publisher Document that libmspub is able to parse
*/
bool libmspub::MSPUBDocument::isSupported(WPXInputStream *input)
{
  return false;
}

/**
Parses the input stream content. It will make callbacks to the functions provided by a
MSPUBPaintInterface class implementation when needed. This is often commonly called the
'main parsing routine'.
\param input The input stream
\param painter A MSPUBPainterInterface implementation
\return A value that indicates whether the parsing was successful
*/
bool libmspub::MSPUBDocument::parse(::WPXInputStream *input, libwpg::WPGPaintInterface *painter)
{
  return false;
}

/**
Parses the input stream content and generates a valid Scalable Vector Graphics
Provided as a convenience function for applications that support SVG internally.
\param input The input stream
\param output The output string whose content is the resulting SVG
\return A value that indicates whether the SVG generation was successful.
*/
bool libmspub::MSPUBDocument::generateSVG(::WPXInputStream *input, WPXString &output)
{
  std::ostringstream tmpOutputStream;
  libmspub::MSPUBSVGGenerator generator(tmpOutputStream);
  bool result = libmspub::MSPUBDocument::parse(input, &generator);
  if (result)
    output = WPXString(tmpOutputStream.str().c_str());
  else
    output = WPXString("");
  return result;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
