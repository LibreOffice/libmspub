/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __SHADOW_H__
#define __SHADOW_H__

#include "ColorReference.h"

namespace libmspub
{
enum ShadowType
{
  OFFSET,
  DOUBLE,
  RICH,
  SHADOW_SHAPE,
  DRAWING,
  EMBOSS_OR_ENGRAVE
};
struct Shadow
{
  ShadowType m_type;
  int m_offsetXInEmu;
  int m_offsetYInEmu;
  int m_SecondOffsetXInEmu;
  int m_SecondOffsetYInEmu;
  double m_originXInEmu;
  double m_originYInEmu;
  double m_opacity;
  ColorReference m_color;
  ColorReference m_highColor;
  Shadow(ShadowType type, int offsetXInEmu, int offsetYInEmu, int secondOffsetXInEmu, int secondOffsetYInEmu,
         double originXInEmu, double originYInEmu, double opacity,
         ColorReference color, ColorReference colorH)
    : m_type(type), m_offsetXInEmu(offsetXInEmu), m_offsetYInEmu(offsetYInEmu),
      m_SecondOffsetXInEmu(secondOffsetXInEmu), m_SecondOffsetYInEmu(secondOffsetYInEmu),
      m_originXInEmu(originXInEmu), m_originYInEmu(originYInEmu),
      m_opacity(opacity), m_color(color), m_highColor(colorH)
  {
  }
};
bool needsEmulation(const Shadow &shadow);
} // namespace libmspub

#endif /* __SHADOW_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
