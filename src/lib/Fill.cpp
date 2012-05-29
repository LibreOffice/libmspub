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

ImgFill::ImgFill(unsigned imgIndex, const MSPUBCollector *owner) : Fill(owner), m_imgIndex(imgIndex)
{
}

WPXPropertyListVector ImgFill::getProperties(WPXPropertyList *out) const
{
  out->insert("draw:fill", "bitmap");
  if (m_imgIndex <= m_owner->images.size())
  {
    const std::pair<ImgType, WPXBinaryData> &img = m_owner->images[m_imgIndex - 1];
    out->insert("libwpg:mime-type", MSPUBCollector::ImgShape::mimeByImgType(img.first));
    out->insert("draw:fill-image", img.second.getBase64Data());
    out->insert("draw:fill-image-ref-point", "top-left");
  }
  return WPXPropertyListVector();
}

SolidFill::SolidFill(unsigned color, const MSPUBCollector *owner) : Fill(owner), m_color(color)
{
}

WPXPropertyListVector SolidFill::getProperties(WPXPropertyList *out) const
{
  Color fillColor = m_owner->getColorByReference(m_color);
  out->insert("draw:fill", "solid");
  out->insert("draw:fill-color", MSPUBCollector::getColorString(fillColor));
  return WPXPropertyListVector();
}

GradientFill::GradientFill(const MSPUBCollector *owner, double angle) : Fill(owner), colors(), m_angle(angle)
{
}

void GradientFill::addColor(unsigned c, unsigned offsetPercent)
{
  colors.push_back(std::pair<unsigned, unsigned>(c, offsetPercent));
}

WPXPropertyListVector GradientFill::getProperties(WPXPropertyList *out) const
{
  WPXPropertyListVector ret;
  out->insert("draw:fill", "gradient");
  out->insert("draw:angle", m_angle);
  for (unsigned i = 0; i < colors.size(); ++i)
  {
    Color c = m_owner->getColorByReference(colors[i].first);
    WPXPropertyList stopProps;
    WPXString sValue;
    sValue.sprintf("%d%%", colors[i].second);
    stopProps.insert("svg:offset", sValue);
    stopProps.insert("svg:stop-color", MSPUBCollector::getColorString(c));
    ret.append(stopProps);
  }
  return ret;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
