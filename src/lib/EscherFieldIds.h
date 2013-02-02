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

#ifndef __ESCHERFIELDIDS_H__
#define __ESCHERFIELDIDS_H__

#define FIELDID_XS                     0x2001
#define FIELDID_YS                     0x2002
#define FIELDID_XE                     0x2003
#define FIELDID_YE                     0x2004
#define FIELDID_PXID                   0x4104
#define FIELDID_BG_PXID                0x4186
#define FIELDID_SHAPE_ID               0x6801
#define FIELDID_FILL_COLOR             0x0181
#define FIELDID_FILL_OPACITY           0x0182
#define FIELDID_FILL_BACK_COLOR        0x0183
#define FIELDID_FILL_BACK_OPACITY      0x0184
#define FIELDID_FILL_STYLE_BOOL_PROPS  0x01BF
#define FIELDID_LINE_COLOR             0x01C0
#define FIELDID_LINE_BACK_COLOR        0x01C2
#define FIELDID_LINE_STYLE_BOOL_PROPS  0x01FF
#define FIELDID_GEOM_BOOL_PROPS        0x017F
#define FIELDID_FILL_TYPE              0x0180
#define FIELDID_FILL_ANGLE             0x018B
#define FIELDID_FILL_FOCUS             0x018C
#define FIELDID_FIELD_STYLE_BOOL_PROPS 0x01BF
#define FIELDID_ADJUST_VALUE_1         0x0147
#define FIELDID_ADJUST_VALUE_2         0x0148
#define FIELDID_ADJUST_VALUE_3         0x0149
#define FIELDID_ROTATION               0x0004
#define FIELDID_DY_TEXT_LEFT           0x0081
#define FIELDID_DY_TEXT_TOP            0x0082
#define FIELDID_DY_TEXT_RIGHT          0x0083
#define FIELDID_DY_TEXT_BOTTOM         0x0084
#define FIELDID_LINE_WIDTH             0x01CB
#define FIELDID_LINE_TOP_COLOR         0x0580
#define FIELDID_LINE_TOP_WIDTH         0x058B
#define FIELDID_LINE_TOP_BOOL_PROPS    0x05BF
#define FIELDID_LINE_RIGHT_COLOR       0x05C0
#define FIELDID_LINE_RIGHT_WIDTH       0x05CB
#define FIELDID_LINE_RIGHT_BOOL_PROPS  0x05FF
#define FIELDID_LINE_BOTTOM_COLOR      0x0600
#define FIELDID_LINE_BOTTOM_WIDTH      0x060B
#define FIELDID_LINE_BOTTOM_BOOL_PROPS 0x063F
#define FIELDID_LINE_LEFT_COLOR        0x0540
#define FIELDID_LINE_LEFT_WIDTH        0x054B
#define FIELDID_LINE_LEFT_BOOL_PROPS   0x057F
#define FIELDID_GEO_RIGHT              0x0142
#define FIELDID_GEO_BOTTOM             0x0143
#define FIELDID_P_VERTICES             0xC145
#define FIELDID_P_SEGMENTS             0xC146
#define FIELDID_P_CONNECTION_SITES     0xC151
#define FIELDID_P_ADJUST_HANDLES       0xC155
#define FIELDID_P_GUIDES               0xC156
#define FIELDID_P_INSCRIBE             0xC157
#define FIELDID_LINE_DASHING           0x01CE
#define FIELDID_LINE_ENDCAP_STYLE      0x01D7
#define FIELDID_NUM_COLUMNS            0x008C
#define FIELDID_COLUMN_SPACING         0x008D
#define FIELDID_BEGIN_ARROW_STYLE      0x01D0
#define FIELDID_BEGIN_ARROW_WIDTH      0x01D2
#define FIELDID_BEGIN_ARROW_HEIGHT     0x01D3
#define FIELDID_END_ARROW_STYLE        0x01D1
#define FIELDID_END_ARROW_WIDTH        0x01D4
#define FIELDID_END_ARROW_HEIGHT       0x01D5
#define FIELDID_PICTURE_RECOLOR        0x011A
#define FIELDID_SHADOW_TYPE            0x0200
#define FIELDID_SHADOW_COLOR           0x0201
#define FIELDID_SHADOW_OPACITY         0x0204
#define FIELDID_SHADOW_OFFSET_X        0x0205
#define FIELDID_SHADOW_OFFSET_Y        0x0206
#define FIELDID_SHADOW_ORIGIN_X        0x0210
#define FIELDID_SHADOW_ORIGIN_Y        0x0211
#define FIELDID_SHADOW_BOOL_PROPS      0x023F

#define FLAG_USE_LINE   (1 << 19)
#define FLAG_LINE       (1 << 3)
#define FLAG_USE_LEFT_INSET_PEN (1 << 22)
#define FLAG_USE_LEFT_INSET_PEN_OK (1 << 21)
#define FLAG_LEFT_INSET_PEN (1 << 6)
#define FLAG_LEFT_INSET_PEN_OK (1 << 5)
#define FLAG_GEOM_USE_LINE_OK (1 << 12)
#define FLAG_GEOM_LINE_OK (1 << 28)

#endif /* __ESCHERFIELDIDS_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
