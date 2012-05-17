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
  m_painter(painter), contentChunkReferences(), m_width(0), m_height(0), m_widthSet(false), m_heightSet(false), m_commonPageProperties(), m_numPages(0), textStringsById(), pagesBySeqNum(), textShapesBySeqNum()
{
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

libmspub::ContentChunkReference::ContentChunkReference(libmspub::MSPUBContentChunkType type, unsigned long offset, unsigned long end, unsigned seqNum, unsigned parentSeqNum) :
  type(type), offset(offset), end(end), seqNum(seqNum), parentSeqNum(parentSeqNum)
{
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

bool libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum)
{
  std::map<unsigned, PageInfo>::iterator i_page = pagesBySeqNum.find(pageSeqNum);
  if (i_page == pagesBySeqNum.end())
  {
    MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in addTextShape!\n", pageSeqNum));
    return false;
  }
  else
  {
    std::map<unsigned, std::vector<unsigned char> >::iterator i_str = textStringsById.find(stringId);
    if (i_str == textStringsById.end())
    {
      MSPUB_DEBUG_MSG(("Text string of id 0x%x not found in addTextShape!\n", stringId));
      return false;
    }
    else
    {
      std::pair<std::map<unsigned, TextShapeInfo>::iterator, bool> result = textShapesBySeqNum.insert(std::pair<const unsigned, TextShapeInfo>(seqNum, TextShapeInfo(i_str->second)));
      if (result.second)
      {
        i_page->second.textShapeReferences.push_back(result.first);
        MSPUB_DEBUG_MSG(("addTextShape succeeded with id 0x%x", stringId));
        return true;
      }
      MSPUB_DEBUG_MSG(("already tried to add the text shape of seqnum 0x%x to this page!\n", seqNum));
      return false;
    }
  }
}

bool libmspub::MSPUBCollector::setShapeCoordinatesInEmu(unsigned short seqNum, long xs, long ys, long xe, long ye)
{
  if (! (m_widthSet && m_heightSet))
  {
    return false;
  }
  std::map<unsigned, TextShapeInfo>::iterator elt = textShapesBySeqNum.find(seqNum);
  double x_center = m_commonPageProperties["svg:width"]->getDouble() / 2;
  double y_center = m_commonPageProperties["svg:height"]->getDouble() / 2;
  if (elt != textShapesBySeqNum.end())
  {
    elt->second.props.insert("svg:x", x_center + (double)xs / EMUS_IN_INCH);
    elt->second.props.insert("svg:y", y_center + (double)ys / EMUS_IN_INCH);
    elt->second.props.insert("svg:width", (double)(xe - xs) / EMUS_IN_INCH);
    elt->second.props.insert("svg:height", (double)(ye - ys) / EMUS_IN_INCH);
  }
  return true;
}

bool libmspub::MSPUBCollector::go()
{
  for (std::map<unsigned, PageInfo>::const_iterator i = pagesBySeqNum.begin(); i != pagesBySeqNum.end(); ++i)
  {
    m_painter->startGraphics(m_commonPageProperties);
    for (std::vector<std::map<unsigned, TextShapeInfo>::const_iterator>::const_iterator j = i->second.textShapeReferences.begin(); j != i->second.textShapeReferences.end(); ++j)
    {
      WPXString text;
      appendCharacters(text, (*j)->second.str);
      m_painter->startTextObject((*j)->second.props, WPXPropertyListVector());
      m_painter->startTextLine(WPXPropertyList());
      m_painter->insertText(text);
      m_painter->endTextLine();
      m_painter->endTextObject();
    }
    m_painter->endGraphics();
  }
  return true;
}


bool libmspub::MSPUBCollector::addTextString(const std::vector<unsigned char> &str, unsigned id)
{
  MSPUB_DEBUG_MSG(("addTextString, id: 0x%x\n", id));
  textStringsById[id] = str;
  return true; //FIXME: Warn if the string already existed in the map.
}

void libmspub::MSPUBCollector::setWidthInEmu(unsigned long widthInEmu)
{
  //FIXME: Warn if this is called twice
  m_width = widthInEmu;
  m_commonPageProperties.insert("svg:width", ((double)widthInEmu)/EMUS_IN_INCH);
  m_widthSet = true;
}

void libmspub::MSPUBCollector::setHeightInEmu(unsigned long heightInEmu)
{
  //FIXME: Warn if this is called twice
  m_height = heightInEmu;
  m_commonPageProperties.insert("svg:height", ((double)heightInEmu)/EMUS_IN_INCH);
  m_heightSet = true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
