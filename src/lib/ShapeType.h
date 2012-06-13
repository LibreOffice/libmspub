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

#ifndef __SHAPETYPE_H__
#define __SHAPETYPE_H__

namespace libmspub
{
enum ShapeType
{
  RECTANGLE         = 1,
  ROUND_RECTANGLE   = 2,
  ELLIPSE           = 3,
  DIAMOND           = 4,
  ISOCELES_TRIANGLE = 5,
  RIGHT_TRIANGLE    = 6,
  PARALLELOGRAM     = 7,
  TRAPEZOID         = 8,
  HEXAGON           = 9,
  OCTAGON           = 10,
  PLUS              = 11,
  STAR              = 12,
  ARROW             = 13,
  HOME_PLATE        = 15,
  CUBE              = 16,
  BALLOON           = 17,
  SEAL_4            = 0xBB
};
} // libmspub
#endif /* __SHAPETYPE_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
