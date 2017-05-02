/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __COORDINATE_H__
#define __COORDINATE_H__
#include "MSPUBConstants.h"
namespace libmspub
{
struct Coordinate
{
  Coordinate(int xs, int ys, int xe, int ye) : m_xs(xs), m_ys(ys), m_xe(xe), m_ye(ye) { }
  Coordinate() : m_xs(0), m_ys(0), m_xe(0), m_ye(0) { }
  int m_xs, m_ys, m_xe, m_ye;
  double getXIn(double pageWidth) const
  {
    return pageWidth / 2 + double(m_xs) / EMUS_IN_INCH;
  }
  double getYIn(double pageHeight) const
  {
    return pageHeight / 2 + double(m_ys) / EMUS_IN_INCH;
  }
  double getWidthIn() const
  {
    return double(int64_t(m_xe) - m_xs) / EMUS_IN_INCH;
  }
  double getHeightIn() const
  {
    return double(int64_t(m_ye) - m_ys) / EMUS_IN_INCH;
  }
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
