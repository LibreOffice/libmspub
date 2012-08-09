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
#ifndef __LINE_H__
#define __LINE_H__
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
