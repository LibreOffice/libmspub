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

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), chunkReferences(), m_width(0), m_height(0), m_widthSet(false), m_heightSet(false), m_commonProperties(), m_numPages(0)
{
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

libmspub::ChunkReference::ChunkReference(unsigned type, unsigned long offset, unsigned long end, unsigned seqNum, unsigned parentSeqNum) :
  type(type), offset(offset), end(end), seqNum(seqNum), parentSeqNum(parentSeqNum)
{
}

bool libmspub::MSPUBCollector::addChunkReference(unsigned type, unsigned long offset, unsigned seqNum, unsigned parentSeqNum)
{
  static bool first = true;
  static unsigned lastType;
  static unsigned lastOffset;
  static unsigned lastSeqNum;
  static unsigned lastParentSeqNum;
  if (!first)
  {
    chunkReferences.push_back(libmspub::ChunkReference(lastType, lastOffset, offset, lastSeqNum, lastParentSeqNum));
  }
  first = false;
  lastType = type;
  lastOffset = offset;
  lastSeqNum = seqNum;
  lastParentSeqNum = parentSeqNum;
  return true;
}

bool libmspub::MSPUBCollector::chunkReferencesOver(unsigned long end)
{
  //FIXME: Assert that this hasn't been called already.
  addChunkReference(0, end, 0, 0); //the three zeroes are ignored; this just causes the last chunk reference to be added to the list.
  return true;
}

const std::list<libmspub::ChunkReference>& libmspub::MSPUBCollector::getChunkReferences()
{
  //FIXME: Assert that chunkReferencesOver has been called.
  return chunkReferences;
}

bool libmspub::MSPUBCollector::addPage()
{
  if (! (m_widthSet && m_heightSet) )
  {
    return false;
  }
  if (m_numPages++ > 0)
  {
    m_painter->endGraphics();
  }
  m_painter->startGraphics(m_commonProperties);
  return true;
}

bool libmspub::MSPUBCollector::pagesOver()
{
  if (m_numPages > 0)
  {
    m_painter->endGraphics();
  }
  return true;
}

void libmspub::MSPUBCollector::setWidthInEmu(unsigned long widthInEmu)
{
  //FIXME: Warn if this is called twice
  m_width = widthInEmu;
  m_commonProperties.insert("svg:width", ((double)widthInEmu)/914400.0);
  m_widthSet = true;
}

void libmspub::MSPUBCollector::setHeightInEmu(unsigned long heightInEmu)
{
  //FIXME: Warn if this is called twice
  m_height = heightInEmu;
  m_commonProperties.insert("svg:height", ((double)heightInEmu)/914400.0);
  m_heightSet = true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
