/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_DASH_H
#define INCLUDED_DASH_H

#include <vector>

#include <boost/optional.hpp>

namespace libmspub
{
enum DotStyle
{
  RECT_DOT,
  ROUND_DOT
};
enum MSPUBDashStyle
{
  MSPUB_DS_SOLID,
  DASH_SYS,
  DOT_SYS,
  DASH_DOT_SYS,
  DASH_DOT_DOT_SYS,
  DOT_GEL,
  DASH_GEL,
  LONG_DASH_GEL,
  DASH_DOT_GEL,
  LONG_DASH_DOT_GEL,
  LONG_DASH_DOT_DOT_GEL
};
struct Dot
{
  boost::optional<double> m_length;
  unsigned m_count;
  Dot(unsigned count) : m_length(), m_count(count)
  {
  }
  Dot(unsigned count, double length) : m_length(length), m_count(count)
  {
  }
};
struct Dash
{
  double m_distance;
  DotStyle m_dotStyle;
  std::vector<Dot> m_dots; // empty vector is interpreted as solid line
  Dash(double distance, DotStyle dotStyle) : m_distance(distance),
    m_dotStyle(dotStyle), m_dots()
  {
  }
};
bool operator!=(const Dot &lhs, const Dot &rhs);
bool operator==(const Dot &lhs, const Dot &rhs);
bool operator==(const Dash &lhs, const Dash &rhs);
Dash getDash(MSPUBDashStyle style, unsigned shapeLineWidthInEmu,
             DotStyle dotStyle);
} // namespace libmspub

#endif /* INCLUDED_DASH_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
