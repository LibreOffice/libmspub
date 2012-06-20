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

#include "MSPUBCollector.h"
#include "libmspub_utils.h"
#include "MSPUBConstants.h"
#include "MSPUBTypes.h"
#include "PolygonUtils.h"

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), m_contentChunkReferences(), m_width(0), m_height(0),
  m_widthSet(false), m_heightSet(false),
  m_numPages(0), m_textStringsById(), m_pagesBySeqNum(),
  m_shapesBySeqNum(), m_images(),
  m_textColors(), m_fonts(),
  m_defaultCharStyles(), m_defaultParaStyles(), m_shapeTypesBySeqNum(),
  m_possibleImageShapeSeqNums(), m_shapeImgIndicesBySeqNum(),
  m_shapeCoordinatesBySeqNum(), m_shapeLineColorsBySeqNum(),
  m_shapeFillsBySeqNum(), m_paletteColors(), m_shapeSeqNumsOrdered(),
  m_pageSeqNumsByShapeSeqNum(), m_textInfoBySeqNum(), m_bgShapeSeqNumsByPageSeqNum(),
  m_skipIfNotBgSeqNums(), m_adjustValuesByIndexBySeqNum(),
  m_shapeRotationsBySeqNum(), m_shapeFlipsBySeqNum()
{
}

void libmspub::MSPUBCollector::setPageBgShape(unsigned pageSeqNum, unsigned seqNum)
{
  m_bgShapeSeqNumsByPageSeqNum[pageSeqNum] = seqNum;
}

void libmspub::MSPUBCollector::setShapeOrder(unsigned seqNum)
{
  m_shapeSeqNumsOrdered.push_back(seqNum);
}

void libmspub::MSPUBCollector::addPaletteColor(Color c)
{
  m_paletteColors.push_back(c);
}

WPXPropertyListVector libmspub::Shape::updateGraphicsProps()
{
  return WPXPropertyListVector();
}

void libmspub::Shape::output(libwpg::WPGPaintInterface *painter, Coordinate coord)
{
  WPXPropertyListVector graphicsPropsVector = updateGraphicsProps();
  owner->m_painter->setStyle(graphicsProps, graphicsPropsVector);
  setCoordProps(coord);
  write(painter);
}

bool libmspub::GeometricShape::hasFill()
{
  return (graphicsProps["draw:fill"]) ? (graphicsProps["draw:fill"]->getStr() != "none") : false;
}

bool libmspub::ImgShape::hasFill()
{
  return true;
}

void libmspub::GeometricShape::output(libwpg::WPGPaintInterface *painter, Coordinate coord)
{
  WPXPropertyListVector graphicsPropsVector = updateGraphicsProps();
  WPXString stroke = graphicsProps["draw:stroke"] ? graphicsProps["draw:stroke"]->getStr() : "none";
  WPXString fill = graphicsProps["draw:fill"] ? graphicsProps["draw:fill"]->getStr() : "none";
  bool hasFill_ = hasFill();
  if (stroke != "none" && hasFill_)
    owner->m_painter->startLayer(WPXPropertyList());
  if (hasFill())
  {
    graphicsProps.insert("draw:stroke", "none");
    owner->m_painter->setStyle(graphicsProps, graphicsPropsVector);
    setCoordProps(coord);
    m_closeEverything = true;
    write(painter);
  }
  if (stroke != "none")
  {
    graphicsProps.insert("draw:stroke", stroke);
    graphicsProps.insert("draw:fill", "none");
    m_closeEverything = false;
    setCoordProps(coord);
    owner->m_painter->setStyle(graphicsProps, graphicsPropsVector);
    write(painter);
  }
  if (stroke != "none" && hasFill_)
    owner->m_painter->endLayer();
}

void libmspub::Shape::setCoordProps(Coordinate coord)
{
  owner->setRectCoordProps(coord, &props);
}

void libmspub::TextShape::write(libwpg::WPGPaintInterface *painter)
{
  painter->startTextObject(props, WPXPropertyListVector());
  for (unsigned i_lines = 0; i_lines < str.size(); ++i_lines)
  {
    WPXPropertyList paraProps = owner->getParaStyleProps(str[i_lines].style, str[i_lines].style.defaultCharStyleIndex);
    painter->startTextLine(paraProps);
    for (unsigned i_spans = 0; i_spans < str[i_lines].spans.size(); ++i_spans)
    {
      WPXString text;
      appendCharacters(text, str[i_lines].spans[i_spans].chars);
      WPXPropertyList charProps = owner->getCharStyleProps(str[i_lines].spans[i_spans].style, str[i_lines].style.defaultCharStyleIndex);
      painter->startTextSpan(charProps);
      painter->insertText(text);
      painter->endTextSpan();
    }
    painter->endTextLine();
  }
  painter->endTextObject();
}

void libmspub::GeometricShape::setClockwiseRotation(short rotation)
{
  m_clockwiseRotation = rotation;
}

double libmspub::GeometricShape::getSpecialValue(const CustomShape &shape, int arg) const
{
  if (PROP_ADJUST_VAL_FIRST <= arg && PROP_ADJUST_VAL_LAST >= arg)
  {
    unsigned adjustIndex = arg - PROP_ADJUST_VAL_FIRST;
    if (adjustIndex < shape.m_numDefaultAdjustValues)
    {
      if ((shape.m_adjustShiftMask >> adjustIndex) & 0x1)
      {
        return m_adjustValues[adjustIndex] >> 16;
      }
      return m_adjustValues[adjustIndex];
    }
    return 0;
  }
  if (arg & OTHER_CALC_VAL)
  {
    return getCalculationValue(arg & 0xff, true);
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

double libmspub::GeometricShape::getCalculationValue(unsigned index, bool recursiveEntry) const
{
  const CustomShape *p_shape = getCustomShape(m_type);
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
    m_valuesSeen.clear();
    m_valuesSeen.resize(shape.m_numCalculations);
  }
  if (m_valuesSeen[index])
  {
    //recursion detected
    return 0;
  }
  m_valuesSeen[index] = true;

  const Calculation &c = shape.mp_calculations[index];
  bool oneSpecial = (c.m_flags & 0x2000) != 0;
  bool twoSpecial = (c.m_flags & 0x4000) != 0;
  bool threeSpecial = (c.m_flags & 0x8000) != 0;

  double valOne = oneSpecial ? getSpecialValue(shape, c.m_argOne) : c.m_argOne;
  double valTwo = twoSpecial ? getSpecialValue(shape, c.m_argTwo) : c.m_argTwo;
  double valThree = threeSpecial ? getSpecialValue(shape, c.m_argThree) : c.m_argThree;
  m_valuesSeen[index] = false;
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
    return atan2(valTwo, valOne) / (PI / 180);
  case 9:
    return valOne * sin(valTwo * (PI / 180) );
  case 10:
    return valOne * cos(valTwo * (PI / 180) );
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
    return (cos(valThree * (PI / 180)) * (valOne - 10800) + sin(valThree * (PI / 180)) * (valTwo - 10800)) + 10800;
  case 0x82:
    return -(sin(valThree * (PI / 180)) * (valOne - 10800) - cos(valThree * (PI / 180)) * (valTwo - 10800)) + 10800;
  default:
    return 0;
  }
}

void libmspub::GeometricShape::fillDefaultAdjustValues()
{
  if (m_filledDefaultAdjustValues)
  {
    return;
  }
  m_filledDefaultAdjustValues = true;
  const CustomShape *def = getCustomShape(m_type);
  if (def)
  {
    for (unsigned i = 0; i < def->m_numDefaultAdjustValues; ++i)
    {
      m_adjustValues.push_back(def->mp_defaultAdjustValues[i]);
    }
  }
}

void libmspub::GeometricShape::setAdjustValue(unsigned index, int adjustValue)
{
  for (unsigned i = m_adjustValues.size(); i <= index; ++i)
  {
    m_adjustValues.push_back(0);
  }
  m_adjustValues[index] = adjustValue;
}

void libmspub::GeometricShape::setText(std::vector<TextParagraph> str)
{
  m_str = str;
  m_hasText = true;
}

void libmspub::GeometricShape::setCoordProps(Coordinate coord)
{
  m_x = owner->m_width / 2 + (double)(coord.m_xs) / EMUS_IN_INCH;
  m_y = owner->m_height / 2 + (double)(coord.m_ys) / EMUS_IN_INCH;
  m_width = (double)(coord.m_xe - coord.m_xs) / EMUS_IN_INCH;
  m_height = (double)(coord.m_ye - coord.m_ys) / EMUS_IN_INCH;
  const CustomShape *p_shape = getCustomShape(m_type);
  if (p_shape)
  {
    m_textCoord = p_shape->getTextRectangle(coord.m_xs, coord.m_ys, coord.m_xe - coord.m_xs, coord.m_ye - coord.m_ys, this);
  }
  else
  {
    m_textCoord = coord;
  }
}

WPXPropertyListVector libmspub::FillableShape::updateGraphicsProps()
{
  if (m_fill)
  {
    return m_fill->getProperties(&graphicsProps);
  }
  else
  {
    graphicsProps.insert("draw:fill", "none");
  }
  return WPXPropertyListVector();
}

WPXPropertyListVector libmspub::TextShape::updateGraphicsProps()
{
  if (m_fill)
  {
    return m_fill->getProperties(&props);
  }
  return WPXPropertyListVector();
}

WPXPropertyListVector libmspub::GeometricShape::updateGraphicsProps()
{
  if (m_lineSet)
  {
    graphicsProps.insert("draw:stroke", "solid");
    graphicsProps.insert("svg:stroke-color", libmspub::MSPUBCollector::getColorString(m_line.getFinalColor(owner->m_paletteColors)));
  }
  else
  {
    graphicsProps.insert("draw:stroke", "none");
  }
  return FillableShape::updateGraphicsProps();
}

void libmspub::GeometricShape::write(libwpg::WPGPaintInterface *painter)
{
  // If a shape includes text, and if the font from the PUB file
  // is not available in LibreOffice (unfortunately we haven't gotten embedded fonts working yet...)
  // then the bounding box of the text might be different from the size of the shape.
  // So which size should be used? We should compromise by using the size of the text for rectangular shapes,
  // and the size of the shape for other shapes. However currently the size of the shape is used in all cases,
  // causing ugliness in some documents.
  const CustomShape *shape = getCustomShape(m_type);
  if (shape)
  {
    writeCustomShape(shape, props, painter, m_x, m_y, m_height, m_width, this, m_closeEverything, m_clockwiseRotation, m_flipV, m_flipH);
  }
  if (m_hasText && !m_closeEverything) // only fill in text on the second pass
  {
    owner->setRectCoordProps(m_textCoord, &props);
    painter->startTextObject(props, WPXPropertyListVector());
    for (unsigned i_lines = 0; i_lines < m_str.size(); ++i_lines)
    {
      WPXPropertyList paraProps = owner->getParaStyleProps(m_str[i_lines].style, m_str[i_lines].style.defaultCharStyleIndex);
      painter->startTextLine(paraProps);
      for (unsigned i_spans = 0; i_spans < m_str[i_lines].spans.size(); ++i_spans)
      {
        WPXString text;
        appendCharacters(text, m_str[i_lines].spans[i_spans].chars);
        WPXPropertyList charProps = owner->getCharStyleProps(m_str[i_lines].spans[i_spans].style, m_str[i_lines].style.defaultCharStyleIndex);
        painter->startTextSpan(charProps);
        painter->insertText(text);
        painter->endTextSpan();
      }
      painter->endTextLine();
    }
    painter->endTextObject();
  }
}

void libmspub::FillableShape::setFill(Fill *f)
{
  m_fill = f;
}

void libmspub::GeometricShape::setLine(ColorReference line)
{
  m_line = line;
  m_lineSet = true;
}

libmspub::ImgShape::ImgShape(const GeometricShape &from, ImgType imgType, WPXBinaryData i, MSPUBCollector *o) :
  GeometricShape(from.m_pageSeqNum, o), img(i)
{
  this->m_type = from.m_type;
  this->props = from.props;
  setMime_(imgType);
}

const char *libmspub::ImgShape::mimeByImgType(ImgType type)
{
  switch (type)
  {
  case PNG:
    return "image/png";
  case JPEG:
    return "image/jpeg";
  case DIB:
    return "image/bmp";
  case PICT:
    return "image/pict";
  case WMF:
    return "image/wmf";
  case EMF:
    return "image/emf";
  case TIFF:
    return "image/tiff";
  default:
    MSPUB_DEBUG_MSG(("Unknown image type %d passed to mimeByImgType!\n", type));
    return 0;
  }
}

void libmspub::ImgShape::setMime_(ImgType imgType)
{
  const char *mimetype = mimeByImgType(imgType);
  if (mimetype)
    props.insert("libwpg:mime-type", mimetype);
}
void libmspub::ImgShape::write(libwpg::WPGPaintInterface *painter)
{
  props.insert("svg:x", m_x);
  props.insert("svg:y", m_y);
  props.insert("svg:width", m_width);
  props.insert("svg:height", m_height);
  painter->drawGraphicObject(props, img);
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

bool libmspub::MSPUBCollector::setShapeRotation(unsigned seqNum, short rotation)
{
  return m_shapeRotationsBySeqNum.insert(std::pair<const unsigned, short>(seqNum, rotation)).second;
}

bool libmspub::MSPUBCollector::setShapeFlip(unsigned seqNum, bool flipVertical, bool flipHorizontal)
{
  return m_shapeFlipsBySeqNum.insert(std::pair<const unsigned, std::pair<bool, bool> >(seqNum, std::pair<bool, bool>(flipVertical, flipHorizontal))).second;
}

bool libmspub::MSPUBCollector::setShapeType(unsigned seqNum, ShapeType type)
{
  return m_shapeTypesBySeqNum.insert(std::pair<const unsigned, ShapeType>(seqNum, type)).second;
}

bool libmspub::MSPUBCollector::setAdjustValue(unsigned seqNum, unsigned index, int adjust)
{
  return m_adjustValuesByIndexBySeqNum[seqNum].insert(std::pair<const unsigned, int>(index, adjust)).second;
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

bool libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum)
{
  return m_textInfoBySeqNum.insert(std::pair<unsigned, std::pair<unsigned,unsigned> >(
                                     seqNum, std::pair<unsigned, unsigned>(stringId, pageSeqNum))).second;
}

bool libmspub::MSPUBCollector::setShapeImgIndex(unsigned seqNum, unsigned index)
{
  MSPUB_DEBUG_MSG(("Setting image index of shape with seqnum 0x%x to 0x%x\n", seqNum, index));
  return m_shapeImgIndicesBySeqNum.insert(std::pair<const unsigned, unsigned>(seqNum, index)).second;
}

bool libmspub::MSPUBCollector::setShapeLineColor(unsigned seqNum, ColorReference line)
{
  return m_shapeLineColorsBySeqNum.insert(std::pair<const unsigned, ColorReference>(
      seqNum, line)).second;
}

bool libmspub::MSPUBCollector::setShapeFill(unsigned seqNum, Fill *fill, bool skipIfNotBg)
{
  m_shapeFillsBySeqNum.insert(seqNum, fill);
  if (skipIfNotBg)
  {
    m_skipIfNotBgSeqNums.insert(seqNum);
  }
  return true;
}

void libmspub::MSPUBCollector::setRectCoordProps(Coordinate coord, WPXPropertyList *props)
{
  int xs = coord.m_xs, ys = coord.m_ys, xe = coord.m_xe, ye = coord.m_ye;
  double x_center = m_width / 2;
  double y_center = m_height / 2;
  props->insert("svg:x", x_center + (double)xs / EMUS_IN_INCH);
  props->insert("svg:y", y_center + (double)ys / EMUS_IN_INCH);
  props->insert("svg:width", (double)(xe - xs) / EMUS_IN_INCH);
  props->insert("svg:height", (double)(ye - ys) / EMUS_IN_INCH);
}

bool libmspub::MSPUBCollector::setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye)
{
  return m_shapeCoordinatesBySeqNum.insert(std::pair<const unsigned, Coordinate>(seqNum, Coordinate(xs, ys, xe, ye))).second;
}

void libmspub::MSPUBCollector::addFont(std::vector<unsigned char> name)
{
  m_fonts.push_back(name);
}

void libmspub::MSPUBCollector::assignImages()
{
  for (unsigned i = 0; i < m_possibleImageShapeSeqNums.size(); ++i)
  {
    unsigned seqNum = m_possibleImageShapeSeqNums[i];
    unsigned *index = getIfExists(m_shapeImgIndicesBySeqNum, seqNum);
    GeometricShape *shape = (GeometricShape *)ptr_getIfExists(m_shapesBySeqNum, seqNum);
    if (!shape)
    {
      MSPUB_DEBUG_MSG(("Could not find shape of seqnum 0x%x in assignImages\n", seqNum));
      return;
    }
    short *ptr_rotation = getIfExists(m_shapeRotationsBySeqNum, seqNum);
    if (ptr_rotation)
    {
      shape->setClockwiseRotation(*ptr_rotation);
    }
    std::pair<bool, bool> *ptr_flips = getIfExists(m_shapeFlipsBySeqNum, seqNum);
    if (ptr_flips)
    {
      shape->m_flipV = ptr_flips->first;
      shape->m_flipH = ptr_flips->second;
    }
    if (index && *index - 1 < m_images.size())
    {
      ImgShape *toInsert = new ImgShape(*shape, m_images[*index - 1].first, m_images[*index - 1].second, this);
      m_shapesBySeqNum.erase(seqNum);
      m_shapesBySeqNum.insert(seqNum, toInsert);
    }
    else
    {
      ShapeType *type = getIfExists(m_shapeTypesBySeqNum, seqNum);
      if (type)
      {
        shape->m_type = *type;
      }
      else
      {
        MSPUB_DEBUG_MSG(("Could not find shape type for shape of seqnum 0x%x\n", m_possibleImageShapeSeqNums[i]));
      }
      std::pair<unsigned, unsigned> *ptr_textInfo = getIfExists(m_textInfoBySeqNum, seqNum);
      if (ptr_textInfo)
      {
        unsigned stringId = ptr_textInfo->first;
        std::vector<TextParagraph> *ptr_str = getIfExists(m_textStringsById, stringId);
        if (ptr_str)
        {
          shape->setText(*ptr_str);
        }
      }
      shape->fillDefaultAdjustValues();
      ColorReference *ptr_lineColor = getIfExists(m_shapeLineColorsBySeqNum, seqNum);
      if (ptr_lineColor)
      {
        shape->setLine(*ptr_lineColor);
      }
      Fill *ptr_fill = ptr_getIfExists(m_shapeFillsBySeqNum, seqNum);
      if (ptr_fill && m_skipIfNotBgSeqNums.find(seqNum) == m_skipIfNotBgSeqNums.end())
      {
        shape->setFill(ptr_fill);
      }
      for (std::map<unsigned, int>::const_iterator iter= m_adjustValuesByIndexBySeqNum[seqNum].begin();
           iter != m_adjustValuesByIndexBySeqNum[seqNum].end(); ++iter)
      {
        shape->setAdjustValue(iter->first, iter->second);
      }
    }
  }
}

WPXPropertyList libmspub::MSPUBCollector::getParaStyleProps(const ParagraphStyle &style, unsigned defaultParaStyleIndex)
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
  return ret;
}

WPXPropertyList libmspub::MSPUBCollector::getCharStyleProps(const CharacterStyle &style, unsigned defaultCharStyleIndex)
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
  assignImages();
  // order the shapes in each page
  for (unsigned i = 0; i < m_shapeSeqNumsOrdered.size(); ++i)
  {
    unsigned *ptr_pageSeqNum = getIfExists(m_pageSeqNumsByShapeSeqNum, m_shapeSeqNumsOrdered[i]);
    if (ptr_pageSeqNum)
    {
      PageInfo *ptr_page = getIfExists(m_pagesBySeqNum, *ptr_pageSeqNum);
      if (ptr_page)
      {
        ptr_page->m_shapeSeqNumsOrdered.push_back(m_shapeSeqNumsOrdered[i]);
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
    const std::vector<unsigned> &shapeSeqNumsOrdered = i->second.m_shapeSeqNumsOrdered; // for readability
    if (!shapeSeqNumsOrdered.empty())
    {
      m_painter->startGraphics(pageProps);
      unsigned *ptr_fillSeqNum = getIfExists(m_bgShapeSeqNumsByPageSeqNum, i->first);
      if (ptr_fillSeqNum)
      {
        Fill *ptr_fill = ptr_getIfExists(m_shapeFillsBySeqNum, *ptr_fillSeqNum);
        if (ptr_fill)
        {
          GeometricShape bg(i->first, this);
          bg.setFill(ptr_fill);
          Coordinate wholePage(-m_width/2 * EMUS_IN_INCH, -m_height/2 * EMUS_IN_INCH, m_width/2 * EMUS_IN_INCH, m_height/2 * EMUS_IN_INCH);
          bg.output(m_painter, wholePage);
        }
      }
      for (unsigned i_seqNums = 0; i_seqNums < shapeSeqNumsOrdered.size(); ++i_seqNums)
      {
        Shape *shape = ptr_getIfExists(m_shapesBySeqNum, shapeSeqNumsOrdered[i_seqNums]);
        if (shape)
        {
          Coordinate *coord = getIfExists(m_shapeCoordinatesBySeqNum, shapeSeqNumsOrdered[i_seqNums]);
          if (coord)
          {
            shape->output(m_painter, *coord);
          }
          else
          {
            MSPUB_DEBUG_MSG(("Could not output shape of seqnum 0x%x: no coordinates provided\n", shapeSeqNumsOrdered[i_seqNums]));
          }
        }
        else
        {
          MSPUB_DEBUG_MSG(("No shape with seqnum 0x%x found in collector\n", shapeSeqNumsOrdered[i_seqNums]));
        }
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

bool libmspub::MSPUBCollector::addShape(unsigned seqNum, unsigned pageSeqNum)
{
  m_shapesBySeqNum.insert(seqNum, new GeometricShape(pageSeqNum, this));
  m_possibleImageShapeSeqNums.push_back(seqNum);
  PageInfo *page = getIfExists(m_pagesBySeqNum, pageSeqNum);
  if (page)
  {
    page->m_shapeSeqNums.push_back(seqNum);
    m_pageSeqNumsByShapeSeqNum.insert(std::pair<unsigned, unsigned>(seqNum, pageSeqNum));
  }
  return true;
}

void libmspub::MSPUBCollector::addTextColor(ColorReference c)
{
  m_textColors.push_back(c);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
