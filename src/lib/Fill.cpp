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

#include "Fill.h"
#include "FillType.h"
#include "libmspub_utils.h"
#include "MSPUBCollector.h"

using namespace libmspub;

Fill::Fill(const MSPUBCollector *owner) : m_owner(owner)
{
}

ImgFill::ImgFill(unsigned imgIndex, const MSPUBCollector *owner, bool isTexture, int rot) : Fill(owner), m_imgIndex(imgIndex), m_isTexture(isTexture), m_rotation(rot)
{
}

librevenge::RVNGPropertyListVector ImgFill::getProperties(librevenge::RVNGPropertyList *out) const
{
  out->insert("draw:fill", "bitmap");
  if (m_imgIndex > 0 && m_imgIndex <= m_owner->m_images.size())
  {
    const std::pair<ImgType, librevenge::RVNGBinaryData> &img = m_owner->m_images[m_imgIndex - 1];
    out->insert("librevenge:mime-type", mimeByImgType(img.first));
    out->insert("draw:fill-image", img.second.getBase64Data());
    out->insert("draw:fill-image-ref-point", "top-left");
    if (! m_isTexture)
    {
      out->insert("style:repeat", "stretch");
    }
    if (m_rotation != 0)
    {
      librevenge::RVNGString sValue;
      sValue.sprintf("%d", m_rotation);
      out->insert("librevenge:rotate", sValue);
    }
  }
  return librevenge::RVNGPropertyListVector();
}

PatternFill::PatternFill(unsigned imgIndex, const MSPUBCollector *owner, ColorReference fg, ColorReference bg) : ImgFill(imgIndex, owner, true, 0), m_fg(fg), m_bg(bg)
{
}

librevenge::RVNGPropertyListVector PatternFill::getProperties(librevenge::RVNGPropertyList *out) const
{
  Color fgColor = m_fg.getFinalColor(m_owner->m_paletteColors);
  Color bgColor = m_bg.getFinalColor(m_owner->m_paletteColors);
  out->insert("draw:fill", "bitmap");
  if (m_imgIndex > 0 && m_imgIndex <= m_owner->m_images.size())
  {
    const std::pair<ImgType, librevenge::RVNGBinaryData> &img = m_owner->m_images[m_imgIndex - 1];
    const ImgType &type = img.first;
    const librevenge::RVNGBinaryData *data = &img.second;
    // fix broken MSPUB DIB by putting in correct fg and bg colors
    librevenge::RVNGBinaryData fixedImg;
    if (type == DIB && data->size() >= 0x36 + 8)
    {
      fixedImg.append(data->getDataBuffer(), 0x36);
      fixedImg.append(fgColor.b);
      fixedImg.append(fgColor.g);
      fixedImg.append(fgColor.r);
      fixedImg.append((unsigned char)'\0');
      fixedImg.append(bgColor.b);
      fixedImg.append(bgColor.g);
      fixedImg.append(bgColor.r);
      fixedImg.append((unsigned char)'\0');
      fixedImg.append(data->getDataBuffer() + 0x36 + 8, data->size() - 0x36 - 8);
      data = &fixedImg;
    }
    out->insert("librevenge:mime-type", mimeByImgType(type));
    out->insert("draw:fill-image", data->getBase64Data());
    out->insert("draw:fill-image-ref-point", "top-left");
  }
  return librevenge::RVNGPropertyListVector();
}

SolidFill::SolidFill(ColorReference color, double opacity, const MSPUBCollector *owner) : Fill(owner), m_color(color), m_opacity(opacity)
{
}

librevenge::RVNGPropertyListVector SolidFill::getProperties(librevenge::RVNGPropertyList *out) const
{
  Color fillColor = m_color.getFinalColor(m_owner->m_paletteColors);
  out->insert("draw:fill", "solid");
  out->insert("draw:fill-color", MSPUBCollector::getColorString(fillColor));
  librevenge::RVNGString val;
  val.sprintf("%d%%", (int)(m_opacity * 100));
  out->insert("draw:opacity", val);
  out->insert("svg:fill-rule", "nonzero");
  return librevenge::RVNGPropertyListVector();
}

GradientFill::GradientFill(const MSPUBCollector *owner, double angle, int type) : Fill(owner), m_stops(), m_angle(angle), m_type(type), m_fillLeftVal(0.0), m_fillTopVal(0.0), m_fillRightVal(0.0), m_fillBottomVal(0.0)
{
}

void GradientFill::setFillCenter(double left, double top, double right, double bottom)
{
  m_fillLeftVal = left;
  m_fillTopVal = top;
  m_fillRightVal = right;
  m_fillBottomVal = bottom;
}

void GradientFill::addColor(ColorReference c, unsigned offsetPercent, double opacity)
{
  m_stops.push_back(StopInfo(c, offsetPercent, opacity));
}

void GradientFill::addColorReverse(ColorReference c, unsigned offsetPercent, double opacity)
{
  m_stops.insert(m_stops.begin(), StopInfo(c, offsetPercent, opacity));
}

void GradientFill::completeComplexFill()
{
  unsigned stops = m_stops.size();
  for (unsigned i = stops; i > 0; i--)
  {
    if (m_stops[i-1].m_offsetPercent != 50)
      m_stops.push_back(StopInfo(m_stops[i-1].m_colorReference, 50 - m_stops[i-1].m_offsetPercent + 50, m_stops[i-1].m_opacity));
  }
}

librevenge::RVNGPropertyListVector GradientFill::getProperties(librevenge::RVNGPropertyList *out) const
{
  librevenge::RVNGPropertyListVector ret;
  out->insert("draw:fill", "gradient");
  out->insert("svg:fill-rule", "nonzero");
  out->insert("draw:angle", -m_angle); // draw:angle is clockwise in odf format
  switch (m_type)
  {
  case SHADE_CENTER:
    out->insert("libmspub:shade", "center");
    if ((m_fillLeftVal > 0.5) && (m_fillTopVal > 0.5) && (m_fillRightVal > 0.5) && (m_fillBottomVal > 0.5))
      out->insert("libmspub:shade-ref-point", "bottom-right");
    else if ((m_fillLeftVal < 0.5) && (m_fillTopVal < 0.5) && (m_fillRightVal < 0.5) && (m_fillBottomVal < 0.5))
      out->insert("libmspub:shade-ref-point", "top-left");
    else if ((m_fillLeftVal > 0.5) && (m_fillTopVal < 0.5) && (m_fillRightVal > 0.5) && (m_fillBottomVal < 0.5))
      out->insert("libmspub:shade-ref-point", "top-right");
    else if ((m_fillLeftVal < 0.5) && (m_fillTopVal > 0.5) && (m_fillRightVal < 0.5) && (m_fillBottomVal > 0.5))
      out->insert("libmspub:shade-ref-point", "bottom-left");
    break;
  case SHADE_SHAPE:
    out->insert("libmspub:shade", "shape");
    break;
  case SHADE:
  case SHADE_SCALE:
  default:
    out->insert("libmspub:shade", "normal");
    break;
  }
  for (unsigned i = 0; i < m_stops.size(); ++i)
  {
    Color c = m_stops[i].m_colorReference.getFinalColor(m_owner->m_paletteColors);
    librevenge::RVNGPropertyList stopProps;
    librevenge::RVNGString sValue;
    sValue.sprintf("%d%%", m_stops[i].m_offsetPercent);
    stopProps.insert("svg:offset", sValue);
    stopProps.insert("svg:stop-color", MSPUBCollector::getColorString(c));
    sValue.sprintf("%d%%", (int)(m_stops[i].m_opacity * 100));
    stopProps.insert("svg:stop-opacity", sValue);
    ret.append(stopProps);
  }
  return ret;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
