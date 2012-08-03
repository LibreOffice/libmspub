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

#include "MSPUBParser97.h"
#include "MSPUBCollector.h"
#include "libmspub_utils.h"
#include "MSPUBTypes.h"

libmspub::MSPUBParser97::MSPUBParser97(WPXInputStream *input, MSPUBCollector *collector)
  : MSPUBParser2k(input, collector), m_isBanner(false)
{
  m_collector->setEncoding(WIN_1252);
}

unsigned short libmspub::MSPUBParser97::getTextMarker() const
{
  return 0x0000;
}

unsigned libmspub::MSPUBParser97::getTextIdOffset() const
{
  return 0x46;
}

bool libmspub::MSPUBParser97::parse()
{
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
  {
    MSPUB_DEBUG_MSG(("Couldn't get contents stream.\n"));
    return false;
  }
  if (!parseContents(contents))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse contents stream.\n"));
    delete contents;
    return false;
  }
  return m_collector->go();
}

bool libmspub::MSPUBParser97::parseDocument(WPXInputStream *input)
{
  if (m_documentChunkIndex.is_initialized())
  {
    input->seek(m_contentChunks[m_documentChunkIndex.get()].offset + 0x12, WPX_SEEK_SET);
    unsigned short coordinateSystemMark = readU16(input);
    m_isBanner = coordinateSystemMark == 0x0007;
    unsigned width = readU32(input);
    unsigned height = readU32(input);
    m_collector->setWidthInEmu(width);
    m_collector->setHeightInEmu(height);
    return true;
  }
  return false;
}

void libmspub::MSPUBParser97::parseContentsTextIfNecessary(WPXInputStream *input)
{
  input->seek(0x12, WPX_SEEK_SET);
  input->seek(readU32(input), WPX_SEEK_SET);
  input->seek(14, WPX_SEEK_CUR);
  unsigned textStart = readU32(input);
  unsigned textEnd = readU32(input);
  unsigned prop1Index = readU16(input);
  unsigned prop2Index = readU16(input);
  unsigned prop3Index = readU16(input);
  unsigned prop3End = readU16(input);
  std::vector<SpanInfo97> spanInfos = getSpansInfo(input, prop1Index,
                                      prop2Index, prop3Index, prop3End);
  input->seek(textStart, WPX_SEEK_SET);
  TextInfo97 textInfo = getTextInfo(input, textEnd - textStart);
  unsigned iParaEnd = 0, iSpanEnd = 0;
  unsigned currentParaIndex = 0;
  unsigned currentSpanIndex = 0;
  for (unsigned iShapeEnd = 0; iShapeEnd < textInfo.m_shapeEnds.size(); ++iShapeEnd)
  {
    unsigned shapeEnd = std::min<unsigned>(textInfo.m_shapeEnds[iShapeEnd], textInfo.m_chars.size());
    std::vector<TextParagraph> shapeParas;
    while (currentParaIndex < shapeEnd)
    {
      unsigned paraEnd = iParaEnd < textInfo.m_paragraphEnds.size() ?
                         textInfo.m_paragraphEnds[iParaEnd++] : shapeEnd;
      if (paraEnd > shapeEnd)
      {
        --iParaEnd;
        paraEnd = shapeEnd;
      }
      std::vector<TextSpan> paraSpans;
      while (currentSpanIndex < paraEnd)
      {
        const SpanInfo97 &spanInfo = iSpanEnd < spanInfos.size() ?
                                     spanInfos[iSpanEnd++] :
                                     SpanInfo97(paraEnd, CharacterStyle());
        unsigned spanEnd = spanInfo.m_spanEnd;
        if (spanEnd > paraEnd)
        {
          --iSpanEnd;
          spanEnd = paraEnd;
        }
        const CharacterStyle &spanStyle = spanInfo.m_style;
        std::vector<unsigned char> spanChars;
        spanChars.reserve(spanEnd - currentSpanIndex);
        for (unsigned i = currentSpanIndex; i < spanEnd; ++i)
        {
          unsigned char ch = textInfo.m_chars[i];
          if (ch == 0xB) // Pub97 interprets vertical tab as nonbreaking space.
          {
            spanChars.push_back('\n');
          }
          else if (ch == 0x0D)
          {
            if (i + 1 < spanEnd && textInfo.m_chars[i + 1] == 0x0A)
            {
              ++i; // ignore the 0x0D and advance past the 0x0A
            }
          }
          else if (ch == 0x0C)
          {
            // ignore the 0x0C
          }
          else
          {
            spanChars.push_back(ch);
          }
        }
        paraSpans.push_back(TextSpan(spanChars, spanStyle));
        currentSpanIndex = spanEnd;
      }
      shapeParas.push_back(TextParagraph(paraSpans, ParagraphStyle()));
      currentParaIndex = paraEnd;
    }
    m_collector->addTextString(shapeParas, iShapeEnd);
  }
}

std::vector<libmspub::MSPUBParser97::SpanInfo97> libmspub::MSPUBParser97::getSpansInfo(
  WPXInputStream *input,
  unsigned prop1Index, unsigned prop2Index, unsigned /* prop3Index */,
  unsigned /* prop3End */)
{
  std::vector<unsigned> spanEnds;
  std::vector<SpanInfo97> ret;
  for (unsigned i = prop1Index; i < prop2Index; ++i)
  {
    unsigned offset = i * 0x200;
    input->seek(offset + 0x1FF, WPX_SEEK_SET);
    unsigned numEntries = readU8(input);
    input->seek(offset, WPX_SEEK_SET);
    // Skip the first thing; it is not an end
    unsigned start = readU32(input);
    for (unsigned j = 0; j < numEntries; ++j)
    {
      spanEnds.push_back(readU32(input) - start);
    }
    std::vector<unsigned char> spanStyleIndices;
    for (unsigned j = 0; j < spanEnds.size(); ++j)
    {
      spanStyleIndices.push_back(readU8(input));
    }
    while (stillReading(input, offset + 0x200) && readU8(input) == 0)
    {
      ;
    }
    input->seek(-1, WPX_SEEK_CUR);
    std::map<unsigned char, CharacterStyle> stylesByIndex;
    while (stillReading(input, offset + 0x1FF))
    {
      unsigned length = readU8(input);
      unsigned nextOffset = input->tell() + length;
      unsigned char index = static_cast<unsigned char>(
                              (input->tell() - 1 - offset) / 2);
      stylesByIndex[index] = readCharacterStyle(input, length);
      input->seek(nextOffset, WPX_SEEK_SET);
    }
    for (unsigned j = 0; j < spanEnds.size(); ++j)
    {
      ret.push_back(SpanInfo97(spanEnds[j], j < spanStyleIndices.size() ?
                               stylesByIndex[spanStyleIndices[j]] : CharacterStyle()));
    }
  }
  return ret;
}

libmspub::CharacterStyle libmspub::MSPUBParser97::readCharacterStyle(
  WPXInputStream *input, unsigned length)
{
  unsigned begin = input->tell();
  bool underline = false, italic = false, bold = false;
  int colorIndex = -1;
  unsigned fontIndex = 0;
  int textSizeVariationFromDefault = 0;

  if (length >= 1)
  {
    unsigned char biFlags = readU8(input);
    bold = biFlags & 0x1;
    italic = biFlags & 0x2;
  }
  if (length >= 3)
  {
    input->seek(begin + 0x2, WPX_SEEK_SET);
    fontIndex = readU8(input);
  }
  if (length >= 9)
  {
    input->seek(begin + 0x8, WPX_SEEK_SET);
    underline = readU8(input) & 0x1;
  }
  if (length >= 5)
  {
    input->seek(begin + 0x4, WPX_SEEK_SET);
    textSizeVariationFromDefault =
      length >= 6 ? readS16(input) : readS8(input);
  }
  if (length >= 16)
  {
    input->seek(begin + 0xC, WPX_SEEK_SET);
    colorIndex = getColorIndexByQuillEntry(readU32(input));
  }
  double textSizeInPt = 10 +
                        static_cast<double>(textSizeVariationFromDefault) / 2;
  return CharacterStyle(underline, italic, bold, textSizeInPt, colorIndex,
                        fontIndex);
}

libmspub::MSPUBParser97::TextInfo97 libmspub::MSPUBParser97::getTextInfo(WPXInputStream *input, unsigned length)
{
  std::vector<unsigned char> chars;
  chars.reserve(length);
  std::vector<unsigned> paragraphEnds;
  std::vector<unsigned> shapeEnds;
  unsigned start = input->tell();
  unsigned char last = '\0';
  while (stillReading(input, start + length))
  {
    chars.push_back(readU8(input));
    if (last == 0xD && chars.back() == 0xA)
    {
      paragraphEnds.push_back(chars.size());
    }
    else if (chars.back() == 0xC)
    {
      shapeEnds.push_back(chars.size());
    }
    last = chars.back();
  }
  return TextInfo97(chars, paragraphEnds, shapeEnds);
}

int libmspub::MSPUBParser97::translateCoordinateIfNecessary(int coordinate) const
{
  if (m_isBanner)
  {
    return coordinate - 120 * EMUS_IN_INCH;
  }
  else
  {
    return coordinate - 25 * EMUS_IN_INCH;
  }
}

unsigned libmspub::MSPUBParser97::getFirstLineOffset() const
{
  return 0x22;
}

unsigned libmspub::MSPUBParser97::getSecondLineOffset() const
{
  return 0x2D;
}

unsigned libmspub::MSPUBParser97::getShapeFillTypeOffset() const
{
  return 0x20;
}

unsigned libmspub::MSPUBParser97::getShapeFillColorOffset() const
{
  return 0x18;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
