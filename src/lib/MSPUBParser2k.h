/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __MSPUBPARSER2K_H__
#define __MSPUBPARSER2K_H__

#include <vector>
#include <map>

#include "MSPUBParser.h"
#include "ShapeType.h"

namespace libmspub
{

class MSPUBParser2k : public MSPUBParser
{
  static ShapeType getShapeType(unsigned char shapeSpecifier);
  std::vector<unsigned> m_imageDataChunkIndices;
  std::vector<unsigned> m_quillColorEntries;
  std::map<unsigned, std::vector<unsigned> > m_chunkChildIndicesById;

protected:
  // helper functions
  bool parse2kShapeChunk(const ContentChunkReference &chunk, librevenge::RVNGInputStream *input,
                         boost::optional<unsigned> pageSeqNum = boost::optional<unsigned>(),
                         bool topLevelCall = true);
  void parseShapeLine(librevenge::RVNGInputStream *input, bool isRectangle, unsigned offset, unsigned seqNum);
  void parseShapeType(librevenge::RVNGInputStream *input,
                      unsigned seqNum, unsigned chunkOffset,
                      bool &isGroup, bool &isLine, bool &isImage, bool &isRectangle,
                      unsigned &flagsOffset);
  void parseShapeRotation(librevenge::RVNGInputStream *input, bool isGroup, bool isLine, unsigned seqNum,
                          unsigned chunkOffset);
  void parseShapeFlips(librevenge::RVNGInputStream *input, unsigned flagsOffset, unsigned seqNum,
                       unsigned chunkOffset);
  void parseShapeCoordinates(librevenge::RVNGInputStream *input, unsigned seqNum, unsigned chunkOffset);
  bool parseGroup(librevenge::RVNGInputStream *input, unsigned seqNum, unsigned page);
  void assignShapeImgIndex(unsigned seqNum);
  void parseShapeFill(librevenge::RVNGInputStream *input, unsigned seqNum, unsigned chunkOffset);
  bool parseContents(librevenge::RVNGInputStream *input);
  virtual bool parseDocument(librevenge::RVNGInputStream *input);
  virtual unsigned getColorIndexByQuillEntry(unsigned entry);
  virtual int translateCoordinateIfNecessary(int coordinate) const;
  virtual unsigned getFirstLineOffset() const;
  virtual unsigned getSecondLineOffset() const;
  virtual unsigned getShapeFillTypeOffset() const;
  virtual unsigned getShapeFillColorOffset() const;
  virtual unsigned short getTextMarker() const;
  virtual unsigned getTextIdOffset() const;
  static Color getColorBy2kIndex(unsigned char index);
  static Color getColorBy2kHex(unsigned hex);
  static unsigned translate2kColorReference(unsigned ref2k);
  static PageType getPageTypeBySeqNum(unsigned seqNum);
  virtual void parseContentsTextIfNecessary(librevenge::RVNGInputStream *input);
public:
  explicit MSPUBParser2k(librevenge::RVNGInputStream *input, MSPUBCollector *collector);
  virtual bool parse();
  virtual ~MSPUBParser2k();
};

} // namespace libmspub

#endif //  __MSPUBPARSER2K_H__

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
