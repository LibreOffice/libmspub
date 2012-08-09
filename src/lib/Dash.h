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

#ifndef __DASH_H__
#define __DASH_H__

#include <boost/optional.hpp>
#include <vector>

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

#endif /* __DASH_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
