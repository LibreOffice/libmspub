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

#ifndef __MSPUBPARSER_H__
#define __MSPUBPARSER_H__

#include <map>
#include <set>
#include <vector>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <librevenge/librevenge.h>
#include <librevenge/librevenge.h>

#include "MSPUBTypes.h"
#include "Fill.h"
#include "Coordinate.h"
#include "PolygonUtils.h"

namespace libmspub
{
class MSPUBCollector;
class FindBySeqNum
{
  unsigned seqNum;
public:
  FindBySeqNum(unsigned sn) : seqNum(sn) { }
  bool operator()(const libmspub::ContentChunkReference &ref)
  {
    return ref.seqNum == seqNum;
  }
};

class FindByParentSeqNum
{
  unsigned seqNum;
public:
  FindByParentSeqNum(unsigned sn) : seqNum(sn) { }
  bool operator()(const libmspub::ContentChunkReference &ref)
  {
    return ref.parentSeqNum == seqNum;
  }
};

struct FOPTValues
{
  std::map<unsigned short, unsigned> m_scalarValues;
  std::map<unsigned short, std::vector<unsigned char> > m_complexValues;
  FOPTValues() : m_scalarValues(), m_complexValues()
  {
  }
};

class MSPUBParser
{
public:
  explicit MSPUBParser(librevenge::RVNGInputStream *input, MSPUBCollector *collector);
  virtual ~MSPUBParser();
  virtual bool parse();
protected:
  virtual unsigned getColorIndexByQuillEntry(unsigned entry);

  struct TextSpanReference
  {
    TextSpanReference(unsigned short f, unsigned short l, const CharacterStyle &cs) : first(f), last(l), charStyle(cs) { }
    unsigned short first;
    unsigned short last;
    CharacterStyle charStyle;
  };

  struct TextParagraphReference
  {
    TextParagraphReference(unsigned short f, unsigned short l, const ParagraphStyle &ps) : first(f), last(l), paraStyle(ps) { }
    unsigned short first;
    unsigned short last;
    ParagraphStyle paraStyle;
  };

  typedef std::vector<ContentChunkReference>::const_iterator ccr_iterator_t;

  MSPUBParser();
  MSPUBParser(const MSPUBParser &);
  MSPUBParser &operator=(const MSPUBParser &);
  virtual bool parseContents(librevenge::RVNGInputStream *input);
  bool parseQuill(librevenge::RVNGInputStream *input);
  bool parseEscher(librevenge::RVNGInputStream *input);
  bool parseEscherDelay(librevenge::RVNGInputStream *input);

  MSPUBBlockInfo parseBlock(librevenge::RVNGInputStream *input, bool skipHierarchicalData = false);
  EscherContainerInfo parseEscherContainer(librevenge::RVNGInputStream *input);

  bool parseContentChunkReference(librevenge::RVNGInputStream *input, MSPUBBlockInfo block);
  QuillChunkReference parseQuillChunkReference(librevenge::RVNGInputStream *input);
  bool parseDocumentChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk);
  bool parsePageChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk);
  bool parsePaletteChunk(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk);
  bool parsePageShapeList(librevenge::RVNGInputStream *input, MSPUBBlockInfo block, unsigned pageSeqNum);
  bool parseShape(librevenge::RVNGInputStream *input, const ContentChunkReference &chunk);
  bool parseBorderArtChunk(librevenge::RVNGInputStream *input,
                           const ContentChunkReference &chunk);
  bool parseFontChunk(librevenge::RVNGInputStream *input,
                      const ContentChunkReference &chunk);
  void parsePaletteEntry(librevenge::RVNGInputStream *input, MSPUBBlockInfo block);
  void parseColors(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk);
  void parseFonts(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk);
  void parseDefaultStyle(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk);
  void parseShapeGroup(librevenge::RVNGInputStream *input, const EscherContainerInfo &spgr, Coordinate parentCoordinateSystem, Coordinate parentGroupAbsoluteCoord);
  void skipBlock(librevenge::RVNGInputStream *input, MSPUBBlockInfo block);
  void parseEscherShape(librevenge::RVNGInputStream *input, const EscherContainerInfo &sp, Coordinate &parentCoordinateSystem, Coordinate &parentGroupAbsoluteCoord);
  bool findEscherContainer(librevenge::RVNGInputStream *input, const EscherContainerInfo &parent, EscherContainerInfo &out, unsigned short type);
  bool findEscherContainerWithTypeInSet(librevenge::RVNGInputStream *input, const EscherContainerInfo &parent, EscherContainerInfo &out, std::set<unsigned short> types);
  std::map<unsigned short, unsigned> extractEscherValues(librevenge::RVNGInputStream *input, const EscherContainerInfo &record);
  FOPTValues extractFOPTValues(librevenge::RVNGInputStream *input,
                               const libmspub::EscherContainerInfo &record);
  std::vector<TextSpanReference> parseCharacterStyles(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk);
  std::vector<TextParagraphReference> parseParagraphStyles(librevenge::RVNGInputStream *input, const QuillChunkReference &chunk);
  std::vector<Calculation> parseGuides(const std::vector<unsigned char>
                                       &guideData);
  std::vector<Vertex> parseVertices(const std::vector<unsigned char>
                                    &vertexData);
  std::vector<unsigned> parseTableCellDefinitions(librevenge::RVNGInputStream *input,
                                                  const QuillChunkReference &chunk);
  std::vector<unsigned short> parseSegments(
    const std::vector<unsigned char> &segmentData);
  DynamicCustomShape getDynamicCustomShape(
    const std::vector<unsigned char> &vertexData,
    const std::vector<unsigned char> &segmentData,
    const std::vector<unsigned char> &guideData,
    unsigned geoWidth, unsigned geoHeight);
  int getColorIndex(librevenge::RVNGInputStream *input, const MSPUBBlockInfo &info);
  unsigned getFontIndex(librevenge::RVNGInputStream *input, const MSPUBBlockInfo &info);
  CharacterStyle getCharacterStyle(librevenge::RVNGInputStream *input);
  ParagraphStyle getParagraphStyle(librevenge::RVNGInputStream *input);
  boost::shared_ptr<Fill> getNewFill(const std::map<unsigned short, unsigned> &foptValues, bool &skipIfNotBg, std::map<unsigned short, std::vector<unsigned char> > &foptVal);

  librevenge::RVNGInputStream *m_input;
  MSPUBCollector *m_collector;
  std::vector<MSPUBBlockInfo> m_blockInfo;
  std::vector<ContentChunkReference> m_contentChunks;
  std::vector<unsigned> m_cellsChunkIndices;
  std::vector<unsigned> m_pageChunkIndices;
  std::vector<unsigned> m_shapeChunkIndices;
  std::vector<unsigned> m_paletteChunkIndices;
  std::vector<unsigned> m_borderArtChunkIndices;
  std::vector<unsigned> m_fontChunkIndices;
  std::vector<unsigned> m_unknownChunkIndices;
  boost::optional<unsigned> m_documentChunkIndex;
  int m_lastSeenSeqNum;
  unsigned m_lastAddedImage;
  std::vector<int> m_alternateShapeSeqNums;
  std::vector<int> m_escherDelayIndices;

  static short getBlockDataLength(unsigned type);
  static bool isBlockDataString(unsigned type);
  static PageType getPageTypeBySeqNum(unsigned seqNum);
  static unsigned getEscherElementTailLength(unsigned short type);
  static unsigned getEscherElementAdditionalHeaderLength(unsigned short type);
  static ImgType imgTypeByBlipType(unsigned short type);
  static int getStartOffset(ImgType type, unsigned short initial);
  static bool lineExistsByFlagPointer(unsigned *flags,
                                      unsigned *geomFlags = NULL);
};

} // namespace libmspub

#endif //  __MSPUBRAPHICS_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
