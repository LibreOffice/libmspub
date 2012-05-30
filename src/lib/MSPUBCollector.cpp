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
  m_painter(painter), contentChunkReferences(), m_width(0), m_height(0),
  m_widthSet(false), m_heightSet(false),
  m_numPages(0), textStringsById(), pagesBySeqNum(),
  shapesBySeqNum(), images(),
  textColors(), defaultColor(0, 0, 0), fonts(),
  defaultCharStyles(), defaultParaStyles(), shapeTypesBySeqNum(),
  possibleImageShapeSeqNums(), shapeImgIndicesBySeqNum(),
  shapeCoordinatesBySeqNum(), shapeLineColorsBySeqNum(),
  shapeFillsBySeqNum(), paletteColors(), shapeSeqNumsOrdered(),
  pageSeqNumsByShapeSeqNum(), textInfoBySeqNum(), bgShapeSeqNumsByPageSeqNum(),
  skipIfNotBgSeqNums()
{
}

void libmspub::MSPUBCollector::setPageBgShape(unsigned pageSeqNum, unsigned seqNum)
{
  bgShapeSeqNumsByPageSeqNum[pageSeqNum] = seqNum;
}

void libmspub::MSPUBCollector::setShapeOrder(unsigned seqNum)
{
  shapeSeqNumsOrdered.push_back(seqNum);
}

void libmspub::MSPUBCollector::addPaletteColor(Color c)
{
  paletteColors.push_back(c);
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
  switch (type)
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
  if (lineSet)
  {
    graphicsProps.insert("draw:stroke", "solid");
    graphicsProps.insert("svg:stroke-color", getColorString(owner->getColorByReference(line)));
  }
  return FillableShape::updateGraphicsProps();
}

void libmspub::MSPUBCollector::GeometricShape::write(libwpg::WPGPaintInterface *painter)
{
  switch(type)
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

void libmspub::MSPUBCollector::GeometricShape::setLine(unsigned l)
{
  line = l;
  lineSet = true;
}

libmspub::MSPUBCollector::ImgShape::ImgShape(const GeometricShape &from, ImgType imgType, WPXBinaryData i, MSPUBCollector *o) :
  GeometricShape(from.pageSeqNum, o), img(i)
{
  this->type = from.type;
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
  return shapeTypesBySeqNum.insert(std::pair<const unsigned, ShapeType>(seqNum, type)).second;
}

void libmspub::MSPUBCollector::setDefaultColor(unsigned char r, unsigned char g, unsigned char b)
{
  defaultColor = Color(r, g, b);
}

void libmspub::MSPUBCollector::addDefaultCharacterStyle(const CharacterStyle &st)
{
  defaultCharStyles.push_back(st);
}

void libmspub::MSPUBCollector::addDefaultParagraphStyle(const ParagraphStyle &st)
{
  defaultParaStyles.push_back(st);
}

bool libmspub::MSPUBCollector::addPage(unsigned seqNum)
{
  if (! (m_widthSet && m_heightSet) )
  {
    return false;
  }
  MSPUB_DEBUG_MSG(("Adding page of seqnum 0x%x\n", seqNum));
  pagesBySeqNum[seqNum] = PageInfo();
  return true;
}

void libmspub::MSPUBCollector::assignTextShapes()
{
  for (std::map<unsigned, std::pair<unsigned, unsigned> >::const_iterator i = textInfoBySeqNum.begin();
       i != textInfoBySeqNum.end(); ++i)
  {
    unsigned pageSeqNum = i->second.second;
    unsigned stringId = i->second.first;
    unsigned seqNum = i->first;
    PageInfo *ptr_page = getIfExists(pagesBySeqNum, pageSeqNum);
    if (!ptr_page)
    {
      MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in assignTextShapes!\n", pageSeqNum));
      continue;
    }
    std::vector<TextParagraph> *ptr_str = getIfExists(textStringsById, stringId);
    if (! ptr_str)
    {
      MSPUB_DEBUG_MSG(("Text string of id 0x%x not found in assignTextShape!\n", stringId));
      continue;
    }
    if (ptr_getIfExists(shapesBySeqNum, seqNum))
    {
      MSPUB_DEBUG_MSG(("already tried to add the text shape of seqnum 0x%x to this page!\n", seqNum));
      continue;
    }
    TextShape *ptr_shape = new TextShape(*ptr_str, this);
    Fill *ptr_fill = ptr_getIfExists(shapeFillsBySeqNum, seqNum);
    if (ptr_fill && skipIfNotBgSeqNums.find(seqNum) == skipIfNotBgSeqNums.end())
    {
      ptr_shape->setFill(ptr_fill);
    }
    shapesBySeqNum.insert(seqNum, ptr_shape);
    ptr_page->shapeSeqNums.push_back(seqNum);
    pageSeqNumsByShapeSeqNum.insert(std::pair<unsigned, unsigned>(seqNum, pageSeqNum));
  }
}

bool libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum)
{
  return textInfoBySeqNum.insert(std::pair<unsigned, std::pair<unsigned,unsigned> >(
                                   seqNum, std::pair<unsigned, unsigned>(stringId, pageSeqNum))).second;
}

bool libmspub::MSPUBCollector::setShapeImgIndex(unsigned seqNum, unsigned index)
{
  MSPUB_DEBUG_MSG(("Setting image index of shape with seqnum 0x%x to 0x%x\n", seqNum, index));
  return shapeImgIndicesBySeqNum.insert(std::pair<const unsigned, unsigned>(seqNum, index)).second;
}

bool libmspub::MSPUBCollector::setShapeLineColor(unsigned seqNum, unsigned line)
{
  return shapeLineColorsBySeqNum.insert(std::pair<const unsigned, unsigned>(
                                          seqNum, line)).second;
}

bool libmspub::MSPUBCollector::setShapeFill(unsigned seqNum, Fill *fill, bool skipIfNotBg)
{
  shapeFillsBySeqNum.insert(seqNum, fill);
  if (skipIfNotBg)
  {
    skipIfNotBgSeqNums.insert(seqNum);
  }
  return true;
}

void libmspub::MSPUBCollector::setRectCoordProps(Coordinate coord, WPXPropertyList *props)
{
  int xs = coord.xs, ys = coord.ys, xe = coord.xe, ye = coord.ye;
  double x_center = m_width / 2;
  double y_center = m_height / 2;
  props->insert("svg:x", x_center + (double)xs / EMUS_IN_INCH);
  props->insert("svg:y", y_center + (double)ys / EMUS_IN_INCH);
  props->insert("svg:width", (double)(xe - xs) / EMUS_IN_INCH);
  props->insert("svg:height", (double)(ye - ys) / EMUS_IN_INCH);
}

void libmspub::MSPUBCollector::setEllipseCoordProps(Coordinate coord, WPXPropertyList *props)
{
  int xs = coord.xs, ys = coord.ys, xe = coord.xe, ye = coord.ye;
  double x_center = m_width / 2;
  double y_center = m_height / 2;
  props->insert("svg:cx", x_center + ((double)xs + (double)xe)/(2 * EMUS_IN_INCH));
  props->insert("svg:cy", y_center + ((double)ys + (double)ye)/(2 * EMUS_IN_INCH));
  props->insert("svg:rx", (double)(xe - xs)/(2 * EMUS_IN_INCH));
  props->insert("svg:ry", (double)(ye - ys)/(2 * EMUS_IN_INCH));
}

bool libmspub::MSPUBCollector::setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye)
{
  return shapeCoordinatesBySeqNum.insert(std::pair<const unsigned, Coordinate>(seqNum, Coordinate(xs, ys, xe, ye))).second;
}

void libmspub::MSPUBCollector::addFont(std::vector<unsigned char> name)
{
  fonts.push_back(name);
}

void libmspub::MSPUBCollector::assignImages()
{
  for (unsigned i = 0; i < possibleImageShapeSeqNums.size(); ++i)
  {
    unsigned *index = getIfExists(shapeImgIndicesBySeqNum, possibleImageShapeSeqNums[i]);
    GeometricShape *shape = (GeometricShape *)ptr_getIfExists(shapesBySeqNum, possibleImageShapeSeqNums[i]);
    if (!shape)
    {
      MSPUB_DEBUG_MSG(("Could not find shape of seqnum 0x%x in assignImages\n", possibleImageShapeSeqNums[i]));
      return;
    }
    if (index && *index - 1 < images.size())
    {
      ImgShape *toInsert = new ImgShape(*shape, images[*index - 1].first, images[*index - 1].second, this);
      shapesBySeqNum.erase(possibleImageShapeSeqNums[i]);
      shapesBySeqNum.insert(possibleImageShapeSeqNums[i], toInsert);
    }
    else
    {
      ShapeType *type = getIfExists(shapeTypesBySeqNum, possibleImageShapeSeqNums[i]);
      if (type)
      {
        shape->type = *type;
      }
      else
      {
        MSPUB_DEBUG_MSG(("Could not find shape type for shape of seqnum 0x%x\n", possibleImageShapeSeqNums[i]));
      }
      unsigned *ptr_lineColor = getIfExists(shapeLineColorsBySeqNum, possibleImageShapeSeqNums[i]);
      if (ptr_lineColor)
      {
        shape->setLine(*ptr_lineColor);
      }
      Fill *ptr_fill = ptr_getIfExists(shapeFillsBySeqNum, possibleImageShapeSeqNums[i]);
      if (ptr_fill && skipIfNotBgSeqNums.find(possibleImageShapeSeqNums[i]) == skipIfNotBgSeqNums.end())
      {
        shape->setFill(ptr_fill);
      }
    }
  }
}

WPXPropertyList libmspub::MSPUBCollector::getParaStyleProps(const ParagraphStyle &style, unsigned defaultParaStyleIndex)
{
  ParagraphStyle _nothing;
  const ParagraphStyle &defaultParaStyle = defaultParaStyleIndex < defaultParaStyles.size() ? defaultParaStyles[defaultParaStyleIndex] : _nothing;
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
  const CharacterStyle &defaultCharStyle = defaultCharStyleIndex < defaultCharStyles.size() ? defaultCharStyles[defaultCharStyleIndex] : _nothing;
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
  if (style.colorIndex >= 0 && (size_t)style.colorIndex < textColors.size())
  {
    ret.insert("fo:color", getColorString(getColorByReference(textColors[style.colorIndex])));
  }
  else if (defaultCharStyle.colorIndex >= 0 && (size_t)defaultCharStyle.colorIndex < textColors.size())
  {
    ret.insert("fo:color", getColorString(getColorByReference(textColors[defaultCharStyle.colorIndex])));
  }
  else
  {
    ret.insert("fo:color", getColorString(defaultColor));
  }
  if (style.fontIndex < fonts.size())
  {
    WPXString str;
    appendCharacters(str, fonts[style.fontIndex]);
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
  if (paletteColors.size() < 8)
  {
    paletteColors.insert(paletteColors.begin(), Color());
  }
  assignImages();
  assignTextShapes();
  // order the shapes in each page
  for (unsigned i = 0; i < shapeSeqNumsOrdered.size(); ++i)
  {
    unsigned *ptr_pageSeqNum = getIfExists(pageSeqNumsByShapeSeqNum, shapeSeqNumsOrdered[i]);
    if (ptr_pageSeqNum)
    {
      PageInfo *ptr_page = getIfExists(pagesBySeqNum, *ptr_pageSeqNum);
      if (ptr_page)
      {
        ptr_page->shapeSeqNumsOrdered.push_back(shapeSeqNumsOrdered[i]);
      }
    }
  }
  for (std::map<unsigned, PageInfo>::const_iterator i = pagesBySeqNum.begin(); i != pagesBySeqNum.end(); ++i)
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
    const std::vector<unsigned> &shapeSeqNumsOrdered = i->second.shapeSeqNumsOrdered; // for readability
    if (shapeSeqNumsOrdered.size() > 0)
    {
      m_painter->startGraphics(pageProps);
      unsigned *ptr_fillSeqNum = getIfExists(bgShapeSeqNumsByPageSeqNum, i->first);
      if (ptr_fillSeqNum)
      {
        Fill *ptr_fill = ptr_getIfExists(shapeFillsBySeqNum, *ptr_fillSeqNum);
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
        Shape *shape = ptr_getIfExists(shapesBySeqNum, shapeSeqNumsOrdered[i_seqNums]);
        if (shape)
        {
          Coordinate *coord = getIfExists(shapeCoordinatesBySeqNum, shapeSeqNumsOrdered[i_seqNums]);
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
  textStringsById[id] = str;
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
  while (images.size() < index)
  {
    images.push_back(std::pair<ImgType, WPXBinaryData>(UNKNOWN, WPXBinaryData()));
  }
  images[index - 1] = std::pair<ImgType, WPXBinaryData>(type, img);
  return true;
}

bool libmspub::MSPUBCollector::addShape(unsigned seqNum, unsigned pageSeqNum)
{
  shapesBySeqNum.insert(seqNum, new GeometricShape(pageSeqNum, this));
  possibleImageShapeSeqNums.push_back(seqNum);
  PageInfo *page = getIfExists(pagesBySeqNum, pageSeqNum);
  if (page)
  {
    page->shapeSeqNums.push_back(seqNum);
    pageSeqNumsByShapeSeqNum.insert(std::pair<unsigned, unsigned>(seqNum, pageSeqNum));
  }
  return true;
}

void libmspub::MSPUBCollector::addTextColor(unsigned c)
{
  textColors.push_back(c);
}

libmspub::Color libmspub::MSPUBCollector::getColorByReference(unsigned c) const
{
  unsigned char type = (c >> 24) & 0xFF;
  if (type == 0x08)
  {
    if ((c & 0xFFFFFF) >= paletteColors.size())
    {
      return defaultColor;
    }
    return paletteColors[c & 0xFFFFFF];
  }
  return Color(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
