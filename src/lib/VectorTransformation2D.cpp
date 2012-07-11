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

#include "VectorTransformation2D.h"
#include <math.h>

libmspub::VectorTransformation2D::VectorTransformation2D() : m_m11(1), m_m12(0), m_m21(0), m_m22(1), m_x(0), m_y(0)
{
}

//We choose by convention to make function composition LEFT-multiplication, rather than right.
libmspub::VectorTransformation2D libmspub::operator*(const VectorTransformation2D &l, const VectorTransformation2D &r)
{
  VectorTransformation2D ret;
  ret.m_m11 = l.m_m11 * r.m_m11 + l.m_m12 * r.m_m21;
  ret.m_m12 = l.m_m11 * r.m_m12 + l.m_m12 * r.m_m22;
  ret.m_m21 = l.m_m21 * r.m_m11 + l.m_m22 * r.m_m21;
  ret.m_m22 = l.m_m21 * r.m_m12 + l.m_m22 * r.m_m22;
  ret.m_x   = l.m_m11 * r.m_x   + l.m_m12 * r.m_y + l.m_x;
  ret.m_y   = l.m_m21 * r.m_x   + l.m_m22 * r.m_y + l.m_y;
  return ret;
}

libmspub::VectorTransformation2D libmspub::VectorTransformation2D::fromFlips(bool flipH, bool flipV)
{
  VectorTransformation2D ret;
  ret.m_m21 = ret.m_m12 = 0;
  ret.m_m11 = flipH ? -1 : 1;
  ret.m_m22 = flipV ? -1 : 1;
  return ret;
}

libmspub::VectorTransformation2D libmspub::VectorTransformation2D::fromTranslate(double x, double y)
{
  VectorTransformation2D ret;
  ret.m_m11 = ret.m_m22 = 1;
  ret.m_m21 = ret.m_m12 = 0;
  ret.m_x = x;
  ret.m_y = y;
  return ret;
}

libmspub::VectorTransformation2D libmspub::VectorTransformation2D::fromCounterRadians(double theta)
{
  VectorTransformation2D ret;
  ret.m_m11 = cos(theta);
  ret.m_m12 = -sin(theta);
  ret.m_m21 = sin(theta);
  ret.m_m22 = cos(theta);
  return ret;
}

libmspub::Vector2D libmspub::VectorTransformation2D::transform(Vector2D v) const
{
  double x = m_m11 * v.m_x + m_m12 * v.m_y + m_x;
  double y = m_m21 * v.m_x + m_m22 * v.m_y + m_y;
  return Vector2D(x, y);
}

libmspub::Vector2D libmspub::VectorTransformation2D::transformWithOrigin(Vector2D v, Vector2D origin) const
{
  return transform(v - origin) + origin;
}

libmspub::Vector2D libmspub::operator+(const Vector2D &l, const Vector2D &r)
{
  double x = l.m_x + r.m_x;
  double y = l.m_y + r.m_y;
  return Vector2D(x, y);
}

libmspub::Vector2D libmspub::operator-(const Vector2D &l, const Vector2D &r)
{
  double x = l.m_x - r.m_x;
  double y = l.m_y - r.m_y;
  return Vector2D(x, y);
}

double libmspub::VectorTransformation2D::getRotation() const
{
  if (fabs(getHorizontalScaling()) > 0.0001)
  {
    return atan2(m_m21, m_m11);
  }
  if (fabs(getVerticalScaling()) > 0.0001)
  {
    return atan2(-m_m12, m_m22);
  }
  return 0;
}

double libmspub::VectorTransformation2D::getHorizontalScaling() const
{
  return m_m11 * m_m11 + m_m21 * m_m21;
}

double libmspub::VectorTransformation2D::getVerticalScaling() const
{
  return m_m12 * m_m12 + m_m22 * m_m22;
}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
