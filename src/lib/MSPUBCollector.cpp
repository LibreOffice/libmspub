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
  m_widthSet(false), m_heightSet(false), m_commonPageProperties(),
  m_numPages(0), textStringsById(), pagesBySeqNum(), textShapesBySeqNum(),
  imgShapesBySeqNum(), images(), possibleImageShapes(), colors(), fonts(),
  defaultCharStyle(false, false, false, -1, -1)
{
}

libmspub::MSPUBCollector::~MSPUBCollector()
{
}

libmspub::ContentChunkReference::ContentChunkReference(libmspub::MSPUBContentChunkType type,
    unsigned long offset, unsigned long end,
    unsigned seqNum, unsigned parentSeqNum) :
  type(type), offset(offset), end(end), seqNum(seqNum), parentSeqNum(parentSeqNum)
{
}

void libmspub::MSPUBCollector::setDefaultCharacterStyle(const CharacterStyle &st)
{
  defaultCharStyle = st;
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

bool libmspub::MSPUBCollector::addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum)
{
  std::map<unsigned, PageInfo>::iterator i_page = pagesBySeqNum.find(pageSeqNum);
  if (i_page == pagesBySeqNum.end())
  {
    MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in addTextShape!\n", pageSeqNum));
    return false;
  }
  else
  {
    std::map<unsigned, std::vector<TextParagraph> >::iterator i_str = textStringsById.find(stringId);
    if (i_str == textStringsById.end())
    {
      MSPUB_DEBUG_MSG(("Text string of id 0x%x not found in addTextShape!\n", stringId));
      return false;
    }
    else
    {
      std::pair<std::map<unsigned, TextShapeInfo>::iterator, bool> result =
        textShapesBySeqNum.insert(std::pair<const unsigned, TextShapeInfo>(seqNum, TextShapeInfo(i_str->second)));
      if (result.second)
      {
        i_page->second.textShapeReferences.push_back(result.first);
        MSPUB_DEBUG_MSG(("addTextShape succeeded with id 0x%x\n", stringId));
        return true;
      }
      MSPUB_DEBUG_MSG(("already tried to add the text shape of seqnum 0x%x to this page!\n", seqNum));
      return false;
    }
  }
}

bool libmspub::MSPUBCollector::setShapeImgIndex(unsigned seqNum, unsigned index)
{
  MSPUB_DEBUG_MSG(("Setting image index of shape with seqnum 0x%x to 0x%x\n", seqNum, index));
  std::map<unsigned, UnknownShapeInfo>::iterator i = possibleImageShapes.find(seqNum);
  if (i != possibleImageShapes.end())
  {
    i->second.imgIndex = index;
    return true;
  }
  return false;
}

bool libmspub::MSPUBCollector::setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye)
{
  if (! (m_widthSet && m_heightSet))
  {
    return false;
  }
  WPXPropertyList *propsToEdit;
  std::map<unsigned, TextShapeInfo>::iterator elt = textShapesBySeqNum.find(seqNum);
  if (elt != textShapesBySeqNum.end())
  {
    propsToEdit = &(elt->second.props);
  }
  else
  {
    std::map<unsigned, UnknownShapeInfo>::iterator elt = possibleImageShapes.find(seqNum);
    if (elt != possibleImageShapes.end())
    {
      propsToEdit = &(elt->second.props);
    }
    else
    {
      return false;
    }
  }
  double x_center = m_commonPageProperties["svg:width"]->getDouble() / 2;
  double y_center = m_commonPageProperties["svg:height"]->getDouble() / 2;
  propsToEdit->insert("svg:x", x_center + (double)xs / EMUS_IN_INCH);
  propsToEdit->insert("svg:y", y_center + (double)ys / EMUS_IN_INCH);
  propsToEdit->insert("svg:width", (double)(xe - xs) / EMUS_IN_INCH);
  propsToEdit->insert("svg:height", (double)(ye - ys) / EMUS_IN_INCH);
  return true;
}

void libmspub::MSPUBCollector::addFont(std::vector<unsigned char> name)
{
  fonts.push_back(name);
}

void libmspub::MSPUBCollector::assignImages()
{
  for (std::map<unsigned, UnknownShapeInfo>::const_iterator i = possibleImageShapes.begin(); i != possibleImageShapes.end(); ++i)
  {
    if (i->second.imgIndex > 0 && i->second.imgIndex <= images.size())
    {
      std::pair<std::map<unsigned, ImgShapeInfo>::iterator, bool> result =
        imgShapesBySeqNum.insert(std::pair<unsigned, ImgShapeInfo>(i->first, ImgShapeInfo(images[i->second.imgIndex - 1].first, images[i->second.imgIndex - 1].second, i->second.props)));
      if (result.second)
      {
        std::map<unsigned, PageInfo>::iterator i_pg = pagesBySeqNum.find(i->second.pageSeqNum);
        if (i_pg != pagesBySeqNum.end())
        {
          i_pg->second.imgShapeReferences.push_back(result.first);
        }
      }
    }
  }
}

WPXPropertyList libmspub::MSPUBCollector::getParaStyleProps(const ParagraphStyle &style)
{
  WPXPropertyList ret;
  switch (style.align)
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

WPXPropertyList libmspub::MSPUBCollector::getCharStyleProps(const CharacterStyle &style)
{
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
  if (style.colorIndex >= 0 && (size_t)style.colorIndex < colors.size())
  {
    ret.insert("fo:color", getColorString(colors[style.colorIndex]));
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
  assignImages();
  for (std::map<unsigned, PageInfo>::const_iterator i = pagesBySeqNum.begin(); i != pagesBySeqNum.end(); ++i)
  {
    m_painter->startGraphics(m_commonPageProperties);
    for (std::vector<std::map<unsigned, TextShapeInfo>::const_iterator>::const_iterator j = i->second.textShapeReferences.begin();
         j != i->second.textShapeReferences.end(); ++j)
    {
      m_painter->startTextObject((*j)->second.props, WPXPropertyListVector());
      for (std::vector<TextParagraph>::const_iterator k = (*j)->second.str.begin(); k != (*j)->second.str.end(); ++k)
      {
        WPXPropertyList paraProps = getParaStyleProps(k->style);
        m_painter->startTextLine(paraProps);
        for (std::vector<TextSpan>::const_iterator l = k->spans.begin(); l != k->spans.end(); ++l)
        {
          WPXString text;
          appendCharacters(text, l->chars);
          WPXPropertyList charProps = getCharStyleProps(l->style);
          m_painter->startTextSpan(charProps);
          m_painter->insertText(text);
          m_painter->endTextSpan();
        }
        m_painter->endTextLine();
      }
      m_painter->endTextObject();
    }
    for (std::vector<std::map<unsigned, ImgShapeInfo>::const_iterator>::const_iterator j = i->second.imgShapeReferences.begin();
         j != i->second.imgShapeReferences.end(); ++j)
    {
      m_painter->drawGraphicObject((*j)->second.props, (*j)->second.img);
    }
    m_painter->endGraphics();
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
  m_width = widthInEmu;
  m_commonPageProperties.insert("svg:width", ((double)widthInEmu)/EMUS_IN_INCH);
  m_widthSet = true;
}

void libmspub::MSPUBCollector::setHeightInEmu(unsigned long heightInEmu)
{
  //FIXME: Warn if this is called twice
  m_height = heightInEmu;
  m_commonPageProperties.insert("svg:height", ((double)heightInEmu)/EMUS_IN_INCH);
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
  /*std::map<unsigned, unsigned>::const_iterator i_ind = imgIndicesByShapeSeqNum.find(seqNum);
  if (i_ind == imgIndicesByShapeSeqNum.end())
  {
    MSPUB_DEBUG_MSG(("Didn't find which embedded image index corresponds to shape 0x%x before trying to add the shape in addImageShape!\n", seqNum));
    return false;
  }
  unsigned imgIndex = i_ind->second;
  std::map<unsigned, PageInfo>::iterator i_page = pagesBySeqNum.find(pageSeqNum);
  if (i_page == pagesBySeqNum.end())
  {
    MSPUB_DEBUG_MSG(("Page of seqnum 0x%x not found in addImageShape!\n", pageSeqNum));
    return false;
  }
  if (imgIndex > images.size() || images[imgIndex - 1].first == UNKNOWN)
  {
    MSPUB_DEBUG_MSG(("Valid image of index %d not found in addImageShape!\n", imgIndex));
    return false;
  }
  std::pair<std::map<unsigned, ImgShapeInfo>::const_iterator, bool> result = imgShapesBySeqNum.insert(std::pair<const unsigned, ImgShapeInfo>(seqNum, ImgShapeInfo(images[imgIndex - 1].first, images[imgIndex - 1].second)));
  if (result.second)
  {
    i_page->second.imgShapeReferences.push_back(result.first);
    MSPUB_DEBUG_MSG(("addImageShape succeeded with id 0x%x", imgIndex));
    return true;
  }
  MSPUB_DEBUG_MSG(("already tried to add the image shape of seqnum 0x%x to this page!\n", seqNum));
  return false;*/
  possibleImageShapes.insert(std::pair<unsigned, UnknownShapeInfo>(seqNum, UnknownShapeInfo(pageSeqNum)));
  return true;
}

void libmspub::MSPUBCollector::addColor(unsigned char r, unsigned char g, unsigned char b)
{
  colors.push_back(Color(r, g, b));
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
