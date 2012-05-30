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

#ifndef __MSPUBPARSER_H__
#define __MSPUBPARSER_H__

#include <map>
#include <vector>
#include <memory>

#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

#include "MSPUBTypes.h"
#include "Fill.h"

class WPXInputStream;

namespace libmspub
{
class MSPUBCollector;

class MSPUBParser
{
public:
  explicit MSPUBParser(WPXInputStream *input, MSPUBCollector *collector);
  virtual ~MSPUBParser();
  bool parse();
private:

  struct TextSpanReference
  {
    TextSpanReference(unsigned short f, unsigned short l, CharacterStyle cs) : first(f), last(l), charStyle(cs) { }
    unsigned short first;
    unsigned short last;
    CharacterStyle charStyle;
  };

  struct TextParagraphReference
  {
    TextParagraphReference(unsigned short f, unsigned short l, ParagraphStyle ps) : first(f), last(l), paraStyle(ps) { }
    unsigned short first;
    unsigned short last;
    ParagraphStyle paraStyle;
  };

  typedef std::vector<ContentChunkReference>::const_iterator ccr_iterator_t;

  MSPUBParser();
  MSPUBParser(const MSPUBParser &);
  MSPUBParser &operator=(const MSPUBParser &);
  bool parseContents(WPXInputStream *input);
  bool parseQuill(WPXInputStream *input);
  bool parseEscher(WPXInputStream *input);
  bool parseEscherDelay(WPXInputStream *input);

  MSPUBBlockInfo parseBlock(WPXInputStream *input, bool skipHierarchicalData = false);
  EscherContainerInfo parseEscherContainer(WPXInputStream *input);

  ContentChunkReference *parseContentChunkReference(WPXInputStream *input, MSPUBBlockInfo block);
  QuillChunkReference parseQuillChunkReference(WPXInputStream *input);
  bool parseDocumentChunk(WPXInputStream *input, const ContentChunkReference &chunk);
  bool parsePageChunk(WPXInputStream *input, const ContentChunkReference &chunk);
  bool parsePaletteChunk(WPXInputStream *input, const ContentChunkReference &chunk);
  bool parseShapes(WPXInputStream *input, MSPUBBlockInfo block, unsigned pageSeqNum);
  bool parseShape(WPXInputStream *input, unsigned seqNum, unsigned pageSeqNum);
  void parsePaletteEntry(WPXInputStream *input, MSPUBBlockInfo block);
  void parseColors(WPXInputStream *input, const QuillChunkReference &chunk);
  void parseFonts(WPXInputStream *input, const QuillChunkReference &chunk);
  void parseDefaultStyle(WPXInputStream *input, const QuillChunkReference &chunk);
  void skipBlock(WPXInputStream *input, MSPUBBlockInfo block);
  bool findEscherContainer(WPXInputStream *input, const EscherContainerInfo &parent, EscherContainerInfo &out, unsigned short type);
  std::map<unsigned short, unsigned> extractEscherValues(WPXInputStream *input, const EscherContainerInfo &record);
  std::vector<TextSpanReference> parseCharacterStyles(WPXInputStream *input, const QuillChunkReference &chunk);
  std::vector<TextParagraphReference> parseParagraphStyles(WPXInputStream *input, const QuillChunkReference &chunk);
  int getColorIndex(WPXInputStream *input, const MSPUBBlockInfo &info);
  unsigned getFontIndex(WPXInputStream *input, const MSPUBBlockInfo &info);
  CharacterStyle getCharacterStyle(WPXInputStream *input, bool inStsh = false);
  ParagraphStyle getParagraphStyle(WPXInputStream *input);
  Fill *getNewFill(const std::map<unsigned short, unsigned> &foptValues, const std::vector<int> &escherDelayIndices, bool &skipIfNotBg);

  WPXInputStream *m_input;
  MSPUBCollector *m_collector;
  std::vector<MSPUBBlockInfo> m_blockInfo;
  std::vector<ContentChunkReference> m_pageChunks;
  std::vector<ContentChunkReference> m_shapeChunks;
  std::vector<ContentChunkReference> m_paletteChunks;
  std::vector<ContentChunkReference> m_unknownChunks;
  ContentChunkReference m_documentChunk;
  int m_lastSeenSeqNum;
  unsigned m_lastAddedImage;
  bool m_seenDocumentChunk;

  static short getBlockDataLength(unsigned type);
  static bool isBlockDataString(unsigned type);
  static PageType getPageTypeBySeqNum(unsigned seqNum);
  static unsigned getEscherElementTailLength(unsigned short type);
  static unsigned getEscherElementAdditionalHeaderLength(unsigned short type);
  static ImgType imgTypeByBlipType(unsigned short type);
  static int getStartOffset(ImgType type, unsigned short initial);
};

} // namespace libmspub

#endif //  __MSPUBRAPHICS_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
