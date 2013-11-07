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

#ifndef __MSPUBPARSER97_H__
#define __MSPUBPARSER97_H__

#include "MSPUBParser2k.h"

namespace libmspub
{
class MSPUBParser97 : public MSPUBParser2k
{
  struct TextInfo97
  {
    std::vector<unsigned char> m_chars;
    std::vector<unsigned> m_paragraphEnds;
    std::vector<unsigned> m_shapeEnds;
    TextInfo97(const std::vector<unsigned char> &chars,
               const std::vector<unsigned> &paragraphEnds,
               const std::vector<unsigned> &shapeEnds)
      : m_chars(chars), m_paragraphEnds(paragraphEnds),
        m_shapeEnds(shapeEnds)
    {
    }
  };

  struct SpanInfo97
  {
    unsigned m_spanEnd;
    CharacterStyle m_style;
    SpanInfo97(unsigned spanEnd, const CharacterStyle &style)
      : m_spanEnd(spanEnd), m_style(style)
    {
    }
  };

  bool m_isBanner;

  bool parseDocument(librevenge::RVNGInputStream *input);
  int translateCoordinateIfNecessary(int coordinate) const;
  unsigned getFirstLineOffset() const;
  unsigned getSecondLineOffset() const;
  unsigned getShapeFillTypeOffset() const;
  unsigned getShapeFillColorOffset() const;
  unsigned short getTextMarker() const;
  unsigned getTextIdOffset() const;
  CharacterStyle readCharacterStyle(librevenge::RVNGInputStream *input,
                                    unsigned length);
  void parseContentsTextIfNecessary(librevenge::RVNGInputStream *input);
  std::vector<SpanInfo97> getSpansInfo(librevenge::RVNGInputStream *input,
                                       unsigned prop1Index, unsigned prop2Index, unsigned prop3Index,
                                       unsigned prop3End);
  TextInfo97 getTextInfo(librevenge::RVNGInputStream *input, unsigned length);
public:
  MSPUBParser97(librevenge::RVNGInputStream *input, MSPUBCollector *collector);
  bool parse();
};
}

#endif //  __MSPUBPARSER97_H__

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
