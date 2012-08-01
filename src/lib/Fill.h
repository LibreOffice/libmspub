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

#ifndef __FILL_H__
#define __FILL_H__

#include <cstddef>

#include <vector>

#include <libwpd/libwpd.h>

#include "ColorReference.h"

namespace libmspub
{
class MSPUBCollector;
class Fill
{
protected:
  const MSPUBCollector *m_owner;
public:
  Fill(const MSPUBCollector *owner);
  virtual WPXPropertyListVector getProperties(WPXPropertyList *out) const = 0;
  virtual ~Fill() { }
private:
  Fill(const Fill &) : m_owner(NULL) { }
  Fill &operator=(const Fill &);
};

class ImgFill : public Fill
{
protected:
  unsigned m_imgIndex;
private:
  bool m_isTexture;
public:
  ImgFill(unsigned imgIndex, const MSPUBCollector *owner, bool isTexture);
  virtual WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  ImgFill(const ImgFill &) : Fill(NULL), m_imgIndex(0), m_isTexture(false) { }
  ImgFill &operator=(const ImgFill &);
};

class PatternFill : public ImgFill
{
  ColorReference m_fg;
  ColorReference m_bg;
public:
  PatternFill(unsigned imgIndex, const MSPUBCollector *owner, ColorReference fg, ColorReference bg);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  PatternFill(const PatternFill &) : ImgFill(0, NULL, true), m_fg(0x08000000), m_bg(0x08000000) { }
  PatternFill &operator=(const ImgFill &);
};

class SolidFill : public Fill
{
  ColorReference m_color;
  double m_opacity;
public:
  SolidFill(ColorReference color, double opacity, const MSPUBCollector *owner);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  SolidFill(const SolidFill &) : Fill(NULL), m_color(0x08000000), m_opacity(1) { }
  SolidFill &operator=(const SolidFill &);
};

class GradientFill : public Fill
{
  struct StopInfo
  {
    ColorReference m_colorReference;
    unsigned m_offsetPercent;
    double m_opacity;
    StopInfo(ColorReference colorReference, unsigned offsetPercent, double opacity) : m_colorReference(colorReference), m_offsetPercent(offsetPercent), m_opacity(opacity) { }
  };
  std::vector<StopInfo> m_stops;
  double m_angle;
public:
  GradientFill(const MSPUBCollector *owner, double angle = 0);
  void addColor(ColorReference c, unsigned offsetPercent, double opacity);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  GradientFill(const GradientFill &) : Fill(NULL), m_stops(), m_angle(0) { }
  GradientFill &operator=(const GradientFill &);
};
}

#endif /* __FILL_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
