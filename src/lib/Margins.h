/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_MARGINS_H
#define INCLUDED_MARGINS_H
namespace libmspub
{
struct Margins
{
  Margins(unsigned left, unsigned top, unsigned right, unsigned bottom) :
    m_left(left), m_top(top), m_right(right), m_bottom(bottom) { }
  Margins() : m_left(0), m_top(0), m_right(0), m_bottom(0) { }
  unsigned m_left, m_top, m_right, m_bottom;
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
