/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_MSPUBCOLLECTOR_H
#define INCLUDED_MSPUBCOLLECTOR_H

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <librevenge/librevenge.h>

#include "Arrow.h"
#include "BorderArtInfo.h"
#include "ColorReference.h"
#include "Coordinate.h"
#include "Dash.h"
#include "EmbeddedFontInfo.h"
#include "Fill.h"
#include "MSPUBContentChunkType.h"
#include "MSPUBTypes.h"
#include "PolygonUtils.h"
#include "Shadow.h"
#include "ShapeGroupElement.h"
#include "ShapeInfo.h"
#include "ShapeType.h"
#include "VerticalAlign.h"
#include "libmspub_utils.h"

namespace libmspub
{
class MSPUBCollector
{
  friend class Fill;
  friend class ImgFill;
  friend class SolidFill;
  friend class GradientFill;
  friend class PatternFill;
public:
  typedef std::list<ContentChunkReference>::const_iterator ccr_iterator_t;

  MSPUBCollector(librevenge::RVNGDrawingInterface *painter);
  virtual ~MSPUBCollector();

  // collector functions
  void collectMetaData(const librevenge::RVNGPropertyList &metaData);

  bool addPage(unsigned seqNum);
  bool addTextString(const std::vector<TextParagraph> &str, unsigned id);
  void addTextShape(unsigned stringId, unsigned seqNum);
  bool addImage(unsigned index, ImgType type, librevenge::RVNGBinaryData img);
  void setBorderImageOffset(unsigned index, unsigned offset);
  librevenge::RVNGBinaryData *addBorderImage(ImgType type, unsigned borderArtIndex);
  void setShapePage(unsigned seqNum, unsigned pageSeqNum);

  void setNextPage(unsigned seqNum);

  void setShapeType(unsigned seqNum, ShapeType type);
  void setShapeCropType(unsigned seqNum, ShapeType cropType);
  void setShapePictureRecolor(unsigned seqNum, const ColorReference &recolor);
  void setShapePictureBrightness(unsigned seqNum, int brightness);
  void setShapePictureContrast(unsigned seqNum, int contrast);
  void setShapeTableInfo(unsigned seqNum, const TableInfo &ti);
  void setShapeBorderImageId(unsigned seqNum, unsigned borderImageId);
  void setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye);
  void setShapeImgIndex(unsigned seqNum, unsigned index);
  void setShapeFill(unsigned seqNum, std::shared_ptr<Fill> fill, bool skipIfNotBg);
  void setShapeDash(unsigned seqNum, const Dash &dash);
  void setAdjustValue(unsigned seqNum, unsigned index, int adjust);
  void setShapeRotation(unsigned seqNum, double rotation);
  void setShapeFlip(unsigned, bool, bool);
  void setShapeMargins(unsigned seqNum, unsigned left, unsigned top, unsigned right, unsigned bottom);
  void setShapeBorderPosition(unsigned seqNum, BorderPosition pos);
  void setShapeCoordinatesRotated90(unsigned seqNum);
  void setShapeCustomPath(unsigned seqNum,
                          const DynamicCustomShape &shape);
  void setShapeClipPath(unsigned seqNum, const std::vector<libmspub::Vertex> &clip);
  void setShapeVerticalTextAlign(unsigned seqNum, VerticalAlign va);
  void designateMasterPage(unsigned seqNum);
  void setMasterPage(unsigned pageSeqNum, unsigned masterSeqNum);
  void setShapeStretchBorderArt(unsigned seqNum);
  void setShapeShadow(unsigned seqNum, const Shadow &shadow);

  // Microsoft "Embedded OpenType" ... need to figure out how to convert
  // this to a sane format and how to get LibreOffice to understand embedded fonts.
  void addEOTFont(const librevenge::RVNGString &name, const librevenge::RVNGBinaryData &data);

  void beginGroup();
  bool endGroup();

  void setShapeLineBackColor(unsigned seqNum, ColorReference backColor);
  void addShapeLine(unsigned seqNum, Line line);
  void setShapeOrder(unsigned seqNum);
  void setPageBgShape(unsigned pageSeqNum, unsigned seqNum);
  void setWidthInEmu(unsigned long);
  void setHeightInEmu(unsigned long);
  void setShapeNumColumns(unsigned seqNum, unsigned numColumns);
  void setShapeColumnSpacing(unsigned seqNum, unsigned spacing);
  void setShapeBeginArrow(unsigned seqNum, const Arrow &arrow);
  void setShapeEndArrow(unsigned seqNum, const Arrow &arrow);

  void addTextColor(ColorReference c);
  void addFont(std::vector<unsigned char> name);

  void addDefaultCharacterStyle(const CharacterStyle &style);
  void addDefaultParagraphStyle(const ParagraphStyle &style);
  void addPaletteColor(Color);
  bool setCurrentGroupSeqNum(unsigned seqNum);

  void useEncodingHeuristic();

  void setTableCellTextEnds(unsigned textId, const std::vector<unsigned> &ends);
  void setTextStringOffset(unsigned textId, unsigned offset);

  bool go();

  bool hasPage(unsigned seqNum) const;
private:

  struct PageInfo
  {
    std::vector<std::shared_ptr<ShapeGroupElement>> m_shapeGroupsOrdered;
    PageInfo() : m_shapeGroupsOrdered() { }
  };

  MSPUBCollector(const MSPUBCollector &);
  MSPUBCollector &operator=(const MSPUBCollector &);

  librevenge::RVNGDrawingInterface *m_painter;
  std::list<ContentChunkReference> m_contentChunkReferences;
  double m_width, m_height;
  bool m_widthSet, m_heightSet;
  unsigned short m_numPages;
  std::map<unsigned, std::vector<TextParagraph> > m_textStringsById;
  std::map<unsigned, PageInfo> m_pagesBySeqNum;
  std::vector<std::pair<ImgType, librevenge::RVNGBinaryData> > m_images;
  std::vector<BorderArtInfo> m_borderImages;
  std::vector<ColorReference> m_textColors;
  std::vector<std::vector<unsigned char> > m_fonts;
  std::vector<CharacterStyle> m_defaultCharStyles;
  std::vector<ParagraphStyle> m_defaultParaStyles;
  std::map<unsigned, ShapeType> m_shapeTypesBySeqNum;
  std::vector<Color> m_paletteColors;
  std::vector<unsigned> m_shapeSeqNumsOrdered;
  std::map<unsigned, unsigned> m_pageSeqNumsByShapeSeqNum;
  std::map<unsigned, unsigned> m_bgShapeSeqNumsByPageSeqNum;
  std::set<unsigned> m_skipIfNotBgSeqNums;
  std::shared_ptr<ShapeGroupElement> m_currentShapeGroup;
  std::vector<std::shared_ptr<ShapeGroupElement>> m_topLevelShapes;
  std::map<unsigned, std::shared_ptr<ShapeGroupElement>> m_groupsBySeqNum;
  std::list<EmbeddedFontInfo> m_embeddedFonts;
  std::map<unsigned, ShapeInfo> m_shapeInfosBySeqNum;
  std::set<unsigned> m_masterPages;
  std::set<unsigned> m_shapesWithCoordinatesRotated90;
  std::map<unsigned, unsigned> m_masterPagesByPageSeqNum;
  std::map<unsigned, std::vector<unsigned> > m_tableCellTextEndsByTextId;
  std::map<unsigned, unsigned> m_stringOffsetsByTextId;
  mutable std::vector<bool> m_calculationValuesSeen;
  std::vector<unsigned> m_pageSeqNumsOrdered;
  bool m_encodingHeuristic;
  std::vector<unsigned char> m_allText;
  mutable boost::optional<const char *> m_calculatedEncoding;
  librevenge::RVNGPropertyList m_metaData;

  // helper functions
  std::vector<int> getShapeAdjustValues(const ShapeInfo &info) const;
  boost::optional<unsigned> getMasterPageSeqNum(unsigned pageSeqNum) const;
  void setRectCoordProps(Coordinate, librevenge::RVNGPropertyList *) const;
  boost::optional<std::vector<libmspub::TextParagraph> > getShapeText(const ShapeInfo &info) const;
  void setupShapeStructures(ShapeGroupElement &elt);
  void addBlackToPaletteIfNecessary();
  void assignShapesToPages();
  void writePage(unsigned pageSeqNum) const;
  void writePageShapes(unsigned pageSeqNum) const;
  void writePageBackground(unsigned pageSeqNum) const;
  void writeImage(double x, double y, double height, double width,
                  ImgType type, const librevenge::RVNGBinaryData &blob,
                  boost::optional<Color> oneBitColor) const;
  bool pageIsMaster(unsigned pageSeqNum) const;

  std::function<void(void)> paintShape(const ShapeInfo &info, const Coordinate &relativeTo, const VectorTransformation2D &foldedTransform, bool isGroup, const VectorTransformation2D &thisTransform) const;
  double getCalculationValue(const ShapeInfo &info, unsigned index, bool recursiveEntry, const std::vector<int> &adjustValues) const;

  librevenge::RVNGPropertyList getCharStyleProps(const CharacterStyle &, boost::optional<unsigned> defaultCharStyleIndex) const;
  librevenge::RVNGPropertyList getParaStyleProps(const ParagraphStyle &, boost::optional<unsigned> defaultParaStyleIndex) const;
  double getSpecialValue(const ShapeInfo &info, const CustomShape &shape, int arg, const std::vector<int> &adjustValues) const;
  void ponderStringEncoding(const std::vector<TextParagraph> &str);
  const char *getCalculatedEncoding() const;
public:
  static librevenge::RVNGString getColorString(const Color &);
};
} // namespace libmspub

#endif /* INCLUDED_MSPUBCOLLECTOR_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
