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
    underline(false), italic(false), bold(false),
    textSizeInPt(), colorIndex(-1), fontIndex(0), superSubType(NO_SUPER_SUB)
  {
  }
  CharacterStyle(bool u, bool i, bool b,
                 boost::optional<double> tSIP = boost::optional<double>(),
                 int cI = -1, unsigned fI = 0, SuperSubType sst = NO_SUPER_SUB) :
    underline(u), italic(i), bold(b), textSizeInPt(tSIP), colorIndex(cI), fontIndex(fI), superSubType(sst) { }
  bool underline;
  bool italic;
  bool bold;
  boost::optional<double> textSizeInPt;
  int colorIndex;
  unsigned fontIndex;
  SuperSubType superSubType;
};

struct ParagraphStyle
{
  ParagraphStyle(Alignment a = (Alignment)-1, unsigned dCSI = 0, unsigned ls = LINE_SPACING_UNIT, unsigned sb = 0, unsigned sa = 0, int fli = 0, unsigned li = 0, unsigned ri = 0, boost::optional<ListInfo> lInfo = boost::optional<ListInfo>()) : align(a), defaultCharStyleIndex(dCSI), lineSpacing(ls), spaceBeforeEmu(sb), spaceAfterEmu(sa), firstLineIndentEmu(fli), leftIndentEmu(li), rightIndentEmu(ri), listInfo(lInfo) { }
  Alignment align;
  unsigned defaultCharStyleIndex;
  unsigned lineSpacing;
  unsigned spaceBeforeEmu;
  unsigned spaceAfterEmu;
  int firstLineIndentEmu;
  unsigned leftIndentEmu;
  unsigned rightIndentEmu;
  boost::optional<ListInfo> listInfo;
};

struct TextSpan
{
  TextSpan(std::vector<unsigned char> c, CharacterStyle s) : chars(c), style(s) { }
  std::vector<unsigned char> chars;
  CharacterStyle style;
};

struct TextParagraph
{
  TextParagraph(std::vector<TextSpan> sp, ParagraphStyle st) : spans(sp), style(st) { }
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
  PICT
};

} // namespace libmspub

#endif /* __MSPUBTYPES_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
