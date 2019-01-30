/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "Dash.h"

#include "MSPUBConstants.h"
#include "libmspub_utils.h"

namespace libmspub
{

bool operator==(const Dot &lhs, const Dot &rhs)
{
  return lhs.m_length == rhs.m_length && lhs.m_count == rhs.m_count;
}

bool operator!=(const Dot &lhs, const Dot &rhs)
{
  return !operator==(lhs, rhs);
}

bool operator==(const Dash &lhs, const Dash &rhs)
{
  if (!(lhs.m_distance == rhs.m_distance &&
        lhs.m_dotStyle == rhs.m_dotStyle && lhs.m_dots.size() == rhs.m_dots.size()))
  {
    return false;
  }
  for (size_t i = 0; i < lhs.m_dots.size(); ++i)
  {
    if (lhs.m_dots[i] != rhs.m_dots[i])
    {
      return false;
    }
  }
  return true;
}

Dash getDash(MSPUBDashStyle style, unsigned shapeLineWidthEmu,
             DotStyle dotStyle)
{
  double shapeLineWidth = static_cast<double>(shapeLineWidthEmu) /
                          EMUS_IN_INCH;
  switch (style)
  {
  default:
    MSPUB_DEBUG_MSG(("Couldn't match dash style, using solid line.\n"));
    MSPUB_FALLTHROUGH;
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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
