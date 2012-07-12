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
#include "MSPUBCollector.h"
#include "libmspub_utils.h"
#include "MSPUBConstants.h"
#include "MSPUBTypes.h"
#include "PolygonUtils.h"
#include "Coordinate.h"

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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), m_contentChunkReferences(), m_width(0), m_height(0),
  m_widthSet(false), m_heightSet(false),
  m_numPages(0), m_textStringsById(), m_pagesBySeqNum(),
  m_images(),
  m_textColors(), m_fonts(),
  m_defaultCharStyles(), m_defaultParaStyles(), m_shapeTypesBySeqNum(),
  m_possibleImageShapeSeqNums(),
  m_paletteColors(), m_shapeSeqNumsOrdered(),
  m_pageSeqNumsByShapeSeqNum(), m_bgShapeSeqNumsByPageSeqNum(),
  m_skipIfNotBgSeqNums(),
  m_currentShapeGroup(NULL), m_topLevelShapes(),
  m_groupsBySeqNum(), m_shapeInfosBySeqNum(),
  m_calculationValuesSeen()
{
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
  const CustomShape *ptr_shape = getCustomShape(info.m_type.get_value_or(RECTANGLE));
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
  if (info.m_textInfo.is_initialized())
  {
    unsigned stringId = info.m_textInfo.get().first;
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
    elt.setShapeInfo(*ptr_info);
    unsigned rotation = ptr_info->m_rotation.get_value_or(0);
    rotation = std::fmod(rotation, 360);
    elt.setIsRotated90( (rotation >= 45 && rotation < 135) || (rotation >= 225 && rotation < 315));
    VectorTransformation2D rot = VectorTransformation2D::fromCounterRadians(rotation);
    std::pair<bool, bool> flips = ptr_info->m_flips.get_value_or(std::pair<bool, bool>(false, false));
    VectorTransformation2D flipsTransform = VectorTransformation2D::fromFlips(flips.second, flips.first);
    elt.setTransform(rot * flipsTransform);
  }
}


boost::function<void(void)> libmspub::MSPUBCollector::paintShape(const ShapeInfo &info, const Coordinate &/* relativeTo*/, const VectorTransformation2D &foldedTransform, bool isGroup, const VectorTransformation2D &thisTransform, bool isRotated90) const
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
  bool hasStroke = !info.m_lines.empty();
  WPXString fill = graphicsProps["draw:fill"] ? graphicsProps["draw:fill"]->getStr() : "none";
  bool hasFill = fill != "none";
  boost::optional<std::vector<TextParagraph> > maybeText = getShapeText(info);
  bool hasText = maybeText.is_initialized();
  bool makeLayer = (hasStroke && hasFill) || (hasStroke && hasText) || (hasFill && hasText);
  if (makeLayer)
  {
    m_painter->startLayer(WPXPropertyList());
  }
  graphicsProps.insert("draw:stroke", "none");
  const Coordinate &coord = info.m_coordinates.get_value_or(Coordinate());
  BorderPosition borderPosition = info.m_borderPosition.get_value_or(HALF_INSIDE_SHAPE);
  ShapeType type = info.m_type.get_value_or(RECTANGLE);
  if (hasFill)
  {
    double x, y, height, width;
    if (isRotated90)
    {
      double initialX = coord.getXIn(m_width);
      double initialY = coord.getYIn(m_height);
      double initialWidth = coord.getWidthIn();
      double initialHeight = coord.getHeightIn();
      double centerX = initialX + initialWidth / 2;
      double centerY = initialY + initialHeight / 2;
      x = centerX - initialHeight / 2;
      y = centerY - initialWidth / 2;
      height = initialWidth;
      width = initialHeight;
    }
    else
    {
      x = coord.getXIn(m_width);
      y = coord.getYIn(m_height);
      height = coord.getHeightIn();
      width = coord.getWidthIn();
    }
    m_painter->setStyle(graphicsProps, graphicsPropsVector);

    writeCustomShape(type, graphicsProps, m_painter, x, y, height, width,
                     true, foldedTransform,
                     std::vector<Line>(), boost::bind(&libmspub::MSPUBCollector::getCalculationValue, this, info, _1, false, adjustValues), m_paletteColors);
  }
  const std::vector<Line> &lines = info.m_lines;
  if (hasStroke)
  {
    Coordinate strokeCoord = isShapeTypeRectangle(type) ?
                             getFudgedCoordinates(coord, lines, true, borderPosition) : coord;
    double x, y, height, width;
    if (isRotated90)
    {
      double initialX = strokeCoord.getXIn(m_width);
      double initialY = strokeCoord.getYIn(m_height);
      double initialWidth = strokeCoord.getWidthIn();
      double initialHeight = strokeCoord.getHeightIn();
      double centerX = initialX + initialWidth / 2;
      double centerY = initialY + initialHeight / 2;
      x = centerX - initialHeight / 2;
      y = centerY - initialWidth / 2;
      height = initialWidth;
      width = initialHeight;
    }
    else
    {
      x = strokeCoord.getXIn(m_width);
      y = strokeCoord.getYIn(m_height);
      height = strokeCoord.getHeightIn();
      width = strokeCoord.getWidthIn();
    }
    graphicsProps.insert("draw:fill", "none");
    graphicsProps.insert("draw:stroke", "solid");
    m_painter->setStyle(graphicsProps, graphicsPropsVector);
    writeCustomShape(type, graphicsProps, m_painter, x, y, height, width,
                     false, foldedTransform, lines,
                     boost::bind(
                       &libmspub::MSPUBCollector::getCalculationValue, this, info, _1, false, adjustValues
                     ),
                     m_paletteColors);
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
    m_painter->startTextObject(props, WPXPropertyListVector());
    for (unsigned i_lines = 0; i_lines < text.size(); ++i_lines)
    {
      WPXPropertyList paraProps = getParaStyleProps(text[i_lines].style, text[i_lines].style.defaultCharStyleIndex);
      m_painter->startTextLine(paraProps);
      for (unsigned i_spans = 0; i_spans < text[i_lines].spans.size(); ++i_spans)
      {
        WPXString textString;
        appendCharacters(textString, text[i_lines].spans[i_spans].chars);
        WPXPropertyList charProps = getCharStyleProps(text[i_lines].spans[i_spans].style, text[i_lines].style.defaultCharStyleIndex);
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
  const CustomShape *p_shape = getCustomShape(info.m_type.get_value_or(RECTANGLE));
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
/*
void libmspub::GeometricShape::write(libwpg::WPGPaintInterface *painter)
{
  // If a shape includes text, and if the font from the PUB file
  // is not available in LibreOffice (unfortunately we haven't gotten embedded fonts working yet...)
  // then the bounding box of the text might be different from the size of the shape.
  // So which size should be used? We should compromise by using the size of the text for rectangular shapes,
  // and the size of the shape for other shapes. However currently the size of the shape is used in all cases,
  // causing ugliness in some documents.
  writeCustomShape(m_type, graphicsProps, painter, x, y, height, width, this, m_closeEverything, m_foldedTransform, m_drawStroke ? m_lines : std::vector<Line>());
}
*/

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

void libmspub::MSPUBCollector::setShapeRotation(unsigned seqNum, double rotation)
{
  m_shapeInfosBySeqNum[seqNum].m_rotation = rotation;
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

void libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum)
{
  m_shapeInfosBySeqNum[seqNum].m_textInfo = std::pair<unsigned, unsigned>(stringId, pageSeqNum);
}

void libmspub::MSPUBCollector::setShapeImgIndex(unsigned seqNum, unsigned index)
{
  MSPUB_DEBUG_MSG(("Setting image index of shape with seqnum 0x%x to 0x%x\n", seqNum, index));
  m_shapeInfosBySeqNum[seqNum].m_imgIndex = index;
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

WPXPropertyList libmspub::MSPUBCollector::getParaStyleProps(const ParagraphStyle &style, unsigned defaultParaStyleIndex) const
{
  ParagraphStyle _nothing;
  const ParagraphStyle &defaultParaStyle = defaultParaStyleIndex < m_defaultParaStyles.size() ? m_defaultParaStyles[defaultParaStyleIndex] : _nothing;
  WPXPropertyList ret;
  Alignment align = style.align >= 0 ? style.align : defaultParaStyle.align;
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
  unsigned lineSpacing = style.lineSpacing;
  if (lineSpacing != LINE_SPACING_UNIT)
  {
    ret.insert("fo:line-height", (double)lineSpacing/LINE_SPACING_UNIT, WPX_PERCENT);
  }
  if (style.spaceAfterEmu != 0)
  {
    ret.insert("fo:margin-bottom", (double)style.spaceAfterEmu / EMUS_IN_INCH);
  }
  if (style.spaceBeforeEmu != 0)
  {
    ret.insert("fo:margin-top", (double)style.spaceBeforeEmu / EMUS_IN_INCH);
  }
  if (style.firstLineIndentEmu != 0)
  {
    ret.insert("fo:text-indent", (double)style.firstLineIndentEmu / EMUS_IN_INCH);
  }
  if (style.leftIndentEmu != 0)
  {
    ret.insert("fo:margin-left", (double)style.leftIndentEmu / EMUS_IN_INCH);
  }
  if (style.rightIndentEmu != 0)
  {
    ret.insert("fo:margin-right", (double)style.rightIndentEmu / EMUS_IN_INCH);
  }
  return ret;
}

WPXPropertyList libmspub::MSPUBCollector::getCharStyleProps(const CharacterStyle &style, unsigned defaultCharStyleIndex) const
{
  CharacterStyle _nothing = CharacterStyle(false, false, false);
  const CharacterStyle &defaultCharStyle = defaultCharStyleIndex < m_defaultCharStyles.size() ? m_defaultCharStyles[defaultCharStyleIndex] : _nothing;
  WPXPropertyList ret;
  if (style.italic)
  {
    ret.insert("fo:font-style", "italic");
  }
  if (style.bold)
  {
    ret.insert("fo:font-weight", "bold");
  }
  if (style.underline)
  {
    ret.insert("style:text-underline-type", "single");
  }
  if (style.textSizeInPt != -1)
  {
    ret.insert("fo:font-size", style.textSizeInPt);
  }
  else if (defaultCharStyle.textSizeInPt != -1)
  {
    ret.insert("fo:font-size", defaultCharStyle.textSizeInPt);
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
  if (style.fontIndex < m_fonts.size())
  {
    WPXString str;
    appendCharacters(str, m_fonts[style.fontIndex]);
    ret.insert("style:font-name", str);
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

bool libmspub::MSPUBCollector::go()
{
  if (m_paletteColors.size() < 8)
  {
    m_paletteColors.insert(m_paletteColors.begin(), Color());
  }
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
  for (std::map<unsigned, PageInfo>::const_iterator i = m_pagesBySeqNum.begin(); i != m_pagesBySeqNum.end(); ++i)
  {
    WPXPropertyList pageProps;
    if (m_widthSet)
    {
      pageProps.insert("svg:width", m_width);
    }
    if (m_heightSet)
    {
      pageProps.insert("svg:height", m_height);
    }
    const std::vector<ShapeGroupElement *> &shapeGroupsOrdered = i->second.m_shapeGroupsOrdered; // for readability
    if (!shapeGroupsOrdered.empty())
    {
      m_painter->startGraphics(pageProps);
      const unsigned *ptr_fillSeqNum = getIfExists(m_bgShapeSeqNumsByPageSeqNum, i->first);
      if (ptr_fillSeqNum)
      {
        boost::shared_ptr<const Fill> ptr_fill;
        const ShapeInfo *ptr_info = getIfExists(m_shapeInfosBySeqNum, *ptr_fillSeqNum);
        if (ptr_info)
        {
          ptr_fill = ptr_info->m_fill;
        }
        if (ptr_fill)
        {
          //REALLYFIXME
        }
      }
      for (unsigned i_group = 0; i_group < shapeGroupsOrdered.size(); ++i_group)
      {
        ShapeGroupElement *shapeGroup = shapeGroupsOrdered[i_group];
        shapeGroup->visit(boost::bind(&libmspub::MSPUBCollector::paintShape, this, _1, _2, _3, _4, _5, _6));
      }
      m_painter->endGraphics();
    }
  }
  return true;
}


bool libmspub::MSPUBCollector::addTextString(const std::vector<TextParagraph> &str, unsigned id)
{
  MSPUB_DEBUG_MSG(("addTextString, id: 0x%x\n", id));
  m_textStringsById[id] = str;
  return true; //FIXME: Warn if the string already existed in the map.
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
  MSPUB_DEBUG_MSG(("Image at index %d and of type 0x%x added.\n", index, type));
  while (m_images.size() < index)
  {
    m_images.push_back(std::pair<ImgType, WPXBinaryData>(UNKNOWN, WPXBinaryData()));
  }
  m_images[index - 1] = std::pair<ImgType, WPXBinaryData>(type, img);
  return true;
}

bool libmspub::MSPUBCollector::addShape(unsigned seqNum)
{
  m_possibleImageShapeSeqNums.push_back(seqNum);
  return true;
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

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
