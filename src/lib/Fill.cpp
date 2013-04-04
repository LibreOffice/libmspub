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
#include "libmspub_utils.h"
#include "MSPUBCollector.h"

using namespace libmspub;

Fill::Fill(const MSPUBCollector *owner) : m_owner(owner)
{
}

ImgFill::ImgFill(unsigned imgIndex, const MSPUBCollector *owner, bool isTexture, int rot) : Fill(owner), m_imgIndex(imgIndex), m_isTexture(isTexture), m_rotation(rot)
{
}

WPXPropertyListVector ImgFill::getProperties(WPXPropertyList *out) const
{
  out->insert("draw:fill", "bitmap");
  if (m_imgIndex <= m_owner->m_images.size())
  {
    const std::pair<ImgType, WPXBinaryData> &img = m_owner->m_images[m_imgIndex - 1];
    out->insert("libwpg:mime-type", mimeByImgType(img.first));
    out->insert("draw:fill-image", img.second.getBase64Data());
    out->insert("draw:fill-image-ref-point", "top-left");
    if (! m_isTexture)
    {
      out->insert("style:repeat", "stretch");
    }
    if (m_rotation != 0)
    {
      WPXString sValue;
      sValue.sprintf("%d", m_rotation);
      out->insert("libwpg:rotate", sValue);
    }
  }
  return WPXPropertyListVector();
}

PatternFill::PatternFill(unsigned imgIndex, const MSPUBCollector *owner, ColorReference fg, ColorReference bg) : ImgFill(imgIndex, owner, true, 0), m_fg(fg), m_bg(bg)
{
}

WPXPropertyListVector PatternFill::getProperties(WPXPropertyList *out) const
{
  Color fgColor = m_fg.getFinalColor(m_owner->m_paletteColors);
  Color bgColor = m_bg.getFinalColor(m_owner->m_paletteColors);
  out->insert("draw:fill", "bitmap");
  if (m_imgIndex <= m_owner->m_images.size())
  {
    const std::pair<ImgType, WPXBinaryData> &img = m_owner->m_images[m_imgIndex - 1];
    const ImgType &type = img.first;
    const WPXBinaryData *data = &img.second;
    // fix broken MSPUB DIB by putting in correct fg and bg colors
    WPXBinaryData fixedImg;
    if (type == DIB && data->size() >= 0x36 + 8)
    {
      fixedImg.append(data->getDataBuffer(), 0x36);
      fixedImg.append(bgColor.b);
      fixedImg.append(bgColor.g);
      fixedImg.append(bgColor.r);
      fixedImg.append('\0');
      fixedImg.append(fgColor.b);
      fixedImg.append(fgColor.g);
      fixedImg.append(fgColor.r);
      fixedImg.append('\0');
      fixedImg.append(data->getDataBuffer() + 0x36 + 8, data->size() - 0x36 - 8);
      data = &fixedImg;
    }
    out->insert("libwpg:mime-type", mimeByImgType(type));
    out->insert("draw:fill-image", data->getBase64Data());
    out->insert("draw:fill-image-ref-point", "top-left");
  }
  return WPXPropertyListVector();
}

SolidFill::SolidFill(ColorReference color, double opacity, const MSPUBCollector *owner) : Fill(owner), m_color(color), m_opacity(opacity)
{
}

WPXPropertyListVector SolidFill::getProperties(WPXPropertyList *out) const
{
  Color fillColor = m_color.getFinalColor(m_owner->m_paletteColors);
  out->insert("draw:fill", "solid");
  out->insert("draw:fill-color", MSPUBCollector::getColorString(fillColor));
  WPXString val;
  val.sprintf("%d%%", (int)(m_opacity * 100));
  out->insert("draw:opacity", val);
  out->insert("svg:fill-rule", "nonzero");
  return WPXPropertyListVector();
}

GradientFill::GradientFill(const MSPUBCollector *owner, double angle, int type) : Fill(owner), m_stops(), m_angle(angle), m_type(type)
{
}

void GradientFill::addColor(ColorReference c, unsigned offsetPercent, double opacity)
{
  m_stops.push_back(StopInfo(c, offsetPercent, opacity));
}

WPXPropertyListVector GradientFill::getProperties(WPXPropertyList *out) const
{
  WPXPropertyListVector ret;
  out->insert("draw:fill", "gradient");
  out->insert("svg:fill-rule", "nonzero");
  out->insert("draw:angle", -m_angle); // draw:angle is clockwise in odf format
  switch (m_type)
  {
  case 4:
  case 7:
    out->insert("libmspub:shade", "normal");
    break;
  case 5:
    out->insert("libmspub:shade", "center");
    break;
  case 6:
    out->insert("libmspub:shade", "shape");
    break;
  default:
    out->insert("libmspub:shade", "normal");
    break;
  }
  for (unsigned i = 0; i < m_stops.size(); ++i)
  {
    Color c = m_stops[i].m_colorReference.getFinalColor(m_owner->m_paletteColors);
    WPXPropertyList stopProps;
    WPXString sValue;
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
