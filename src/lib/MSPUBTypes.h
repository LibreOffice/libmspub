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

#ifndef __MSPUBTYPES_H__
#define __MSPUBTYPES_H__

#include <vector>
#include <string>
#include "MSPUBBlockType.h"
#include "MSPUBBlockID.h"
#include "MSPUBContentChunkType.h"

namespace libmspub
{

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
  ContentChunkReference(MSPUBContentChunkType type, unsigned long offset, unsigned long end, unsigned seqNum, unsigned parentSeqNum);
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
  CharacterStyle(bool underline, bool italic, bool bold, int textSizeInPt = -1, int colorIndex = -1, unsigned fontIndex = 0) : underline(underline), italic(italic), bold(bold), textSizeInPt(textSizeInPt), colorIndex(colorIndex), fontIndex(fontIndex) { }
  bool underline;
  bool italic;
  bool bold;
  int textSizeInPt;
  int colorIndex;
  unsigned fontIndex;
};

struct ParagraphStyle
{
  ParagraphStyle(Alignment align = (Alignment)-1, unsigned defaultCharStyleIndex = 0) : align(align), defaultCharStyleIndex(defaultCharStyleIndex) { }
  Alignment align;
  unsigned defaultCharStyleIndex;
};

struct TextSpan
{
  TextSpan(std::vector<unsigned char> chars, CharacterStyle style) : chars(chars), style(style) { }
  std::vector<unsigned char> chars;
  CharacterStyle style;
};

struct TextParagraph
{
  TextParagraph(std::vector<TextSpan> spans, ParagraphStyle style) : spans(spans), style(style) { }
  std::vector<TextSpan> spans;
  ParagraphStyle style;
};

struct Color
{
  Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) { }
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
  JPEG
};

} // namespace libmspub

#endif /* __MSPUBTYPES_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
