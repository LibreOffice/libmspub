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

#ifndef __MSPUBCOLLECTOR_H__
#define __MSPUBCOLLECTOR_H__

#include <list>
#include <vector>
#include <map>
#include <libwpd/WPXPropertyList.h>
#include <libwpg/WPGPaintInterface.h>

#include "MSPUBTypes.h"
#include "MSPUBContentChunkType.h"

namespace libmspub
{

class MSPUBCollector
{
public:
  typedef std::list<ContentChunkReference>::const_iterator ccr_iterator_t;

  MSPUBCollector(::libwpg::WPGPaintInterface *painter);
  virtual ~MSPUBCollector();

  // collector functions
  bool addPage(unsigned seqNum);
  bool addTextString(const std::vector<unsigned char> &str, unsigned id);
  bool addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum);

  bool setShapeCoordinatesInEmu(unsigned short seqNum, long xs, long ys, long xe, long ye);

  void setWidthInEmu(unsigned long);
  void setHeightInEmu(unsigned long);

  bool go();
private:

  struct TextShapeInfo
  {
    TextShapeInfo(std::vector<unsigned char> str) : str(str), props() { }
    std::vector<unsigned char> str;
    WPXPropertyList props;
  };
  struct PageInfo
  {
    PageInfo() : textShapeReferences() { }
    std::vector<std::map<unsigned, TextShapeInfo>::const_iterator> textShapeReferences;
  };

  MSPUBCollector(const MSPUBCollector &);
  MSPUBCollector &operator=(const MSPUBCollector &);

  libwpg::WPGPaintInterface *m_painter;
  std::list<ContentChunkReference> contentChunkReferences;
  unsigned long m_width, m_height;
  bool m_widthSet, m_heightSet;
  WPXPropertyList m_commonPageProperties;
  unsigned short m_numPages;
  std::map<unsigned, std::vector<unsigned char> > textStringsById;
  std::map<unsigned, PageInfo> pagesBySeqNum;
  std::map<unsigned, TextShapeInfo> textShapesBySeqNum;
};

} // namespace libmspub

#endif /* __MSPUBCOLLECTOR_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
