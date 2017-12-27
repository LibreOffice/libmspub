/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LINE_H
#define INCLUDED_LINE_H
#include "Dash.h"
#include <boost/optional.hpp>
#include "ColorReference.h"

namespace libmspub
{
struct Line
{
  ColorReference m_color;
  unsigned m_widthInEmu;
  bool m_lineExists;
  boost::optional<Dash> m_dash;
  Line(ColorReference color, unsigned widthInEmu, bool lineExists) :
    m_color(color), m_widthInEmu(widthInEmu), m_lineExists(lineExists),
    m_dash() { }
  Line(ColorReference color, unsigned widthInEmu, bool lineExists, Dash dash) :
    m_color(color), m_widthInEmu(widthInEmu), m_lineExists(lineExists),
    m_dash(dash) { }
  bool operator==(const Line &r) const
  {
    return m_color == r.m_color && m_widthInEmu == r.m_widthInEmu &&
           m_lineExists == r.m_lineExists && m_dash == r.m_dash;
  }
};
}

#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
