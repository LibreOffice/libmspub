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

#ifndef __SHAPETYPE_H__
#define __SHAPETYPE_H__

namespace libmspub
{
enum ShapeType
{
  RECTANGLE            = 1,
  ROUND_RECTANGLE      = 2,
  ELLIPSE              = 3,
  DIAMOND              = 4,
  ISOCELES_TRIANGLE    = 5,
  RIGHT_TRIANGLE       = 6,
  PARALLELOGRAM        = 7,
  TRAPEZOID            = 8,
  HEXAGON              = 9,
  OCTAGON              = 10,
  PLUS                 = 11,
  STAR                 = 12,
  ARROW                = 13,
  HOME_PLATE           = 15,
  CUBE                 = 16,
  BALLOON              = 17,
  ARC_SHAPE            = 19,
  PLAQUE               = 21,
  CAN                  = 22,
  DONUT                = 23,
  TEXT_SIMPLE          = 24,
  STRAIGHT_CONNECTOR_1 = 32,
  BENT_CONNECTOR_2     = 33,
  BENT_CONNECTOR_3     = 34,
  BENT_CONNECTOR_4     = 35,
  BENT_CONNECTOR_5     = 36,
  CURVED_CONNECTOR_2   = 37,
  CURVED_CONNECTOR_3   = 38,
  CURVED_CONNECTOR_4   = 39,
  CURVED_CONNECTOR_5   = 40,
  CALLOUT_1            = 41,
  CALLOUT_2            = 42,
  CALLOUT_3            = 43,
  RIBBON               = 53,
  RIBBON_2             = 54,
  CHEVRON              = 55,
  PENTAGON             = 56,
  NO_SMOKING           = 57,
  SEAL_8               = 58,
  SEAL_16              = 59,
  SEAL_32              = 60,
  WAVE                 = 64,
  FOLDED_CORNER        = 65,
  LEFT_ARROW           = 66,
  DOWN_ARROW           = 67,
  UP_ARROW             = 68,
  LEFT_RIGHT_ARROW     = 69,
  UP_DOWN_ARROW        = 70,
  IRREGULAR_SEAL_1     = 71,
  IRREGULAR_SEAL_2     = 72,
  LIGHTNING_BOLT       = 73,
  HEART                = 74,
  PICTURE_FRAME        = 75,
  QUAD_ARROW           = 76,
  BEVEL                = 84,
  LEFT_BRACKET         = 85,
  RIGHT_BRACKET        = 86,
  LEFT_BRACE           = 87,
  RIGHT_BRACE          = 88,
  LEFT_UP_ARROW        = 89,
  BENT_UP_ARROW        = 90,
  BENT_ARROW           = 91,
  SEAL_24              = 92,
  STRIPED_RIGHT_ARROW  = 93,
  NOTCHED_RIGHT_ARROW  = 94,
  BLOCK_ARC            = 95,
  SMILEY_FACE          = 96,
  VERTICAL_SCROLL      = 97,
  HORIZONTAL_SCROLL    = 98,
  CIRCULAR_ARROW       = 99,
  U_TURN_ARROW         = 101,
  CURVED_RIGHT_ARROW   = 102,
  CURVED_LEFT_ARROW    = 103,
  CURVED_UP_ARROW      = 104,
  CURVED_DOWN_ARROW    = 105,
  FLOW_CHART_PROCESS   = 109,
  FLOW_CHART_DECISION  = 110,
  FLOW_CHART_IO        = 111,
  FLOW_CHART_PREDEFINED_PROCESS = 112,
  FLOW_CHART_INTERNAL_STORAGE   = 113,
  FLOW_CHART_DOCUMENT  = 114,
  FLOW_CHART_MULTI_DOCUMENT     = 115,
  FLOW_CHART_TERMINATOR         = 116,
  FLOW_CHART_PREPARATION        = 117,
  FLOW_CHART_MANUAL_INPUT       = 118,
  FLOW_CHART_MANUAL_OPERATION   = 119,
  FLOW_CHART_CONNECTOR          = 120,
  FLOW_CHART_PUNCHED_CARD       = 121,
  FLOW_CHART_PUNCHED_TAPE       = 122,
  FLOW_CHART_SUMMING_JUNCTION   = 123,
  FLOW_CHART_OR        = 124,
  FLOW_CHART_COLLATE   = 125,
  FLOW_CHART_SORT      = 126,
  FLOW_CHART_EXTRACT   = 127,
  FLOW_CHART_MERGE     = 128,
  FLOW_CHART_ONLINE_STORAGE     = 130,
  FLOW_CHART_MAGNETIC_TAPE      = 131,
  FLOW_CHART_MAGNETIC_DISK      = 132,
  FLOW_CHART_MAGNETIC_DRUM      = 133,
  FLOW_CHART_DISPLAY  = 134,
  FLOW_CHART_DELAY    = 135,
  TEXT_PLAIN_TEXT     = 136,
  TEXT_STOP           = 137,
  TEXT_TRIANGLE       = 138,
  TEXT_TRIANGLE_INVERTED        = 139,
  SEAL_4               = 0xBB
};
} // libmspub
#endif /* __SHAPETYPE_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
