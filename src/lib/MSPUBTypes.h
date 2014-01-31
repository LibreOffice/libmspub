/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __MSPUBTYPES_H__
#define __MSPUBTYPES_H__

#include <vector>
#include <string>
#include <boost/optional.hpp>
#include "MSPUBBlockType.h"
#include "MSPUBBlockID.h"
#include "MSPUBContentChunkType.h"
#include "MSPUBConstants.h"
#include "ListInfo.h"

namespace libmspub
{
enum BorderPosition
{
  INSIDE_SHAPE,
  HALF_INSIDE_SHAPE,
  OUTSIDE_SHAPE
};

enum SuperSubType
{
  NO_SUPER_SUB,
  SUPERSCRIPT,
  SUBSCRIPT
};

enum Alignment
{
  LEFT = 0,
  CENTER = 2,
  RIGHT = 1,
  JUSTIFY = 6
};

struct EscherContainerInfo
{
  unsigned short initial;
  unsigned short type;
  unsigned long contentsLength;
  unsigned long contentsOffset;
};

struct MSPUBBlockInfo
{
  MSPUBBlockInfo() : id((MSPUBBlockID)0), type((MSPUBBlockType)0), startPosition(0), dataOffset(0), dataLength(0), data(0), stringData() { }
  MSPUBBlockID id;
  MSPUBBlockType type;
  unsigned long startPosition;
  unsigned long dataOffset;
  unsigned long dataLength;
  unsigned data;
  std::vector<unsigned char> stringData;
};

struct ContentChunkReference
{
  ContentChunkReference() : type((MSPUBContentChunkType)0), offset(0), end(0), seqNum(0), parentSeqNum(0) { }
  ContentChunkReference(MSPUBContentChunkType t, unsigned long o, unsigned long e, unsigned sn, unsigned psn) :
    type(t), offset(o), end(e), seqNum(sn), parentSeqNum(psn) {}
  MSPUBContentChunkType type;
  unsigned long offset;
  unsigned long end; //offset of the last element plus one.
  unsigned seqNum;
  unsigned parentSeqNum;
};

struct QuillChunkReference
{
  QuillChunkReference() : length(0), offset(0), id(0), name(), name2() { }
  unsigned long length;
  unsigned long offset;
  unsigned short id;
  std::string name;
  std::string name2;
};

struct CharacterStyle
{
  CharacterStyle() :
    underline(), italic(), bold(),
    textSizeInPt(), colorIndex(-1), fontIndex(), superSubType(NO_SUPER_SUB)
  {
  }
  CharacterStyle(bool u, bool i, bool b,
                 boost::optional<double> tSIP = boost::optional<double>(),
                 int cI = -1,
                 boost::optional<unsigned> fI = boost::optional<unsigned>(),
                 SuperSubType sst = NO_SUPER_SUB) :
    underline(u), italic(i), bold(b), textSizeInPt(tSIP), colorIndex(cI), fontIndex(fI), superSubType(sst) { }
  bool underline;
  bool italic;
  bool bold;
  boost::optional<double> textSizeInPt;
  int colorIndex;
  boost::optional<unsigned> fontIndex;
  SuperSubType superSubType;
};

enum LineSpacingType
{
  LINE_SPACING_SP,
  LINE_SPACING_PT
};

struct LineSpacingInfo
{
  LineSpacingType m_type;
  double m_amount;
  LineSpacingInfo() : m_type(LINE_SPACING_SP), m_amount(1)
  {
  }
  LineSpacingInfo(LineSpacingType type, double amount) :
    m_type(type), m_amount(amount)
  {
  }
};

struct ParagraphStyle
{
  boost::optional<Alignment> m_align;
  boost::optional<unsigned> m_defaultCharStyleIndex;
  boost::optional<LineSpacingInfo> m_lineSpacing;
  boost::optional<unsigned> m_spaceBeforeEmu;
  boost::optional<unsigned> m_spaceAfterEmu;
  boost::optional<int> m_firstLineIndentEmu;
  boost::optional<unsigned> m_leftIndentEmu;
  boost::optional<unsigned> m_rightIndentEmu;
  boost::optional<ListInfo> m_listInfo;
  std::vector<unsigned> m_tabStopsInEmu;
  boost::optional<unsigned> m_dropCapLines;
  boost::optional<unsigned> m_dropCapLetters;
  ParagraphStyle() :
    m_align(), m_defaultCharStyleIndex(), m_lineSpacing(), m_spaceBeforeEmu(),
    m_spaceAfterEmu(), m_firstLineIndentEmu(), m_leftIndentEmu(),
    m_rightIndentEmu(), m_listInfo(), m_tabStopsInEmu(), m_dropCapLines(), m_dropCapLetters()
  {
  }
};

struct TextSpan
{
  TextSpan(const std::vector<unsigned char> &c, const CharacterStyle &s) : chars(c), style(s) { }
  std::vector<unsigned char> chars;
  CharacterStyle style;
};

struct TextParagraph
{
  TextParagraph(const std::vector<TextSpan> &sp, const ParagraphStyle &st) : spans(sp), style(st) { }
  std::vector<TextSpan> spans;
  ParagraphStyle style;
};

struct Color
{
  Color() : r(0), g(0), b(0) { }
  Color(unsigned char red, unsigned char green, unsigned char blue) : r(red), g(green), b(blue) { }
  unsigned char r, g, b;
};

enum PageType
{
  MASTER,
  NORMAL,
  DUMMY_PAGE
};

enum ImgType
{
  UNKNOWN,
  PNG,
  JPEG,
  WMF,
  EMF,
  TIFF,
  DIB,
  PICT,
  JPEGCMYK
};

} // namespace libmspub

#endif /* __MSPUBTYPES_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
