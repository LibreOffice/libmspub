/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//TODO : Adjust handles, glue points

#include "VectorTransformation2D.h"
#include <math.h>

namespace libmspub
{

VectorTransformation2D::VectorTransformation2D() : m_m11(1), m_m12(0), m_m21(0), m_m22(1), m_x(0), m_y(0)
{
}

//We choose by convention to make function composition LEFT-multiplication, rather than right.
VectorTransformation2D operator*(const VectorTransformation2D &l, const VectorTransformation2D &r)
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

VectorTransformation2D VectorTransformation2D::fromFlips(bool flipH, bool flipV)
{
  VectorTransformation2D ret;
  ret.m_m21 = ret.m_m12 = 0;
  ret.m_m11 = flipH ? -1 : 1;
  ret.m_m22 = flipV ? -1 : 1;
  return ret;
}

VectorTransformation2D VectorTransformation2D::fromTranslate(double x, double y)
{
  VectorTransformation2D ret;
  ret.m_m11 = ret.m_m22 = 1;
  ret.m_m21 = ret.m_m12 = 0;
  ret.m_x = x;
  ret.m_y = y;
  return ret;
}

VectorTransformation2D VectorTransformation2D::fromCounterRadians(double theta)
{
  VectorTransformation2D ret;
  ret.m_m11 = cos(theta);
  ret.m_m12 = -sin(theta);
  ret.m_m21 = sin(theta);
  ret.m_m22 = cos(theta);
  return ret;
}

Vector2D VectorTransformation2D::transform(Vector2D v) const
{
  double x = m_m11 * v.m_x + m_m12 * v.m_y + m_x;
  double y = m_m21 * v.m_x + m_m22 * v.m_y + m_y;
  return Vector2D(x, y);
}

Vector2D VectorTransformation2D::transformWithOrigin(Vector2D v, Vector2D origin) const
{
  return transform(v - origin) + origin;
}

Vector2D operator+(const Vector2D &l, const Vector2D &r)
{
  double x = l.m_x + r.m_x;
  double y = l.m_y + r.m_y;
  return Vector2D(x, y);
}

Vector2D operator-(const Vector2D &l, const Vector2D &r)
{
  double x = l.m_x - r.m_x;
  double y = l.m_y - r.m_y;
  return Vector2D(x, y);
}

double VectorTransformation2D::getRotation() const
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

double VectorTransformation2D::getHorizontalScaling() const
{
  return m_m11 * m_m11 + m_m21 * m_m21;
}

double VectorTransformation2D::getVerticalScaling() const
{
  return m_m12 * m_m12 + m_m22 * m_m22;
}

bool VectorTransformation2D::orientationReversing() const
{
  // Is the determinant negative?
  return m_m11 * m_m22 < m_m12 * m_m21;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
