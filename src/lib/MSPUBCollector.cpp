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
 * Copyright (C) 2012-2013 Brennan Vincent <brennanv@email.arizona.edu>
 * Copyright (C) 2012 Fridrich Strba <fridrich.strba@bluewin.ch>
 *
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

#include <math.h>

#include <unicode/ucsdet.h>

#include "MSPUBCollector.h"
#include "libmspub_utils.h"
#include "MSPUBConstants.h"
#include "MSPUBTypes.h"
#include "PolygonUtils.h"
#include "Coordinate.h"

#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

WPXBinaryData &libmspub::MSPUBCollector::addEOTFont(const WPXString &name)
{
  m_embeddedFonts.push_back(EmbeddedFontInfo(name));
  return m_embeddedFonts.back().m_blob;
}

void libmspub::MSPUBCollector::setShapePictureRecolor(unsigned seqNum,
    const ColorReference &recolor)
{
  m_shapeInfosBySeqNum[seqNum].m_pictureRecolor = recolor;
}

void libmspub::MSPUBCollector::setShapePictureBrightness(unsigned seqNum,
    int brightness)
{
  m_shapeInfosBySeqNum[seqNum].m_pictureBrightness = brightness;
}

void libmspub::MSPUBCollector::setShapePictureContrast(unsigned seqNum,
    int contrast)
{
  m_shapeInfosBySeqNum[seqNum].m_pictureContrast = contrast;
}

void libmspub::MSPUBCollector::setShapeBeginArrow(unsigned seqNum,
    const Arrow &arrow)
{
  m_shapeInfosBySeqNum[seqNum].m_beginArrow = arrow;
}

void libmspub::MSPUBCollector::setShapeVerticalTextAlign(unsigned seqNum,
    VerticalAlign va)
{
  m_shapeInfosBySeqNum[seqNum].m_verticalAlign = va;
}

void libmspub::MSPUBCollector::setShapeEndArrow(unsigned seqNum,
    const Arrow &arrow)
{
  m_shapeInfosBySeqNum[seqNum].m_endArrow = arrow;
}

void libmspub::MSPUBCollector::setShapeTableInfo(unsigned seqNum,
    const TableInfo &ti)
{
  m_shapeInfosBySeqNum[seqNum].m_tableInfo = ti;
}

void libmspub::MSPUBCollector::setShapeNumColumns(unsigned seqNum,
    unsigned numColumns)
{
  m_shapeInfosBySeqNum[seqNum].m_numColumns = numColumns;
}

void libmspub::MSPUBCollector::setShapeColumnSpacing(unsigned seqNum,
    unsigned spacing)
{
  m_shapeInfosBySeqNum[seqNum].m_columnSpacing = spacing;
}

void libmspub::MSPUBCollector::setShapeStretchBorderArt(unsigned seqNum)
{
  m_shapeInfosBySeqNum[seqNum].m_stretchBorderArt = true;
}

void libmspub::MSPUBCollector::setRectCoordProps(Coordinate coord, WPXPropertyList *props) const
{
  int xs = coord.m_xs, ys = coord.m_ys, xe = coord.m_xe, ye = coord.m_ye;
  double x_center = m_width / 2;
  double y_center = m_height / 2;
  props->insert("svg:x", x_center + (double)xs / EMUS_IN_INCH);
  props->insert("svg:y", y_center + (double)ys / EMUS_IN_INCH);
  props->insert("svg:width", (double)(xe - xs) / EMUS_IN_INCH);
  props->insert("svg:height", (double)(ye - ys) / EMUS_IN_INCH);
}

libmspub::Coordinate getFudgedCoordinates(libmspub::Coordinate coord, const std::vector<libmspub::Line> &lines, bool makeBigger, libmspub::BorderPosition borderPosition)
{
  libmspub::Coordinate fudged = coord;
  unsigned topFudge = 0;
  unsigned rightFudge = 0;
  unsigned bottomFudge = 0;
  unsigned leftFudge = 0;
  switch (borderPosition)
  {
  case libmspub::HALF_INSIDE_SHAPE:
    topFudge = (!lines.empty()) ? lines[0].m_widthInEmu / 2 : 0;
    rightFudge = (lines.size() > 1) ? lines[1].m_widthInEmu / 2 : 0;
    bottomFudge = (lines.size() > 2) ? lines[2].m_widthInEmu / 2 : 0;
    leftFudge = (lines.size() > 3) ? lines[3].m_widthInEmu / 2 : 0;
    break;
  case libmspub::OUTSIDE_SHAPE:
    topFudge = (!lines.empty()) ? lines[0].m_widthInEmu : 0;
    rightFudge = (lines.size() > 1) ? lines[1].m_widthInEmu : 0;
    bottomFudge = (lines.size() > 2) ? lines[2].m_widthInEmu : 0;
    leftFudge = (lines.size() > 3) ? lines[3].m_widthInEmu : 0;
    break;
  case libmspub::INSIDE_SHAPE:
  default:
    break;
  }
  if (makeBigger)
  {
    fudged.m_xs -= leftFudge;
    fudged.m_xe += rightFudge;
    fudged.m_ys -= topFudge;
    fudged.m_ye += bottomFudge;
  }
  else
  {
    fudged.m_xs += leftFudge;
    fudged.m_xe -= rightFudge;
    fudged.m_ys += topFudge;
    fudged.m_ye -= bottomFudge;
  }
  return fudged;
}

void libmspub::MSPUBCollector::setNextPage(unsigned pageSeqNum)
{
  m_pageSeqNumsOrdered.push_back(pageSeqNum);
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), m_contentChunkReferences(), m_width(0), m_height(0),
  m_widthSet(false), m_heightSet(false),
  m_numPages(0), m_textStringsById(), m_pagesBySeqNum(),
  m_images(), m_borderImages(),
  m_textColors(), m_fonts(),
  m_defaultCharStyles(), m_defaultParaStyles(), m_shapeTypesBySeqNum(),
  m_paletteColors(), m_shapeSeqNumsOrdered(),
  m_pageSeqNumsByShapeSeqNum(), m_bgShapeSeqNumsByPageSeqNum(),
  m_skipIfNotBgSeqNums(),
  m_currentShapeGroup(NULL), m_topLevelShapes(),
  m_groupsBySeqNum(), m_embeddedFonts(),
  m_shapeInfosBySeqNum(), m_masterPages(),
  m_shapesWithCoordinatesRotated90(),
  m_masterPagesByPageSeqNum(),
  m_tableCellTextEndsVector(), m_stringOffsetsByTextId(),
  m_calculationValuesSeen(), m_pageSeqNumsOrdered(),
  m_encodingHeuristic(false), m_allText(),
  m_calculatedEncoding()
{
}

void libmspub::MSPUBCollector::setTextStringOffset(
  unsigned textId, unsigned offset)
{
  m_stringOffsetsByTextId[textId] = offset;
}

void libmspub::MSPUBCollector::setNextTableCellTextEnds(
  const std::vector<unsigned> &ends)
{
  m_tableCellTextEndsVector.push_back(ends);
}

void libmspub::MSPUBCollector::useEncodingHeuristic()
{
  m_encodingHeuristic = true;
}

void libmspub::MSPUBCollector::setShapeShadow(unsigned seqNum, const Shadow &shadow)
{
  m_shapeInfosBySeqNum[seqNum].m_shadow = shadow;
}

void libmspub::noop(const CustomShape *)
{
}

void libmspub::MSPUBCollector::setShapeCoordinatesRotated90(unsigned seqNum)
{
  m_shapesWithCoordinatesRotated90.insert(seqNum);
}

void libmspub::MSPUBCollector::setShapeBorderImageId(unsigned seqNum, unsigned id)
{
  m_shapeInfosBySeqNum[seqNum].m_borderImgIndex = id;
}

void libmspub::MSPUBCollector::setShapeCustomPath(unsigned seqNum,
    const DynamicCustomShape &shape)
{
  m_shapeInfosBySeqNum[seqNum].m_customShape = shape;
}

void libmspub::MSPUBCollector::setShapeClipPath(unsigned seqNum, const std::vector<libmspub::Vertex> &clip)
{
  m_shapeInfosBySeqNum[seqNum].m_clipPath = clip;
}

void libmspub::MSPUBCollector::beginGroup()
{
  ShapeGroupElement *tmp = new ShapeGroupElement(m_currentShapeGroup);
  if (!m_currentShapeGroup)
  {
    m_topLevelShapes.push_back(tmp);
  }
  m_currentShapeGroup = tmp;
}

bool libmspub::MSPUBCollector::endGroup()
{
  if (!m_currentShapeGroup)
  {
    return false;
  }
  m_currentShapeGroup = m_currentShapeGroup->getParent();
  return true;
}

void libmspub::MSPUBCollector::addShapeLine(unsigned seqNum, Line line)
{
  m_shapeInfosBySeqNum[seqNum].m_lines.push_back(line);
}

void libmspub::MSPUBCollector::setShapeBorderPosition(unsigned seqNum, BorderPosition pos)
{
  m_shapeInfosBySeqNum[seqNum].m_borderPosition = pos;
}

bool libmspub::MSPUBCollector::hasPage(unsigned seqNum) const
{
  return m_pagesBySeqNum.find(seqNum) != m_pagesBySeqNum.end();
}

void libmspub::MSPUBCollector::setShapeMargins(unsigned seqNum, unsigned left, unsigned top, unsigned right, unsigned bottom)
{
  m_shapeInfosBySeqNum[seqNum].m_margins = Margins(left, top, right, bottom);
}

void libmspub::MSPUBCollector::setPageBgShape(unsigned pageSeqNum, unsigned seqNum)
{
  m_bgShapeSeqNumsByPageSeqNum[pageSeqNum] = seqNum;
}

bool libmspub::MSPUBCollector::setCurrentGroupSeqNum(unsigned seqNum)
{
  if (!m_currentShapeGroup)
  {
    return false;
  }
  m_currentShapeGroup->setSeqNum(seqNum);
  m_groupsBySeqNum.insert(std::pair<unsigned, ShapeGroupElement *>(seqNum, m_currentShapeGroup));
  return true;
}

void libmspub::MSPUBCollector::setShapeOrder(unsigned seqNum)
{
  ShapeGroupElement *tmp = new ShapeGroupElement(m_currentShapeGroup, seqNum);
  if (!m_currentShapeGroup)
  {
    m_topLevelShapes.push_back(tmp);
  }
}

void libmspub::MSPUBCollector::addPaletteColor(Color c)
{
  m_paletteColors.push_back(c);
}

void no_op()
{
}

void endShapeGroup(libwpg::WPGPaintInterface *painter)
{
  painter->endLayer();
}

std::vector<int> libmspub::MSPUBCollector::getShapeAdjustValues(const ShapeInfo &info) const
{
  std::vector<int> ret;
  boost::shared_ptr<const CustomShape> ptr_shape = info.getCustomShape();
  if (ptr_shape)
  {
    for (unsigned i = 0; i < ptr_shape->m_numDefaultAdjustValues; ++i)
    {
      ret.push_back(ptr_shape->mp_defaultAdjustValues[i]);
    }
  }
  for (std::map<unsigned, int>::const_iterator i = info.m_adjustValuesByIndex.begin();
       i != info.m_adjustValuesByIndex.end(); ++i)
  {
    unsigned index = i->first;
    int adjustVal = i->second;
    for (unsigned j = info.m_adjustValues.size(); j <= index; ++j)
    {
      ret.push_back(0);
    }
    ret[index] = adjustVal;
  }
  return ret;
}

boost::optional<std::vector<libmspub::TextParagraph> > libmspub::MSPUBCollector::getShapeText(const ShapeInfo &info) const
{
  if (!!info.m_textId)
  {
    unsigned stringId = info.m_textId.get();
    const std::vector<TextParagraph> *ptr_str = getIfExists_const(m_textStringsById, stringId);
    if (ptr_str)
    {
      return *ptr_str;
    }
  }
  return boost::optional<std::vector<TextParagraph> >();
}

void libmspub::MSPUBCollector::setupShapeStructures(ShapeGroupElement &elt)
{
  ShapeInfo *ptr_info = getIfExists(m_shapeInfosBySeqNum, elt.getSeqNum());
  if (ptr_info)
  {
    if (!!ptr_info->m_imgIndex)
    {
      unsigned index = ptr_info->m_imgIndex.get();
      int rot = 0;
      if (!!ptr_info->m_innerRotation)
        rot = ptr_info->m_innerRotation.get();
      if (index - 1 < m_images.size())
      {
        ptr_info->m_fill = boost::shared_ptr<const Fill>(new ImgFill(index, this, false, rot));
      }
    }
    elt.setShapeInfo(*ptr_info);
    std::pair<bool, bool> flips = ptr_info->m_flips.get_value_or(std::pair<bool, bool>(false, false));
    VectorTransformation2D flipsTransform = VectorTransformation2D::fromFlips(flips.second, flips.first);
    double rotation = ptr_info->m_rotation.get_value_or(0);
    rotation = doubleModulo(rotation, 360);
    bool rotBackwards = flips.first ^ flips.second;
    VectorTransformation2D rot = VectorTransformation2D::fromCounterRadians((rotBackwards ? -rotation : rotation) * M_PI / 180);
    elt.setTransform(rot * flipsTransform);
  }
}


boost::function<void(void)> libmspub::MSPUBCollector::paintShape(const ShapeInfo &info, const Coordinate &/* relativeTo*/, const VectorTransformation2D &foldedTransform, bool isGroup, const VectorTransformation2D &thisTransform) const
{
  std::vector<int> adjustValues = getShapeAdjustValues(info);
  if (isGroup)
  {
    m_painter->startLayer(WPXPropertyList());
    return boost::bind(&endShapeGroup, m_painter);
  }
  WPXPropertyList graphicsProps;
  WPXPropertyListVector graphicsPropsVector;
  if (info.m_fill)
  {
    graphicsPropsVector = info.m_fill->getProperties(&graphicsProps);
  }
  bool hasStroke = false;
  bool hasBorderArt = false;
  boost::optional<unsigned> maybeBorderImg = info.m_borderImgIndex;
  if (!!maybeBorderImg && !info.m_lines.empty())
  {
    hasStroke = true;
    hasBorderArt = true;
  }
  else
  {
    for (unsigned i = 0; i < info.m_lines.size(); ++i)
    {
      hasStroke = hasStroke || info.m_lines[i].m_lineExists;
      if (hasStroke)
      {
        break;
      }
    }
  }
  WPXString fill = graphicsProps["draw:fill"] ? graphicsProps["draw:fill"]->getStr() : "none";
  bool hasFill = fill != "none";
  boost::optional<std::vector<TextParagraph> > maybeText = getShapeText(info);
  bool hasText = !!maybeText;
  bool makeLayer = hasBorderArt ||
                   (hasStroke && hasFill) || (hasStroke && hasText) || (hasFill && hasText);
  if (makeLayer)
  {
    if (info.m_clipPath.size() > 0)
    {
      const Coordinate &coord = info.m_coordinates.get_value_or(Coordinate());
      double x, y, height, width;
      x = coord.getXIn(m_width);
      y = coord.getYIn(m_height);
      height = coord.getHeightIn();
      width = coord.getWidthIn();
      m_painter->startLayer(calcClipPath(info.m_clipPath, x, y, height, width, foldedTransform, info.getCustomShape()));
    }
    else
      m_painter->startLayer(WPXPropertyList());
  }
  graphicsProps.insert("draw:stroke", "none");
  const Coordinate &coord = info.m_coordinates.get_value_or(Coordinate());
  BorderPosition borderPosition =
    hasBorderArt ? INSIDE_SHAPE : info.m_borderPosition.get_value_or(HALF_INSIDE_SHAPE);
  ShapeType type;
  if (!!info.m_cropType)
  {
    type = info.m_cropType.get();
  }
  else
  {
    type = info.m_type.get_value_or(RECTANGLE);
  }

  if (hasFill)
  {
    double x, y, height, width;
    x = coord.getXIn(m_width);
    y = coord.getYIn(m_height);
    height = coord.getHeightIn();
    width = coord.getWidthIn();
    if (hasBorderArt)
    {
      double borderImgWidth =
        static_cast<double>(info.m_lines[0].m_widthInEmu) / EMUS_IN_INCH;
      if (height > 2 * borderImgWidth && width >= 2 * borderImgWidth)
      {
        x += borderImgWidth;
        y += borderImgWidth;
        height -= 2 * borderImgWidth;
        width -= 2 * borderImgWidth;
      }
    }
    if (!!info.m_pictureRecolor)
    {
      Color obc = info.m_pictureRecolor.get().getFinalColor(m_paletteColors);
      graphicsProps.insert("draw:color-mode", "greyscale");
      graphicsProps.insert("draw:red",
                           static_cast<double>(obc.r) / 255.0, WPX_PERCENT);
      graphicsProps.insert("draw:blue",
                           static_cast<double>(obc.b) / 255.0, WPX_PERCENT);
      graphicsProps.insert("draw:green",
                           static_cast<double>(obc.g) / 255.0, WPX_PERCENT);
    }
    if (!!info.m_pictureBrightness)
      graphicsProps.insert("draw:luminance", static_cast<double>(info.m_pictureBrightness.get() + 32768.0) / 65536.0, WPX_PERCENT);
    bool shadowPropsInserted = false;
    if (!!info.m_shadow)
    {
      const Shadow &s = info.m_shadow.get();
      if (!needsEmulation(s))
      {
        shadowPropsInserted = true;
        graphicsProps.insert("draw:shadow", "visible");
        graphicsProps.insert("draw:shadow-offset-x", static_cast<double>(s.m_offsetXInEmu) / EMUS_IN_INCH);
        graphicsProps.insert("draw:shadow-offset-y", static_cast<double>(s.m_offsetYInEmu) / EMUS_IN_INCH);
        graphicsProps.insert("draw:shadow-color", getColorString(s.m_color.getFinalColor(m_paletteColors)));
        graphicsProps.insert("draw:shadow-opacity", s.m_opacity, WPX_PERCENT);
      }
      // TODO: Emulate shadows that don't conform
      // to LibreOffice's range of possible shadows.
    }
    m_painter->setStyle(graphicsProps, graphicsPropsVector);

    writeCustomShape(type, graphicsProps, m_painter, x, y, height, width,
                     true, foldedTransform,
                     std::vector<Line>(), boost::bind(&libmspub::MSPUBCollector::getCalculationValue, this, info, _1, false, adjustValues), m_paletteColors, info.getCustomShape());
    if (!!info.m_pictureRecolor)
    {
      graphicsProps.remove("draw:color-mode");
      graphicsProps.remove("draw:red");
      graphicsProps.remove("draw:blue");
      graphicsProps.remove("draw:green");
    }
    if (!!info.m_pictureBrightness)
      graphicsProps.remove("draw:luminance");
    if (shadowPropsInserted)
    {
      graphicsProps.remove("draw:shadow");
      graphicsProps.remove("draw:shadow-offset-x");
      graphicsProps.remove("draw:shadow-offset-y");
      graphicsProps.remove("draw:shadow-color");
      graphicsProps.remove("draw:shadow-opacity");
    }
  }
  const std::vector<Line> &lines = info.m_lines;
  if (hasStroke)
  {
    if (hasBorderArt)
    {
      bool stretch = info.m_stretchBorderArt;
      double x = coord.getXIn(m_width);
      double y = coord.getYIn(m_height);
      double height = coord.getHeightIn();
      double width = coord.getWidthIn();
      double borderImgWidth =
        static_cast<double>(info.m_lines[0].m_widthInEmu) / EMUS_IN_INCH;
      unsigned numImagesHoriz = static_cast<unsigned>(width / borderImgWidth);
      unsigned numImagesVert = static_cast<unsigned>(height / borderImgWidth);
      double borderVertTotalPadding = height - numImagesVert * borderImgWidth;
      double borderHorizTotalPadding = width - numImagesHoriz * borderImgWidth;
      if (numImagesHoriz >= 2 && numImagesVert >= 2)
      {
        unsigned numStretchedImagesVert = static_cast<unsigned>(
                                            0.5 + (height - 2 * borderImgWidth) / borderImgWidth);
        unsigned numStretchedImagesHoriz = static_cast<unsigned>(
                                             0.5 + (width - 2 * borderImgWidth) / borderImgWidth);
        double stretchedImgHeight = stretch ?
                                    (height - 2 * borderImgWidth) / numStretchedImagesVert :
                                    borderImgWidth;
        double stretchedImgWidth = stretch ?
                                   (width - 2 * borderImgWidth) / numStretchedImagesHoriz :
                                   borderImgWidth;
        if (stretch)
        {
          numImagesVert = 2 + numStretchedImagesVert;
          numImagesHoriz = 2 + numStretchedImagesHoriz;
        }
        double borderVertPadding = borderVertTotalPadding / (numImagesVert - 1);
        double borderHorizPadding = borderHorizTotalPadding / (numImagesHoriz - 1);
        if (maybeBorderImg.get() < m_borderImages.size())
        {
          const BorderArtInfo &ba = m_borderImages[maybeBorderImg.get()];
          if (!ba.m_offsets.empty())
          {
            WPXPropertyList baProps;
            baProps.insert("draw:stroke", "none");
            baProps.insert("draw:fill", "solid");
            baProps.insert("draw:fill-color", "#ffffff");
            m_painter->setStyle(baProps, WPXPropertyListVector());
            WPXPropertyList topRectProps;
            topRectProps.insert("svg:x", x);
            topRectProps.insert("svg:y", y);
            topRectProps.insert("svg:height", borderImgWidth);
            topRectProps.insert("svg:width", width);
            m_painter->drawRectangle(topRectProps);
            WPXPropertyList rightRectProps;
            rightRectProps.insert("svg:x", x + width - borderImgWidth);
            rightRectProps.insert("svg:y", y);
            rightRectProps.insert("svg:height", height);
            rightRectProps.insert("svg:width", borderImgWidth);
            m_painter->drawRectangle(rightRectProps);
            WPXPropertyList botRectProps;
            botRectProps.insert("svg:x", x);
            botRectProps.insert("svg:y", y + height - borderImgWidth);
            botRectProps.insert("svg:height", borderImgWidth);
            botRectProps.insert("svg:width", width);
            m_painter->drawRectangle(botRectProps);
            WPXPropertyList leftRectProps;
            leftRectProps.insert("svg:x", x);
            leftRectProps.insert("svg:y", y);
            leftRectProps.insert("svg:height", height);
            leftRectProps.insert("svg:width", borderImgWidth);
            m_painter->drawRectangle(leftRectProps);
            std::vector<unsigned>::const_iterator iOffset = ba.m_offsets.begin();
            boost::optional<Color> oneBitColor;
            if (!!info.m_lineBackColor)
            {
              oneBitColor = info.m_lineBackColor.get().getFinalColor(m_paletteColors);
            }
            // top left
            unsigned iOrdOff = find(ba.m_offsetsOrdered.begin(),
                                    ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              writeImage(x, y, borderImgWidth, borderImgWidth,
                        bi.m_type, bi.m_imgBlob, oneBitColor);
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // top
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              for (unsigned iTop = 1; iTop < numImagesHoriz - 1; ++iTop)
              {
                double imgX = stretch ?
                              x + borderImgWidth + (iTop - 1) * stretchedImgWidth :
                              x + iTop * (borderImgWidth + borderHorizPadding);
                writeImage(imgX, y,
                          borderImgWidth, stretchedImgWidth,
                          bi.m_type, bi.m_imgBlob, oneBitColor);
              }
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // top right
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              writeImage(x + width - borderImgWidth, y,
                        borderImgWidth, borderImgWidth,
                        bi.m_type, bi.m_imgBlob, oneBitColor);
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // right
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              for (unsigned iRight = 1; iRight < numImagesVert - 1; ++iRight)
              {
                double imgY = stretch ?
                              y + borderImgWidth + (iRight - 1) * stretchedImgHeight :
                              y + iRight * (borderImgWidth + borderVertPadding);
                writeImage(x + width - borderImgWidth,
                          imgY,
                          stretchedImgHeight, borderImgWidth,
                          bi.m_type, bi.m_imgBlob, oneBitColor);
              }
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // bottom right
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              writeImage(x + width - borderImgWidth,
                        y + height - borderImgWidth,
                        borderImgWidth, borderImgWidth,
                        bi.m_type, bi.m_imgBlob, oneBitColor);
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // bottom
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              for (unsigned iBot = 1; iBot < numImagesHoriz - 1; ++iBot)
              {
                double imgX = stretch ?
                              x + width - borderImgWidth - iBot * stretchedImgWidth :
                              x + width - borderImgWidth - iBot * (borderImgWidth + borderHorizPadding);
                writeImage(
                  imgX, y + height - borderImgWidth,
                  borderImgWidth, stretchedImgWidth,
                  bi.m_type, bi.m_imgBlob, oneBitColor);
              }
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // bottom left
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              writeImage(x, y + height - borderImgWidth,
                        borderImgWidth, borderImgWidth,
                        bi.m_type, bi.m_imgBlob, oneBitColor);
            }
            if (iOffset + 1 != ba.m_offsets.end())
            {
              ++iOffset;
            }
            // left
            iOrdOff = find(ba.m_offsetsOrdered.begin(),
                          ba.m_offsetsOrdered.end(), *iOffset) - ba.m_offsetsOrdered.begin();
            if (iOrdOff < ba.m_images.size())
            {
              const BorderImgInfo &bi = ba.m_images[iOrdOff];
              for (unsigned iLeft = 1; iLeft < numImagesVert - 1; ++iLeft)
              {
                double imgY = stretch ?
                              y + height - borderImgWidth - iLeft * stretchedImgHeight :
                              y + height - borderImgWidth -
                              iLeft * (borderImgWidth + borderVertPadding);
                writeImage(x, imgY, stretchedImgHeight, borderImgWidth,
                          bi.m_type, bi.m_imgBlob, oneBitColor);
              }
            }
          }
        }
      }
    }
    else
    {
      Coordinate strokeCoord = isShapeTypeRectangle(type) ?
                               getFudgedCoordinates(coord, lines, true, borderPosition) : coord;
      double x, y, height, width;
      x = strokeCoord.getXIn(m_width);
      y = strokeCoord.getYIn(m_height);
      height = strokeCoord.getHeightIn();
      width = strokeCoord.getWidthIn();
      graphicsProps.insert("draw:fill", "none");
      if (!!info.m_dash && !info.m_dash.get().m_dots.empty())
      {
        const Dash &dash = info.m_dash.get();
        graphicsProps.insert("draw:stroke", "dash");
        graphicsProps.insert("draw:distance", dash.m_distance, WPX_INCH);
        switch (dash.m_dotStyle)
        {
        case ROUND_DOT:
          graphicsProps.insert("svg:stroke-linecap", "round");
          break;
        case RECT_DOT:
          graphicsProps.insert("svg:stroke-linecap", "butt");
          break;
        default:
          break;
        }
        for (unsigned i = 0; i < dash.m_dots.size(); ++i)
        {
          WPXString dots;
          dots.sprintf("draw:dots%d", i + 1);
          graphicsProps.insert(dots.cstr(), static_cast<int>(dash.m_dots[i].m_count));
          if (!!dash.m_dots[i].m_length)
          {
            WPXString length;
            length.sprintf("draw:dots%d-length", i + 1);
            graphicsProps.insert(length.cstr(), dash.m_dots[i].m_length.get(), WPX_INCH);
          }
        }
      }
      else
      {
        graphicsProps.insert("draw:stroke", "solid");
      }
      m_painter->setStyle(graphicsProps, graphicsPropsVector);
      writeCustomShape(type, graphicsProps, m_painter, x, y, height, width,
                       false, foldedTransform, lines,
                       boost::bind(
                         &libmspub::MSPUBCollector::getCalculationValue, this, info, _1, false, adjustValues
                       ),
                       m_paletteColors, info.getCustomShape());
    }
  }
  if (hasText)
  {
    const std::vector<TextParagraph> &text = maybeText.get();
    graphicsProps.insert("draw:fill", "none");
    Coordinate textCoord = isShapeTypeRectangle(type) ?
                           getFudgedCoordinates(coord, lines, false, borderPosition) : coord;
    m_painter->setStyle(graphicsProps, graphicsPropsVector);
    WPXPropertyList props;
    setRectCoordProps(textCoord, &props);
    double textRotation = thisTransform.getRotation();
    if (textRotation != 0)
    {
      props.insert("libwpg:rotate", textRotation * 180 / M_PI);
    }
    Margins margins = info.m_margins.get_value_or(Margins());
    props.insert("fo:padding-left", (double)margins.m_left / EMUS_IN_INCH);
    props.insert("fo:padding-top", (double)margins.m_top / EMUS_IN_INCH);
    props.insert("fo:padding-right", (double)margins.m_right / EMUS_IN_INCH);
    props.insert("fo:padding-bottom", (double)margins.m_bottom / EMUS_IN_INCH);
    if(!!info.m_verticalAlign)
    {
      switch (info.m_verticalAlign.get())
      {
      default:
      case TOP:
        props.insert("draw:textarea-vertical-align", "top");
        break;
      case MIDDLE:
        props.insert("draw:textarea-vertical-align", "middle");
        break;
      case BOTTOM:
        props.insert("draw:textarea-vertical-align", "bottom");
        break;
      }
    }
    if (info.m_numColumns)
    {
      unsigned ncols = info.m_numColumns.get_value_or(0);
      if (ncols > 0)
        props.insert("fo:column-count", (int)ncols);
    }
    if (info.m_columnSpacing)
    {
      unsigned ngap = info.m_columnSpacing;
      if (ngap > 0)
        props.insert("fo:column-gap", (double)ngap / EMUS_IN_INCH);
    }
    m_painter->startTextObject(props, WPXPropertyListVector());
    for (unsigned i_lines = 0; i_lines < text.size(); ++i_lines)
    {
      WPXPropertyList paraProps = getParaStyleProps(text[i_lines].style, text[i_lines].style.m_defaultCharStyleIndex);
      m_painter->startTextLine(paraProps);
      for (unsigned i_spans = 0; i_spans < text[i_lines].spans.size(); ++i_spans)
      {
        WPXString textString;
        appendCharacters(textString, text[i_lines].spans[i_spans].chars,
                         getCalculatedEncoding());
        WPXPropertyList charProps = getCharStyleProps(text[i_lines].spans[i_spans].style, text[i_lines].style.m_defaultCharStyleIndex);
        m_painter->startTextSpan(charProps);
        m_painter->insertText(textString);
        m_painter->endTextSpan();
      }
      m_painter->endTextLine();
    }
    m_painter->endTextObject();
  }
  if (makeLayer)
  {
    m_painter->endLayer();
  }
  return &no_op;
}

const char *libmspub::MSPUBCollector::getCalculatedEncoding() const
{
  if (!!m_calculatedEncoding)
  {
    return m_calculatedEncoding.get();
  }
  // modern versions are somewhat sane and use Unicode
  if (! m_encodingHeuristic)
  {
    m_calculatedEncoding = "UTF-16LE";
    return m_calculatedEncoding.get();
  }
  // for older versions of PUB, see if we can get ICU to tell us the encoding.
  UErrorCode status = U_ZERO_ERROR;
  UCharsetDetector *ucd = NULL;
  const UCharsetMatch **matches = NULL;
  const UCharsetMatch *ucm = NULL;
  ucd = ucsdet_open(&status);
  int matchesFound = -1;
  const char *name = NULL;
  const char *windowsName = NULL;
  if (m_allText.empty())
  {
    goto csd_fail;
  }
  if (U_FAILURE(status))
  {
    goto csd_fail;
  }
  // don't worry, the below call doesn't require a null-terminated string.
  ucsdet_setText(ucd, (const char *)(&m_allText[0]), m_allText.size(), &status);
  if (U_FAILURE(status))
  {
    goto csd_fail;
  }
  matches = ucsdet_detectAll(ucd, &matchesFound, &status);
  if (U_FAILURE(status))
  {
    goto csd_fail;
  }
  //find best fit that is an actual Windows encoding
  for (int i = 0; i < matchesFound; ++i)
  {
    ucm = matches[i];
    name = ucsdet_getName(ucm, &status);
    if (U_FAILURE(status))
    {
      goto csd_fail;
    }
    windowsName = windowsCharsetNameByOriginalCharset(name);
    if (windowsName)
    {
      m_calculatedEncoding = windowsName;
      ucsdet_close(ucd);
      return windowsName;
    }
  }
csd_fail:
  ucsdet_close(ucd);
  return "windows-1252"; // Pretty likely to give garbage text, but it's the best we can do.
}

void libmspub::MSPUBCollector::setShapeLineBackColor(unsigned shapeSeqNum,
    ColorReference backColor)
{
  m_shapeInfosBySeqNum[shapeSeqNum].m_lineBackColor = backColor;
}

void libmspub::MSPUBCollector::writeImage(double x, double y,
    double height, double width, ImgType type, const WPXBinaryData &blob,
    boost::optional<Color> oneBitColor) const
{
  WPXPropertyList props;
  if (!!oneBitColor)
  {
    Color obc = oneBitColor.get();
    props.insert("draw:color-mode", "greyscale");
    props.insert("draw:red", static_cast<double>(obc.r) / 255.0, WPX_PERCENT);
    props.insert("draw:blue", static_cast<double>(obc.b) / 255.0, WPX_PERCENT);
    props.insert("draw:green", static_cast<double>(obc.g) / 255.0, WPX_PERCENT);
  }
  props.insert("svg:x", x);
  props.insert("svg:y", y);
  props.insert("svg:width", width);
  props.insert("svg:height", height);
  props.insert("libwpg:mime-type", mimeByImgType(type));
  m_painter->drawGraphicObject(props, blob);
}

double libmspub::MSPUBCollector::getSpecialValue(const ShapeInfo &info, const CustomShape &shape, int arg, const std::vector<int> &adjustValues) const
{
  if (PROP_ADJUST_VAL_FIRST <= arg && PROP_ADJUST_VAL_LAST >= arg)
  {
    unsigned adjustIndex = arg - PROP_ADJUST_VAL_FIRST;
    if (adjustIndex < adjustValues.size())
    {
      if ((shape.m_adjustShiftMask >> adjustIndex) & 0x1)
      {
        return adjustValues[adjustIndex] >> 16;
      }
      return adjustValues[adjustIndex];
    }
    return 0;
  }
  if (arg == ASPECT_RATIO)
  {
    const Coordinate &coord = info.m_coordinates.get_value_or(Coordinate());
    return (double)coord.getWidthIn() / coord.getHeightIn();
  }
  if (arg & OTHER_CALC_VAL)
  {
    return getCalculationValue(info, arg & 0xff, true, adjustValues);
  }
  switch (arg)
  {
  case PROP_GEO_LEFT:
    return 0;
  case PROP_GEO_TOP:
    return 0;
  case PROP_GEO_RIGHT:
    return shape.m_coordWidth;
  case PROP_GEO_BOTTOM:
    return shape.m_coordHeight;
  default:
    break;
  }
  return 0;
}

double libmspub::MSPUBCollector::getCalculationValue(const ShapeInfo &info, unsigned index, bool recursiveEntry, const std::vector<int> &adjustValues) const
{
  boost::shared_ptr<const CustomShape> p_shape = info.getCustomShape();
  if (! p_shape)
  {
    return 0;
  }
  const CustomShape &shape = *p_shape;
  if (index >= shape.m_numCalculations)
  {
    return 0;
  }
  if (! recursiveEntry)
  {
    m_calculationValuesSeen.clear();
    m_calculationValuesSeen.resize(shape.m_numCalculations);
  }
  if (m_calculationValuesSeen[index])
  {
    //recursion detected. The simplest way to avoid infinite recursion, at the "cost"
    // of making custom shape parsing not Turing-complete ;), is to ban recursion entirely.
    return 0;
  }
  m_calculationValuesSeen[index] = true;

  const Calculation &c = shape.mp_calculations[index];
  bool oneSpecial = (c.m_flags & 0x2000) != 0;
  bool twoSpecial = (c.m_flags & 0x4000) != 0;
  bool threeSpecial = (c.m_flags & 0x8000) != 0;

  double valOne = oneSpecial ? getSpecialValue(info, shape, c.m_argOne, adjustValues) : c.m_argOne;
  double valTwo = twoSpecial ? getSpecialValue(info, shape, c.m_argTwo, adjustValues) : c.m_argTwo;
  double valThree = threeSpecial ? getSpecialValue(info, shape, c.m_argThree, adjustValues) : c.m_argThree;
  m_calculationValuesSeen[index] = false;
  switch (c.m_flags & 0xFF)
  {
  case 0:
  case 14:
    return valOne + valTwo - valThree;
  case 1:
    return valOne * valTwo / (valThree == 0 ? 1 : valThree);
  case 2:
    return (valOne + valTwo) / 2;
  case 3:
    return fabs(valOne);
  case 4:
    return std::min(valOne, valTwo);
  case 5:
    return std::max(valOne, valTwo);
  case 6:
    return valOne ? valTwo : valThree;
  case 7:
    return sqrt(valOne * valTwo * valThree);
  case 8:
    return atan2(valTwo, valOne) / (M_PI / 180);
  case 9:
    return valOne * sin(valTwo * (M_PI / 180) );
  case 10:
    return valOne * cos(valTwo * (M_PI / 180) );
  case 11:
    return valOne * cos(atan2(valThree, valTwo));
  case 12:
    return valOne * sin(atan2(valThree, valTwo));
  case 13:
    return sqrt(valOne);
  case 15:
    return valThree * sqrt(1 - (valOne / valTwo) * (valOne / valTwo));
  case 16:
    return valOne * tan(valTwo);
  case 0x80:
    return sqrt(valThree * valThree - valOne * valOne);
  case 0x81:
    return (cos(valThree * (M_PI / 180)) * (valOne - 10800) + sin(valThree * (M_PI / 180)) * (valTwo - 10800)) + 10800;
  case 0x82:
    return -(sin(valThree * (M_PI / 180)) * (valOne - 10800) - cos(valThree * (M_PI / 180)) * (valTwo - 10800)) + 10800;
  default:
    return 0;
  }
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

void libmspub::MSPUBCollector::setShapeRotation(unsigned seqNum, double rotation)
{
  m_shapeInfosBySeqNum[seqNum].m_rotation = rotation;
  m_shapeInfosBySeqNum[seqNum].m_innerRotation = (int)rotation;
}

void libmspub::MSPUBCollector::setShapeFlip(unsigned seqNum, bool flipVertical, bool flipHorizontal)
{
  m_shapeInfosBySeqNum[seqNum].m_flips = std::pair<bool, bool>(flipVertical, flipHorizontal);
}

void libmspub::MSPUBCollector::setShapeType(unsigned seqNum, ShapeType type)
{
  m_shapeInfosBySeqNum[seqNum].m_type = type;
}

void libmspub::MSPUBCollector::setAdjustValue(unsigned seqNum, unsigned index, int adjust)
{
  m_shapeInfosBySeqNum[seqNum].m_adjustValuesByIndex[index] = adjust;
}

void libmspub::MSPUBCollector::addDefaultCharacterStyle(const CharacterStyle &st)
{
  m_defaultCharStyles.push_back(st);
}

void libmspub::MSPUBCollector::addDefaultParagraphStyle(const ParagraphStyle &st)
{
  m_defaultParaStyles.push_back(st);
}

bool libmspub::MSPUBCollector::addPage(unsigned seqNum)
{
  if (! (m_widthSet && m_heightSet) )
  {
    return false;
  }
  MSPUB_DEBUG_MSG(("Adding page of seqnum 0x%x\n", seqNum));
  m_pagesBySeqNum[seqNum] = PageInfo();
  return true;
}

void libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum)
{
  m_shapeInfosBySeqNum[seqNum].m_textId = stringId;
}

void libmspub::MSPUBCollector::setShapeImgIndex(unsigned seqNum, unsigned index)
{
  MSPUB_DEBUG_MSG(("Setting image index of shape with seqnum 0x%x to 0x%x\n", seqNum, index));
  m_shapeInfosBySeqNum[seqNum].m_imgIndex = index;
}

void libmspub::MSPUBCollector::setShapeDash(unsigned seqNum, const Dash &dash)
{
  m_shapeInfosBySeqNum[seqNum].m_dash = dash;
}

void libmspub::MSPUBCollector::setShapeFill(unsigned seqNum, boost::shared_ptr<Fill> fill, bool skipIfNotBg)
{
  m_shapeInfosBySeqNum[seqNum].m_fill = fill;
  if (skipIfNotBg)
  {
    m_skipIfNotBgSeqNums.insert(seqNum);
  }
}

void libmspub::MSPUBCollector::setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye)
{
  m_shapeInfosBySeqNum[seqNum].m_coordinates = Coordinate(xs, ys, xe, ye);
}

void libmspub::MSPUBCollector::addFont(std::vector<unsigned char> name)
{
  m_fonts.push_back(name);
}

WPXPropertyList libmspub::MSPUBCollector::getParaStyleProps(const ParagraphStyle &style, boost::optional<unsigned> defaultParaStyleIndex) const
{
  ParagraphStyle _nothing;
  const ParagraphStyle &defaultStyle = !!defaultParaStyleIndex && defaultParaStyleIndex.get() < m_defaultParaStyles.size() ? m_defaultParaStyles[defaultParaStyleIndex.get()] : _nothing;
  WPXPropertyList ret;
  Alignment align = style.m_align.get_value_or(
                      defaultStyle.m_align.get_value_or(LEFT));
  switch (align)
  {
  case RIGHT:
    ret.insert("fo:text-align", "right");
    break;
  case CENTER:
    ret.insert("fo:text-align", "center");
    break;
  case JUSTIFY:
    ret.insert("fo:text-align", "justify");
    break;
  case LEFT:
  default:
    ret.insert("fo:text-align", "left");
    break;
  }
  LineSpacingInfo info = style.m_lineSpacing.get_value_or(
                           defaultStyle.m_lineSpacing.get_value_or(LineSpacingInfo()));
  LineSpacingType lineSpacingType = info.m_type;
  double lineSpacing = info.m_amount;
  if (!(lineSpacingType == LINE_SPACING_SP && lineSpacing == 1))
  {
    if (lineSpacingType == LINE_SPACING_SP)
    {
      ret.insert("fo:line-height", lineSpacing, WPX_PERCENT);
    }
    else if (lineSpacingType == LINE_SPACING_PT)
    {
      ret.insert("fo:line-height", lineSpacing, WPX_POINT);
    }
  }
  unsigned spaceAfterEmu = style.m_spaceAfterEmu.get_value_or(
                             defaultStyle.m_spaceAfterEmu.get_value_or(0));
  unsigned spaceBeforeEmu = style.m_spaceBeforeEmu.get_value_or(
                              defaultStyle.m_spaceBeforeEmu.get_value_or(0));
  int firstLineIndentEmu = style.m_firstLineIndentEmu.get_value_or(
                             defaultStyle.m_firstLineIndentEmu.get_value_or(0));
  unsigned leftIndentEmu = style.m_leftIndentEmu.get_value_or(
                             defaultStyle.m_leftIndentEmu.get_value_or(0));
  unsigned rightIndentEmu = style.m_rightIndentEmu.get_value_or(
                              defaultStyle.m_rightIndentEmu.get_value_or(0));
  if (spaceAfterEmu != 0)
  {
    ret.insert("fo:margin-bottom", (double)spaceAfterEmu / EMUS_IN_INCH);
  }
  if (spaceBeforeEmu != 0)
  {
    ret.insert("fo:margin-top", (double)spaceBeforeEmu / EMUS_IN_INCH);
  }
  if (firstLineIndentEmu != 0)
  {
    ret.insert("fo:text-indent", (double)firstLineIndentEmu / EMUS_IN_INCH);
  }
  if (leftIndentEmu != 0)
  {
    ret.insert("fo:margin-left", (double)leftIndentEmu / EMUS_IN_INCH);
  }
  if (rightIndentEmu != 0)
  {
    ret.insert("fo:margin-right", (double)rightIndentEmu / EMUS_IN_INCH);
  }
  unsigned dropCapLines = style.m_dropCapLines.get_value_or(
                            defaultStyle.m_dropCapLines.get_value_or(0));
  if (dropCapLines != 0)
  {
    ret.insert("style:drop-cap", (int)dropCapLines);
  }
  unsigned dropCapLetters = style.m_dropCapLetters.get_value_or(
                              defaultStyle.m_dropCapLetters.get_value_or(0));
  if (dropCapLetters != 0)
  {
    ret.insert("style:length", (int)dropCapLetters);
  }
  return ret;
}

WPXPropertyList libmspub::MSPUBCollector::getCharStyleProps(const CharacterStyle &style, boost::optional<unsigned> defaultCharStyleIndex) const
{
  CharacterStyle _nothing = CharacterStyle(false, false, false);
  if (!defaultCharStyleIndex)
  {
    defaultCharStyleIndex = 0;
  }
  const CharacterStyle &defaultCharStyle = defaultCharStyleIndex.get() < m_defaultCharStyles.size() ? m_defaultCharStyles[defaultCharStyleIndex.get()] : _nothing;
  WPXPropertyList ret;
  if (style.italic ^ defaultCharStyle.italic)
  {
    ret.insert("fo:font-style", "italic");
  }
  if (style.bold ^ defaultCharStyle.bold)
  {
    ret.insert("fo:font-weight", "bold");
  }
  if (style.underline ^ defaultCharStyle.underline)
  {
    ret.insert("style:text-underline-type", "single");
  }
  if (!!style.textSizeInPt)
  {
    ret.insert("fo:font-size", style.textSizeInPt.get() / POINTS_IN_INCH);
  }
  else if (!!defaultCharStyle.textSizeInPt)
  {
    ret.insert("fo:font-size", defaultCharStyle.textSizeInPt.get()
               / POINTS_IN_INCH);
  }
  if (style.colorIndex >= 0 && (size_t)style.colorIndex < m_textColors.size())
  {
    ret.insert("fo:color", getColorString(m_textColors[style.colorIndex].getFinalColor(m_paletteColors)));
  }
  else if (defaultCharStyle.colorIndex >= 0 && (size_t)defaultCharStyle.colorIndex < m_textColors.size())
  {
    ret.insert("fo:color", getColorString(m_textColors[defaultCharStyle.colorIndex].getFinalColor(m_paletteColors)));
  }
  else
  {
    ret.insert("fo:color", getColorString(Color(0, 0, 0)));  // default color is black
  }
  if (!!style.fontIndex &&
      style.fontIndex.get() < m_fonts.size())
  {
    WPXString str;
    appendCharacters(str, m_fonts[style.fontIndex.get()],
                     getCalculatedEncoding());
    ret.insert("style:font-name", str);
  }
  else if (!!defaultCharStyle.fontIndex &&
           defaultCharStyle.fontIndex.get() < m_fonts.size())
  {
    WPXString str;
    appendCharacters(str, m_fonts[defaultCharStyle.fontIndex.get()],
                     getCalculatedEncoding());
    ret.insert("style:font-name", str);
  }
  else if (!m_fonts.empty())
  {
    WPXString str;
    appendCharacters(str, m_fonts[0],
                     getCalculatedEncoding());
    ret.insert("style:font-name", str);
  }
  switch (style.superSubType)
  {
  case SUPERSCRIPT:
    ret.insert("style:text-position", "50% 67%");
    break;
  case SUBSCRIPT:
    ret.insert("style:text-position", "-50% 67%");
    break;
  default:
    break;
  }
  return ret;
}

WPXString libmspub::MSPUBCollector::getColorString(const Color &color)
{
  WPXString ret;
  ret.sprintf("#%.2x%.2x%.2x",(unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b);
  MSPUB_DEBUG_MSG(("String for r: 0x%x, g: 0x%x, b: 0x%x is %s\n", color.r, color.g, color.b, ret.cstr()));
  return ret;
}

void libmspub::MSPUBCollector::addBlackToPaletteIfNecessary()
{
  if (m_paletteColors.size() < 8)
  {
    m_paletteColors.insert(m_paletteColors.begin(), Color());
  }
}

void libmspub::MSPUBCollector::assignShapesToPages()
{
  for (unsigned i = 0; i < m_topLevelShapes.size(); ++i)
  {
    unsigned *ptr_pageSeqNum = getIfExists(m_pageSeqNumsByShapeSeqNum, m_topLevelShapes[i].getSeqNum());
    m_topLevelShapes[i].setup(boost::bind(&libmspub::MSPUBCollector::setupShapeStructures, this, _1));
    if (ptr_pageSeqNum)
    {
      PageInfo *ptr_page = getIfExists(m_pagesBySeqNum, *ptr_pageSeqNum);
      if (ptr_page)
      {
        ptr_page->m_shapeGroupsOrdered.push_back(&m_topLevelShapes[i]);
      }
    }
  }
}

boost::optional<unsigned> libmspub::MSPUBCollector::getMasterPageSeqNum(unsigned pageSeqNum) const
{
  boost::optional<unsigned> toReturn;
  const unsigned *ptr_masterSeqNum = getIfExists_const(m_masterPagesByPageSeqNum, pageSeqNum);
  if (ptr_masterSeqNum && m_masterPages.find(*ptr_masterSeqNum) != m_masterPages.end())
  {
    return *ptr_masterSeqNum;
  }
  return toReturn;
}

void libmspub::MSPUBCollector::writePage(unsigned pageSeqNum) const
{
  const PageInfo &pageInfo = m_pagesBySeqNum.find(pageSeqNum)->second;
  WPXPropertyList pageProps;
  if (m_widthSet)
  {
    pageProps.insert("svg:width", m_width);
  }
  if (m_heightSet)
  {
    pageProps.insert("svg:height", m_height);
  }
  const std::vector<ShapeGroupElement *> &shapeGroupsOrdered = pageInfo.m_shapeGroupsOrdered;
  if (!shapeGroupsOrdered.empty())
  {
    m_painter->startGraphics(pageProps);
    boost::optional<unsigned> masterSeqNum = getMasterPageSeqNum(pageSeqNum);
    bool hasMaster = !!masterSeqNum;
    if (hasMaster)
    {
      writePageBackground(masterSeqNum.get());
    }
    writePageBackground(pageSeqNum);
    if (hasMaster)
    {
      writePageShapes(masterSeqNum.get());
    }
    writePageShapes(pageSeqNum);
    m_painter->endGraphics();
  }
}

void libmspub::MSPUBCollector::writePageShapes(unsigned pageSeqNum) const
{
  const PageInfo &pageInfo = m_pagesBySeqNum.find(pageSeqNum)->second;
  const std::vector<ShapeGroupElement *> &shapeGroupsOrdered = pageInfo.m_shapeGroupsOrdered;
  for (unsigned i = 0; i < shapeGroupsOrdered.size(); ++i)
  {
    ShapeGroupElement *shapeGroup = shapeGroupsOrdered[i];
    shapeGroup->visit(boost::bind(&libmspub::MSPUBCollector::paintShape, this, _1, _2, _3, _4, _5));
  }
}

void libmspub::MSPUBCollector::writePageBackground(unsigned pageSeqNum) const
{
  const unsigned *ptr_fillSeqNum = getIfExists_const(m_bgShapeSeqNumsByPageSeqNum, pageSeqNum);
  if (ptr_fillSeqNum)
  {
    boost::shared_ptr<const Fill> ptr_fill;
    const ShapeInfo *ptr_info = getIfExists_const(m_shapeInfosBySeqNum, *ptr_fillSeqNum);
    if (ptr_info)
    {
      ptr_fill = ptr_info->m_fill;
    }
    if (ptr_fill)
    {
      ShapeInfo bg;
      bg.m_type = RECTANGLE;
      Coordinate wholePage(-m_width/2 * EMUS_IN_INCH, -m_height/2 * EMUS_IN_INCH, m_width/2 * EMUS_IN_INCH, m_height/2 * EMUS_IN_INCH);
      bg.m_coordinates = wholePage;
      bg.m_pageSeqNum = pageSeqNum;
      bg.m_fill = ptr_fill;
      paintShape(bg, Coordinate(), VectorTransformation2D(), false, VectorTransformation2D());
    }
  }
}

bool libmspub::MSPUBCollector::pageIsMaster(unsigned pageSeqNum) const
{
  return m_masterPages.find(pageSeqNum) != m_masterPages.end();
}

bool libmspub::MSPUBCollector::go()
{
  addBlackToPaletteIfNecessary();
  assignShapesToPages();
  if (m_pageSeqNumsOrdered.empty())
  {
    for (std::map<unsigned, PageInfo>::const_iterator i = m_pagesBySeqNum.begin();
         i != m_pagesBySeqNum.end(); ++i)
    {
      if (!pageIsMaster(i->first))
      {
        writePage(i->first);
      }
    }
  }
  else
  {
    for (unsigned i = 0; i < m_pageSeqNumsOrdered.size(); ++i)
    {
      std::map<unsigned, PageInfo>::const_iterator iter =
        m_pagesBySeqNum.find(m_pageSeqNumsOrdered[i]);
      if (iter != m_pagesBySeqNum.end() && !pageIsMaster(iter->first))
      {
        writePage(iter->first);
      }
    }
  }
  return true;
}


bool libmspub::MSPUBCollector::addTextString(const std::vector<TextParagraph> &str, unsigned id)
{
  MSPUB_DEBUG_MSG(("addTextString, id: 0x%x\n", id));
  m_textStringsById[id] = str;
  if (m_encodingHeuristic)
  {
    ponderStringEncoding(str);
  }
  return true; //FIXME: Warn if the string already existed in the map.
}

void libmspub::MSPUBCollector::ponderStringEncoding(
  const std::vector<TextParagraph> &str)
{
  for (unsigned i = 0; i < str.size(); ++i)
  {
    for (unsigned j = 0; j < str[i].spans.size(); ++j)
    {
      const std::vector<unsigned char> &chars = str[i].spans[j].chars;
      m_allText.insert(m_allText.end(), chars.begin(), chars.end());
    }
  }
}

void libmspub::MSPUBCollector::setWidthInEmu(unsigned long widthInEmu)
{
  //FIXME: Warn if this is called twice
  m_width = ((double)widthInEmu) / EMUS_IN_INCH;
  m_widthSet = true;
}

void libmspub::MSPUBCollector::setHeightInEmu(unsigned long heightInEmu)
{
  //FIXME: Warn if this is called twice
  m_height = ((double)heightInEmu) / EMUS_IN_INCH;
  m_heightSet = true;
}

bool libmspub::MSPUBCollector::addImage(unsigned index, ImgType type, WPXBinaryData img)
{
  while (m_images.size() < index)
  {
    m_images.push_back(std::pair<ImgType, WPXBinaryData>(UNKNOWN, WPXBinaryData()));
  }
  if (index > 0)
  {
    MSPUB_DEBUG_MSG(("Image at index %d and of type 0x%x added.\n", index, type));
    m_images[index - 1] = std::pair<ImgType, WPXBinaryData>(type, img);
  }
  else
  {
    MSPUB_DEBUG_MSG(("0 is not a valid index for image, ignoring.\n"));
  }
  return index > 0;
}

WPXBinaryData *libmspub::MSPUBCollector::addBorderImage(ImgType type,
    unsigned borderArtIndex)
{
  while (borderArtIndex >= m_borderImages.size())
  {
    m_borderImages.push_back(BorderArtInfo());
  }
  m_borderImages[borderArtIndex].m_images.push_back(BorderImgInfo(type));
  return &(m_borderImages[borderArtIndex].m_images.back().m_imgBlob);
}

void libmspub::MSPUBCollector::setBorderImageOffset(unsigned index, unsigned offset)
{
  while (index >= m_borderImages.size())
  {
    m_borderImages.push_back(BorderArtInfo());
  }
  BorderArtInfo &bai = m_borderImages[index];
  bai.m_offsets.push_back(offset);
  bool added = false;
  for (std::vector<unsigned>::iterator i = bai.m_offsetsOrdered.begin();
       i != bai.m_offsetsOrdered.end(); ++i)
  {
    if (*i >= offset)
    {
      bai.m_offsetsOrdered.insert(i, offset);
      added = true;
      break;
    }
  }
  if (!added)
  {
    bai.m_offsetsOrdered.push_back(offset);
  }
}

void libmspub::MSPUBCollector::setShapePage(unsigned seqNum, unsigned pageSeqNum)
{
  m_shapeInfosBySeqNum[seqNum].m_pageSeqNum = pageSeqNum;
  m_pageSeqNumsByShapeSeqNum[seqNum] = pageSeqNum;
}

void libmspub::MSPUBCollector::addTextColor(ColorReference c)
{
  m_textColors.push_back(c);
}

void libmspub::MSPUBCollector::designateMasterPage(unsigned seqNum)
{
  m_masterPages.insert(seqNum);
}

void libmspub::MSPUBCollector::setMasterPage(unsigned seqNum, unsigned masterPageSeqNum)
{
  m_masterPagesByPageSeqNum[seqNum] = masterPageSeqNum;
}

void libmspub::MSPUBCollector::setShapeCropType(unsigned seqNum, ShapeType cropType)
{
  m_shapeInfosBySeqNum[seqNum].m_cropType = cropType;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
