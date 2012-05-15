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

#include "MSPUBCollector.h"
#include "libmspub_utils.h"
#include "MSPUBConstants.h"

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), contentContentChunkReferences(), m_width(0), m_height(0), m_widthSet(false), m_heightSet(false), m_commonProperties(), m_numPages(0), idsToTextStrings(), pagesBySeqNum()
{
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

libmspub::ContentChunkReference::ContentChunkReference(libmspub::MSPUBContentChunkType type, unsigned long offset, unsigned long end, unsigned seqNum, unsigned parentSeqNum) :
  type(type), offset(offset), end(end), seqNum(seqNum), parentSeqNum(parentSeqNum)
{
}

const std::list<libmspub::ContentChunkReference>& libmspub::MSPUBCollector::getContentChunkReferences()
{
  //FIXME: Assert that contentContentChunkReferencesOver has been called.
  return contentContentChunkReferences;
}

bool libmspub::MSPUBCollector::addPage(unsigned seqNum)
{
  if (! (m_widthSet && m_heightSet) )
  {
    return false;
  }
  pagesBySeqNum[seqNum] = PageInfo();
  return true;
}

bool libmspub::MSPUBCollector::addTextShape(unsigned id, unsigned pageSeqNum, unsigned long width, unsigned long height)
{
  std::map<unsigned, PageInfo>::iterator i_page = pagesBySeqNum.find(pageSeqNum);
  if (i_page == pagesBySeqNum.end())
  {
    MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in addTextShape!\n", pageSeqNum));
    return false;
  }
  else
  {
    std::map<unsigned, std::vector<unsigned char> >::iterator i_str = idsToTextStrings.find(id);
    if (i_str == idsToTextStrings.end())
    {
      MSPUB_DEBUG_MSG(("Text string of id 0x%x not found in addTextShape!\n", id));
      return false;
    }
    else
    {
      MSPUB_DEBUG_MSG(("addTextShape succeeded with id 0x%x, pageSeqNum 0x%x\n, width 0x%lx, height 0x%lx", id, pageSeqNum, width, height));
      i_page->second.textShapes.push_back(TextShapeInfo(i_str->second, width, height));
      return true;
    }
  }
}

bool libmspub::MSPUBCollector::go()
{
  for (std::map<unsigned, PageInfo>::const_iterator i = pagesBySeqNum.begin(); i != pagesBySeqNum.end(); ++i)
  {
    m_painter->startGraphics(m_commonProperties);
    for (std::vector<TextShapeInfo>::const_iterator j = i->second.textShapes.begin(); j != i->second.textShapes.end(); ++j)
    {
      WPXString text;
      appendCharacters(text, j->str);
      WPXPropertyList props;
      props.insert("svg:width", ((double)j->width) / EMUS_IN_INCH);
      props.insert("svg:height", ((double)j->height) / EMUS_IN_INCH);
      WPXPropertyListVector vec; //FIXME: What is this for? Perhaps when we have less rudimentary text-handling it will come into play...
      m_painter->startTextObject(props, vec);
      m_painter->insertText(text);
      m_painter->endTextObject();
    }
    m_painter->endGraphics();
  }
  return true;
}


bool libmspub::MSPUBCollector::addTextString(const std::vector<unsigned char> &str, unsigned id)
{
  MSPUB_DEBUG_MSG(("addTextString, id: 0x%x\n", id));
  idsToTextStrings[id] = str;
  return true; //FIXME: Warn if the string already existed in the map.
}

void libmspub::MSPUBCollector::setWidthInEmu(unsigned long widthInEmu)
{
  //FIXME: Warn if this is called twice
  m_width = widthInEmu;
  m_commonProperties.insert("svg:width", ((double)widthInEmu)/EMUS_IN_INCH);
  m_widthSet = true;
}

void libmspub::MSPUBCollector::setHeightInEmu(unsigned long heightInEmu)
{
  //FIXME: Warn if this is called twice
  m_height = heightInEmu;
  m_commonProperties.insert("svg:height", ((double)heightInEmu)/EMUS_IN_INCH);
  m_heightSet = true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
