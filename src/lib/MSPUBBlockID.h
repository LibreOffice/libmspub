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

#ifndef __MSPUBBLOCKID_H__
#define __MSPUBBLOCKID_H__

namespace libmspub
{
enum MSPUBBlockID // Don't be alarmed by multiple elements with the same value; they appear in different contexts.
{
  DOCUMENT_SIZE = 0x12,
  DOCUMENT_WIDTH = 0x1,
  DOCUMENT_HEIGHT = 0x2,
  PAGE_SHAPES = 0x2,
  CHUNK_TYPE = 0x2,
  CHUNK_OFFSET = 0x4,
  CHUNK_PARENT_SEQNUM = 0x5,
  SHAPE_WIDTH = 0xaa,
  SHAPE_HEIGHT = 0xab,
  SHAPE_TEXT_ID = 0x27,
  BOLD_1_ID = 0x02,
  BOLD_2_ID = 0x37,
  ITALIC_1_ID = 0x03,
  ITALIC_2_ID = 0x38,
  UNDERLINE_ID = 0x1E,
  TEXT_SIZE_1_ID = 0x0C,
  TEXT_SIZE_2_ID = 0x39
};
} // namespace libmspub

#endif /* __MSPUBBLOCKID_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
