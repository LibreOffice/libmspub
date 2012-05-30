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
  Fill &operator=(const Fill &)
  {
    return *this;
  }
};

class ImgFill : public Fill
{
  unsigned m_imgIndex;
public:
  ImgFill(unsigned imgIndex, const MSPUBCollector *owner);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  ImgFill(const ImgFill &) : Fill(NULL), m_imgIndex(0) { }
  ImgFill &operator=(const ImgFill &)
  {
    return *this;
  }
};

class SolidFill : public Fill
{
  unsigned m_color;
  double m_opacity;
public:
  SolidFill(unsigned color, double opacity, const MSPUBCollector *owner);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  SolidFill(const SolidFill &) : Fill(NULL), m_color(0), m_opacity(1) { }
  SolidFill &operator=(const SolidFill &)
  {
    return *this;
  }
};

class GradientFill : public Fill
{
  std::vector<std::pair<unsigned, unsigned> > colors;
  double m_angle;
public:
  GradientFill(const MSPUBCollector *owner, double angle = 0);
  void addColor(unsigned c, unsigned offsetPercent);
  WPXPropertyListVector getProperties(WPXPropertyList *out) const;
private:
  GradientFill(const GradientFill &) : Fill(NULL), colors(), m_angle(0) { }
  GradientFill &operator=(const GradientFill &)
  {
    return *this;
  }
};
}

#endif /* __FILL_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
