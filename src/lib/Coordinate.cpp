/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "Coordinate.h"

#include <utility>

namespace libmspub
{

Coordinate::Coordinate(int xs, int ys, int xe, int ye)
  : m_xs(xs)
  , m_ys(ys)
  , m_xe(xe)
  , m_ye(ye)
{
  arrange();
}

void Coordinate::arrange()
{
  if (m_xs > m_xe)
    std::swap(m_xs, m_xe);
  if (m_ys > m_ye)
    std::swap(m_ys, m_ye);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
