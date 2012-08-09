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

#include "Dash.h"
#include "libmspub_utils.h"

bool libmspub::operator==(const libmspub::Dot &lhs, const libmspub::Dot &rhs)
{
  return lhs.m_length == rhs.m_length && lhs.m_count == rhs.m_count;
}

bool libmspub::operator!=(const libmspub::Dot &lhs, const libmspub::Dot &rhs)
{
  return !operator==(lhs, rhs);
}

bool libmspub::operator==(const libmspub::Dash &lhs, const libmspub::Dash &rhs)
{
  if (!(lhs.m_distance == rhs.m_distance && 
    lhs.m_dotStyle == rhs.m_dotStyle && lhs.m_dots.size() == rhs.m_dots.size()))
  {
    return false;
  }
  for (unsigned i = 0; i < lhs.m_dots.size(); ++i)
  {
    if (lhs.m_dots[i] != rhs.m_dots[i])
    {
      return false;
    }
  }
  return true;
}

libmspub::Dash libmspub::getDash(MSPUBDashStyle style, unsigned shapeLineWidthEmu,
    DotStyle dotStyle)
{
  double shapeLineWidth = static_cast<double>(shapeLineWidthEmu) /
    EMUS_IN_INCH;
  switch (style)
  {
  default:
    MSPUB_DEBUG_MSG(("Couldn't match dash style, using solid line.\n"));
  case MSPUB_DS_SOLID:
    return Dash(0, RECT_DOT);
  case DASH_SYS:
  {
    Dash ret(shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 3 * shapeLineWidth));
    return ret;
  }
  case DOT_SYS:
  {
    Dash ret(shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1));
    return ret;
  }
  case DASH_DOT_SYS:
  {
    Dash ret(shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 3 * shapeLineWidth));
    ret.m_dots.push_back(Dot(1));
    return ret;
  }
  case DASH_DOT_DOT_SYS:
  {
    Dash ret(shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 3 * shapeLineWidth));
    ret.m_dots.push_back(Dot(2));
    return ret;
  }
  case DOT_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1));
    return ret;
  }
  case DASH_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 4 * shapeLineWidth));
    return ret;
  }
  case LONG_DASH_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 8 * shapeLineWidth));
    return ret;
  }
  case DASH_DOT_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 4 * shapeLineWidth));
    ret.m_dots.push_back(Dot(1));
    return ret;
  }
  case LONG_DASH_DOT_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 8 * shapeLineWidth));
    ret.m_dots.push_back(Dot(1));
    return ret;
  }
  case LONG_DASH_DOT_DOT_GEL:
  {
    Dash ret(3 * shapeLineWidth, dotStyle);
    ret.m_dots.push_back(Dot(1, 8 * shapeLineWidth));
    ret.m_dots.push_back(Dot(2));
    return ret;
  }
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
