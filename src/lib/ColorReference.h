/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_COLORREFERENCE_H
#define INCLUDED_COLORREFERENCE_H

#include <vector>

#include "MSPUBTypes.h"

namespace libmspub
{
class ColorReference
{
  unsigned m_baseColor;
  unsigned m_modifiedColor;
//  static const unsigned char COLOR_PALETTE;
  static const unsigned char CHANGE_INTENSITY;
  static const unsigned char BLACK_BASE;
  static const unsigned char WHITE_BASE;
  Color getRealColor(unsigned c, const std::vector<Color> &palette) const;
public:
  explicit ColorReference(unsigned color) : m_baseColor(color), m_modifiedColor(color) { }
  ColorReference(unsigned baseColor, unsigned modifiedColor) : m_baseColor(baseColor), m_modifiedColor(modifiedColor) { }
  Color getFinalColor(const std::vector<Color> &palette) const;
public:
  friend bool operator==(const libmspub::ColorReference &, const libmspub::ColorReference &);
};
}

#endif /* INCLUDED_COLORREFERENCE_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
