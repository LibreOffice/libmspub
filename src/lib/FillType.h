/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __FILLTYPE_H__
#define __FILLTYPE_H__

namespace libmspub
{
enum FillType
{
  SOLID = 0x00,
  PATTERN = 0x01,
  TEXTURE = 0x02,
  BITMAP = 0x03,
  SHADE = 0x04, // msofillShade       Shade from start to end points
  SHADE_CENTER = 0x05,  // msofillShadeCenter Shade from bounding rectangle to end point
  SHADE_SHAPE = 0x06, // msofillShadeShape  Shade from shape outline to end point
  SHADE_SCALE = 0x07, // msofillShadeScale Similar to msofillShade, but the fillAngle
  // is additionally scaled by the aspect ratio of
  // the shape. If shape is square, it is the
  // same as msofillShade
  SHADE_TITLE = 0x08,
  BACKGROUND = 0x09
};
} // libmspub
#endif /* __FILLTYPE_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
