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
 * in which case the procdrns of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include <sstream>
#include <string>
#include <string.h>
#include <libwpd-stream/WPXStream.h>
#include "MSPUBParser.h"
#include "MSPUBCollector.h"

libmspub::MSPUBParser::MSPUBParser(WPXInputStream *input, MSPUBCollector *collector)
  : m_input(input), m_collector(collector)
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
  if (!parseQuill(quill, m_collector))
  {
  	delete quill;
	return false;
  }
  delete quill;
  WPXInputStream *escher = m_input->getDocumentOLEStream("Escher/EscherStm");
  if (!escher)
  	return false;
  if (!parseEscher(escher, m_collector))
  {
  	delete escher;
	return false;
  }
  delete escher;
  WPXInputStream *contents = m_input->getDocumentOLEStream("CONTENTS");
  if (!contents)
    return false;
  if (!parseContents(contents, m_collector))
  {
    delete contents;
	return false;
  }
  delete contents;
  return true;
}

bool libmspub::MSPUBParser::parseContents(WPXInputStream *input, libmspub::MSPUBCollector *collector)
{
  return true;
}

bool libmspub::MSPUBParser::parseQuill(WPXInputStream *input, libmspub::MSPUBCollector *collector)
{
  return true;
}

bool libmspub::MSPUBParser::parseEscher(WPXInputStream *input, libmspub::MSPUBCollector *collector)
{
  return true;
}


/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
