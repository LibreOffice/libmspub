/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_MSPUBPARSER97_H
#define INCLUDED_MSPUBPARSER97_H

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

  bool parseDocument(librevenge::RVNGInputStream *input) override;
  int translateCoordinateIfNecessary(int coordinate) const override;
  unsigned getFirstLineOffset() const override;
  unsigned getSecondLineOffset() const override;
  unsigned getShapeFillTypeOffset() const override;
  unsigned getShapeFillColorOffset() const override;
  unsigned short getTextMarker() const override;
  unsigned getTextIdOffset() const override;
  CharacterStyle readCharacterStyle(librevenge::RVNGInputStream *input,
                                    unsigned length);
  void parseContentsTextIfNecessary(librevenge::RVNGInputStream *input) override;
  std::vector<SpanInfo97> getSpansInfo(librevenge::RVNGInputStream *input,
                                       unsigned prop1Index, unsigned prop2Index, unsigned prop3Index,
                                       unsigned prop3End);
  TextInfo97 getTextInfo(librevenge::RVNGInputStream *input, unsigned length);
public:
  MSPUBParser97(librevenge::RVNGInputStream *input, MSPUBCollector *collector);
  bool parse() override;
};
}

#endif //  INCLUDED_MSPUBPARSER97_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
