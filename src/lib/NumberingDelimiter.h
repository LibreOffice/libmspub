/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __NUMBERINGDELIMITER_H__
#define __NUMBERINGDELIMITER_H__

namespace libmspub
{
enum NumberingDelimiter
{
  NO_DELIMITER = -1,
  PARENTHESIS = 0,
  PARENTHESES_SURROUND = 1,
  PERIOD = 2,
  SQUARE_BRACKET = 4,
  COLON = 5,
  SQUARE_BRACKET_SURROUND = 6,
  HYPHEN_SURROUND = 7,
  IDEOGRAPHIC_HALF_COMMA = 8
};
} // namespace libmspub

#endif /* __NUMBERINGDELIMITER_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
