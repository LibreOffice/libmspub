/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */ /* libmspub
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
#include <libwpd/WPXPropertyList.h>
#include <libwpg/WPGPaintInterface.h>

#include "MSPUBTypes.h"

namespace libwpg
{


} // namespace libwpg

namespace libmspub
{



class MSPUBCollector
{
public:
  typedef std::list<ChunkReference>::const_iterator cr_iterator_t;

  MSPUBCollector(::libwpg::WPGPaintInterface *painter);
  virtual ~MSPUBCollector();

  // collector functions
  bool addChunkReference(unsigned type, unsigned long offset, unsigned seqNum, unsigned parentSeqNum);
  bool chunkReferencesOver(unsigned long end);
  bool addPage();
  bool pagesOver();

  void setWidthInEmu(unsigned long);
  void setHeightInEmu(unsigned long);
  const std::list<ChunkReference>& getChunkReferences();

private:
  MSPUBCollector(const MSPUBCollector &);
  MSPUBCollector &operator=(const MSPUBCollector &);

  // helper functions

  libwpg::WPGPaintInterface *m_painter;
  std::list<ChunkReference> chunkReferences;
  unsigned long m_width, m_height;
  bool m_widthSet, m_heightSet;
  WPXPropertyList m_commonProperties;
  unsigned short m_numPages;

  
};

} // namespace libmspub

#endif /* __MSPUBCOLLECTOR_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
