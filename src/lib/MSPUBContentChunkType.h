/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_MSPUBCHUNKTYPE_H
#define INCLUDED_MSPUBCHUNKTYPE_H

namespace libmspub
{
enum MSPUBContentChunkType //Please fill this in and replace magic constants in MSPUBParser accordingly as you discover what a given chunk type means!
{
  UNKNOWN_CHUNK  = 0,
  SHAPE      = 0x01,
  ALTSHAPE   = 0x20,
  DOCUMENT   = 0x44,
  PAGE       = 0x43,
  PALETTE    = 0x5C,
  BORDER_ART = 0x46,
  GROUP      = 0x30,
  LOGO       = 0x31,
  TABLE      = 0x10,
  CELLS      = 0x63,
  FONT       = 0x6C,
  IMAGE_2K, //these don't exist in Pub 2k3 so their value in the enum is not used.
  IMAGE_2K_DATA
};
} // namespace libmspub

#endif /* INCLUDED_MSPUBCHUNKTYPE_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
