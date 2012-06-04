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

libmspub::MSPUBCollector::MSPUBCollector(libwpg::WPGPaintInterface *painter) :
  m_painter(painter), m_contentChunkReferences(), m_width(0), m_height(0),
  m_widthSet(false), m_heightSet(false),
  m_numPages(0), m_textStringsById(), m_pagesBySeqNum(),
  m_shapesBySeqNum(), m_images(),
  m_textColors(), m_defaultColor(0, 0, 0), m_fonts(),
  m_defaultCharStyles(), m_defaultParaStyles(), m_shapeTypesBySeqNum(),
  m_possibleImageShapeSeqNums(), m_shapeImgIndicesBySeqNum(),
  m_shapeCoordinatesBySeqNum(), m_shapeLineColorsBySeqNum(),
  m_shapeFillsBySeqNum(), m_paletteColors(), m_shapeSeqNumsOrdered(),
  m_pageSeqNumsByShapeSeqNum(), m_textInfoBySeqNum(), m_bgShapeSeqNumsByPageSeqNum(),
  m_skipIfNotBgSeqNums()
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

WPXPropertyListVector libmspub::MSPUBCollector::Shape::updateGraphicsProps()
{
  return WPXPropertyListVector();
}

void libmspub::MSPUBCollector::Shape::output(libwpg::WPGPaintInterface *painter, Coordinate coord)
{
  WPXPropertyListVector graphicsPropsVector = updateGraphicsProps();
  owner->m_painter->setStyle(graphicsProps, graphicsPropsVector);
  setCoordProps(coord);
  write(painter);
}

void libmspub::MSPUBCollector::Shape::setCoordProps(Coordinate coord)
{
  owner->setRectCoordProps(coord, &props);
}

void libmspub::MSPUBCollector::TextShape::write(libwpg::WPGPaintInterface *painter)
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


void libmspub::MSPUBCollector::GeometricShape::setCoordProps(Coordinate coord)
{
  switch (m_type)
  {
  case ELLIPSE:
    owner->setEllipseCoordProps(coord, &props);
    break;
  case RECTANGLE:
  default:
    owner->setRectCoordProps(coord, &props);
    break;
  };
}

WPXPropertyListVector libmspub::MSPUBCollector::FillableShape::updateGraphicsProps()
{
  if (fill)
  {
    return fill->getProperties(&graphicsProps);
  }
  return WPXPropertyListVector();
}

WPXPropertyListVector libmspub::MSPUBCollector::TextShape::updateGraphicsProps()
{
  if (fill)
  {
    return fill->getProperties(&props);
  }
  return WPXPropertyListVector();
}

WPXPropertyListVector libmspub::MSPUBCollector::GeometricShape::updateGraphicsProps()
{
  if (m_lineSet)
  {
    graphicsProps.insert("draw:stroke", "solid");
    graphicsProps.insert("svg:stroke-color", getColorString(m_line.getFinalColor(owner->m_paletteColors)));
  }
  else
  {
    graphicsProps.insert("draw:stroke", "none");
  }
  return FillableShape::updateGraphicsProps();
}

void libmspub::MSPUBCollector::GeometricShape::write(libwpg::WPGPaintInterface *painter)
{
  switch(m_type)
  {
  case RECTANGLE:
    painter->drawRectangle(props);
    break;
  case ELLIPSE:
    painter->drawEllipse(props);
    break;
  default:
    break;
  }
}

void libmspub::MSPUBCollector::FillableShape::setFill(Fill *f)
{
  fill = f;
}

void libmspub::MSPUBCollector::GeometricShape::setLine(ColorReference line)
{
  m_line = line;
  m_lineSet = true;
}

libmspub::MSPUBCollector::ImgShape::ImgShape(const GeometricShape &from, ImgType imgType, WPXBinaryData i, MSPUBCollector *o) :
  GeometricShape(from.m_pageSeqNum, o), img(i)
{
  this->m_type = from.m_type;
  this->props = from.props;
  setMime_(imgType);
}

const char *libmspub::MSPUBCollector::ImgShape::mimeByImgType(ImgType type)
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
    return 0;
    MSPUB_DEBUG_MSG(("Unknown image type %d passed to mimeByImgType!\n", type));
  }
}

void libmspub::MSPUBCollector::ImgShape::setMime_(ImgType imgType)
{
  const char *mimetype = mimeByImgType(imgType);
  if (mimetype)
    props.insert("libwpg:mime-type", mimetype);
}
void libmspub::MSPUBCollector::ImgShape::write(libwpg::WPGPaintInterface *painter)
{
  painter->drawGraphicObject(props, img);
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

bool libmspub::MSPUBCollector::setShapeType(unsigned seqNum, ShapeType type)
{
  return m_shapeTypesBySeqNum.insert(std::pair<const unsigned, ShapeType>(seqNum, type)).second;
}

void libmspub::MSPUBCollector::setDefaultColor(unsigned char r, unsigned char g, unsigned char b)
{
  m_defaultColor = Color(r, g, b);
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

void libmspub::MSPUBCollector::assignTextShapes()
{
  for (std::map<unsigned, std::pair<unsigned, unsigned> >::const_iterator i = m_textInfoBySeqNum.begin();
       i != m_textInfoBySeqNum.end(); ++i)
  {
    unsigned pageSeqNum = i->second.second;
    unsigned stringId = i->second.first;
    unsigned seqNum = i->first;
    PageInfo *ptr_page = getIfExists(m_pagesBySeqNum, pageSeqNum);
    if (!ptr_page)
    {
      MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in assignTextShapes!\n", pageSeqNum));
      continue;
    }
    std::vector<TextParagraph> *ptr_str = getIfExists(m_textStringsById, stringId);
    if (! ptr_str)
    {
      MSPUB_DEBUG_MSG(("Text string of id 0x%x not found in assignTextShape!\n", stringId));
      continue;
    }
    if (ptr_getIfExists(m_shapesBySeqNum, seqNum))
    {
      MSPUB_DEBUG_MSG(("already tried to add the text shape of seqnum 0x%x to this page!\n", seqNum));
      continue;
    }
    TextShape *ptr_shape = new TextShape(*ptr_str, this);
    Fill *ptr_fill = ptr_getIfExists(m_shapeFillsBySeqNum, seqNum);
    if (ptr_fill && m_skipIfNotBgSeqNums.find(seqNum) == m_skipIfNotBgSeqNums.end())
    {
      ptr_shape->setFill(ptr_fill);
    }
    m_shapesBySeqNum.insert(seqNum, ptr_shape);
    ptr_page->m_shapeSeqNums.push_back(seqNum);
    m_pageSeqNumsByShapeSeqNum.insert(std::pair<unsigned, unsigned>(seqNum, pageSeqNum));
  }
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

void libmspub::MSPUBCollector::setEllipseCoordProps(Coordinate coord, WPXPropertyList *props)
{
  int xs = coord.m_xs, ys = coord.m_ys, xe = coord.m_xe, ye = coord.m_ye;
  double x_center = m_width / 2;
  double y_center = m_height / 2;
  props->insert("svg:cx", x_center + ((double)xs + (double)xe)/(2 * EMUS_IN_INCH));
  props->insert("svg:cy", y_center + ((double)ys + (double)ye)/(2 * EMUS_IN_INCH));
  props->insert("svg:rx", (double)(xe - xs)/(2 * EMUS_IN_INCH));
  props->insert("svg:ry", (double)(ye - ys)/(2 * EMUS_IN_INCH));
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
    unsigned *index = getIfExists(m_shapeImgIndicesBySeqNum, m_possibleImageShapeSeqNums[i]);
    GeometricShape *shape = (GeometricShape *)ptr_getIfExists(m_shapesBySeqNum, m_possibleImageShapeSeqNums[i]);
    if (!shape)
    {
      MSPUB_DEBUG_MSG(("Could not find shape of seqnum 0x%x in assignImages\n", m_possibleImageShapeSeqNums[i]));
      return;
    }
    if (index && *index - 1 < m_images.size())
    {
      ImgShape *toInsert = new ImgShape(*shape, m_images[*index - 1].first, m_images[*index - 1].second, this);
      m_shapesBySeqNum.erase(m_possibleImageShapeSeqNums[i]);
      m_shapesBySeqNum.insert(m_possibleImageShapeSeqNums[i], toInsert);
    }
    else
    {
      ShapeType *type = getIfExists(m_shapeTypesBySeqNum, m_possibleImageShapeSeqNums[i]);
      if (type)
      {
        shape->m_type = *type;
      }
      else
      {
        MSPUB_DEBUG_MSG(("Could not find shape type for shape of seqnum 0x%x\n", m_possibleImageShapeSeqNums[i]));
      }
      ColorReference *ptr_lineColor = getIfExists(m_shapeLineColorsBySeqNum, m_possibleImageShapeSeqNums[i]);
      if (ptr_lineColor)
      {
        shape->setLine(*ptr_lineColor);
      }
      Fill *ptr_fill = ptr_getIfExists(m_shapeFillsBySeqNum, m_possibleImageShapeSeqNums[i]);
      if (ptr_fill && m_skipIfNotBgSeqNums.find(m_possibleImageShapeSeqNums[i]) == m_skipIfNotBgSeqNums.end())
      {
        shape->setFill(ptr_fill);
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
    ret.insert("fo:color", getColorString(m_defaultColor));
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
  assignTextShapes();
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
    if (shapeSeqNumsOrdered.size() > 0)
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
