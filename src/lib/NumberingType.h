/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __NUMBERINGTYPE_H__
#define __NUMBERINGTYPE_H__

namespace libmspub
{
enum NumberingType
{
  STANDARD_WESTERN,
  UPPERCASE_ROMAN,
  LOWERCASE_ROMAN,
  UPPERCASE_LETTERS,
  LOWERCASE_LETTERS,
  ORDINALS,
  SPELLED_CARDINALS,
  SPELLED_ORDINALS,
  STANDARD_WESTERN_AT_LEAST_TWO_DIGITS = 0x16
};
} // namespace libmspub

#endif /* __NUMBERINGTYPE_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
