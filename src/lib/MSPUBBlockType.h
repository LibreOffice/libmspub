/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __MSPUBBLOCKTYPE_H__
#define __MSPUBBLOCKTYPE_H__

namespace libmspub
{
enum MSPUBBlockType //Please fill this in and replace magic constants in MSPUBParser accordingly as you discover what a given block type means!
{
  TRAILER_DIRECTORY = 0x90,
  GENERAL_CONTAINER = 0x88,
  STRING_CONTAINER  = 0xc0,
  DUMMY             = 0x78,
  SHAPE_SEQNUM      = 0x70
};
} // namespace libmspub

#endif /* __MSPUBBLOCKTYPE_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
