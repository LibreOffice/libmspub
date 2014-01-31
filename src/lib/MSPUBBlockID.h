/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
  DOCUMENT_PAGE_LIST = 0x2,
  PAGE_BG_SHAPE = 0xa,
  PAGE_SHAPES = 0x2,
  CHUNK_TYPE = 0x2,
  CHUNK_OFFSET = 0x4,
  CHUNK_PARENT_SEQNUM = 0x5,
  SHAPE_CROP = 0xb7,
  SHAPE_WIDTH = 0xaa,
  SHAPE_HEIGHT = 0xab,
  SHAPE_DONT_STRETCH_BA = 0x07,
  SHAPE_TEXT_ID = 0x27,
  SHAPE_BORDER_IMAGE_ID = 0x09,
  SUPER_SUB_TYPE_ID = 0x0F,
  BOLD_1_ID = 0x02,
  BOLD_2_ID = 0x37,
  ITALIC_1_ID = 0x03,
  ITALIC_2_ID = 0x38,
  UNDERLINE_ID = 0x1E,
  TEXT_SIZE_1_ID = 0x0C,
  TEXT_SIZE_2_ID = 0x39,
  COLOR_INDEX_CONTAINER_ID = 0x44,
  BARE_COLOR_INDEX_ID = 0x2E,
  FONT_INDEX_CONTAINER_ID = 0x24,
  COLOR_INDEX_ID = 0x0,
  PARAGRAPH_ALIGNMENT = 0x04,
  PARAGRAPH_DEFAULT_CHAR_STYLE = 0x19,
  PARAGRAPH_LINE_SPACING = 0x34,
  PARAGRAPH_SPACE_BEFORE = 0x12,
  PARAGRAPH_SPACE_AFTER  = 0x13,
  PARAGRAPH_FIRST_LINE_INDENT = 0xC,
  PARAGRAPH_LEFT_INDENT = 0xD,
  PARAGRAPH_RIGHT_INDENT = 0xE,
  PARAGRAPH_DROP_CAP_LINES = 0x8,
  PARAGRAPH_DROP_CAP_UP = 0x2C,
  PARAGRAPH_DROP_CAP_LETTERS = 0x2D,
  THIS_MASTER_NAME = 0xE,
  APPLIED_MASTER_NAME = 0xD,
  BA_ARRAY = 0x02,
  BA_IMAGE_ARRAY = 0x0A,
  BA_IMAGE_CONTAINER = 0x00,
  BA_IMAGE = 0x01,
  BA_OFFSET_CONTAINER = 0x08,
  BA_OFFSET_ENTRY = 0x00,
  PARAGRAPH_LIST_NUMBER_RESTART = 0x15,
  PARAGRAPH_LIST_INFO = 0x57,
  PARAGRAPH_LIST_BULLET_CHAR = 0x01,
  PARAGRAPH_LIST_NUMBERING_TYPE = 0x00,
  PARAGRAPH_LIST_NUMBERING_DELIMITER = 0x58,
  SHAPE_VALIGN = 0x35,
  TABLE_WIDTH = 0x68,
  TABLE_HEIGHT = 0x69,
  TABLE_CELLS_SEQNUM = 0x6B,
  TABLE_NUM_ROWS = 0x66,
  TABLE_NUM_COLS = 0x67,
  TABLE_ROWCOL_ARRAY = 0x6D,
  TABLE_ROWCOL_OFFSET = 0x01,
  FONT_CONTAINER_ARRAY = 0x02,
  EMBEDDED_EOT = 0x0C,
  EMBEDDED_FONT_NAME = 0x04,
  PARAGRAPH_TABS = 0x32,
  TAB_ARRAY = 0x28,
  TAB_ENTRY = 0x0,
  TAB_AMOUNT = 0x0
};
} // namespace libmspub

#endif /* __MSPUBBLOCKID_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
