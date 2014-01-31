/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
