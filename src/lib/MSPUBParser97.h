/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
