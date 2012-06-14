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

#include <cstddef> // for NULL
#include <cmath>

#include <libwpg/libwpg.h>

#include "ShapeType.h"
#include "PolygonUtils.h"
#include "libmspub_utils.h"
#include "MSPUBCollector.h"

#define CALCULATED_VALUE | 0x80000000

#define PI 3.14159265

using namespace libmspub;

const Vertex WAVE_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short WAVE_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2001, 0x6000, 0x8000
};

const Calculation WAVE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 15800, 0x400, 4460), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 7200, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 7200), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c)
};

const TextRectangle WAVE_TRS[] =
{
  TextRectangle(Vertex(5 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 23 CALCULATED_VALUE))
};

const int WAVE_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const Vertex WAVE_GLUE_POINTS[] =
{
  Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 10800), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 10800)
};

const CustomShape CS_WAVE(
  WAVE_VERTICES, sizeof(WAVE_VERTICES) / sizeof(Vertex),
  WAVE_SEGMENTS, sizeof(WAVE_SEGMENTS) / sizeof(unsigned short),
  WAVE_CALC, sizeof(WAVE_CALC) / sizeof(Calculation),
  WAVE_DEFAULT_ADJUST, sizeof(WAVE_DEFAULT_ADJUST) / sizeof(int),
  WAVE_TRS, sizeof(WAVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  WAVE_GLUE_POINTS, sizeof(WAVE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FOLDED_CORNER_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 21600), Vertex(0 CALCULATED_VALUE, 21600), Vertex(3 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(21600, 0 CALCULATED_VALUE)
};

const unsigned short FOLDED_CORNER_SEGMENTS[] =
{
  0x4000, 0x0004, 0x6001, 0x8000, 0x4000, 0x0001, 0x2001, 0x6001, 0x8000
};

const Calculation FOLDED_CORNER_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2001, 0x0401, 8000, 10800), Calculation(0x8000, 21600, 0, 0x0402), Calculation(0x2001, 0x0401, 1, 2), Calculation(0x2001, 0x0401, 1, 4), Calculation(0x2001, 0x0401, 1, 7), Calculation(0x2001, 0x0401, 1, 16), Calculation(0x6000, 0x0403, 0x405, 0), Calculation(0x6000, 0x0400, 0x406, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6000, 0x400, 0x407, 0)
};

const TextRectangle FOLDED_CORNER_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 11 CALCULATED_VALUE))
};

const int FOLDED_CORNER_DEFAULT_ADJUST[] =
{
  18900
};

const Vertex FOLDED_CORNER_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FOLDED_CORNER(
  FOLDED_CORNER_VERTICES, sizeof(FOLDED_CORNER_VERTICES) / sizeof(Vertex),
  FOLDED_CORNER_SEGMENTS, sizeof(FOLDED_CORNER_SEGMENTS) / sizeof(unsigned short),
  FOLDED_CORNER_CALC, sizeof(FOLDED_CORNER_CALC) / sizeof(Calculation),
  FOLDED_CORNER_DEFAULT_ADJUST, sizeof(FOLDED_CORNER_DEFAULT_ADJUST) / sizeof(int),
  FOLDED_CORNER_TRS, sizeof(FOLDED_CORNER_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FOLDED_CORNER_GLUE_POINTS, sizeof(FOLDED_CORNER_GLUE_POINTS) / sizeof(Vertex));


const Vertex LEFT_ARROW_VERTICES[] =
{
  Vertex(21600, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0), Vertex(0, 10800), Vertex(1 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 2 CALCULATED_VALUE)
};

const unsigned short LEFT_ARROW_SEGMENTS[] =
{
  0x4000, 0x0006, 0x6001, 0x8000
};

const Calculation LEFT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x6001, 0x403, 0x400, 10800), Calculation(0x6000, 0x401, 0x404, 0), Calculation(0x6001, 0x401, 0x400, 10800), Calculation(0xa000, 0x401, 0, 0x406)
};

const TextRectangle LEFT_ARROW_TRS[] =
{
  TextRectangle(Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 2 CALCULATED_VALUE))
};

const int LEFT_ARROW_DEFAULT_ADJUST[] =
{
  5400, 5400
};

const CustomShape CS_LEFT_ARROW(
  LEFT_ARROW_VERTICES, sizeof(LEFT_ARROW_VERTICES) / sizeof(Vertex),
  LEFT_ARROW_SEGMENTS, sizeof(LEFT_ARROW_SEGMENTS) / sizeof(unsigned short),
  LEFT_ARROW_CALC, sizeof(LEFT_ARROW_CALC) / sizeof(Calculation),
  LEFT_ARROW_DEFAULT_ADJUST, sizeof(LEFT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  LEFT_ARROW_TRS, sizeof(LEFT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex DOWN_ARROW_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(21600, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0)
};

const unsigned short DOWN_ARROW_SEGMENTS[] =
{
  0x4000, 0x0006, 0x6001, 0x8000
};

const Calculation DOWN_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x6001, 0x403, 0x400, 10800), Calculation(0x6000, 0x401, 0x404, 0), Calculation(0x6001, 0x401, 0x400, 10800), Calculation(0xa000, 0x401, 0, 0x406)
};

const TextRectangle DOWN_ARROW_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0), Vertex(2 CALCULATED_VALUE, 5 CALCULATED_VALUE))
};

const int DOWN_ARROW_DEFAULT_ADJUST[] =
{
  16200, 5400
};

const CustomShape CS_DOWN_ARROW(
  DOWN_ARROW_VERTICES, sizeof(DOWN_ARROW_VERTICES) / sizeof(Vertex),
  DOWN_ARROW_SEGMENTS, sizeof(DOWN_ARROW_SEGMENTS) / sizeof(unsigned short),
  DOWN_ARROW_CALC, sizeof(DOWN_ARROW_CALC) / sizeof(Calculation),
  DOWN_ARROW_DEFAULT_ADJUST, sizeof(DOWN_ARROW_DEFAULT_ADJUST) / sizeof(int),
  DOWN_ARROW_TRS, sizeof(DOWN_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex UP_ARROW_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(10800, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 21600)
};

const unsigned short UP_ARROW_SEGMENTS[] =
{
  0x4000, 0x0006, 0x6001, 0x8000
};

const Calculation UP_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x6001, 0x403, 0x400, 10800), Calculation(0x6000, 0x401, 0x404, 0), Calculation(0x6001, 0x401, 0x400, 10800), Calculation(0xa000, 0x401, 0, 0x406)
};

const TextRectangle UP_ARROW_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 21600))
};

const int UP_ARROW_DEFAULT_ADJUST[] =
{
  5400, 5400
};

const CustomShape CS_UP_ARROW(
  UP_ARROW_VERTICES, sizeof(UP_ARROW_VERTICES) / sizeof(Vertex),
  UP_ARROW_SEGMENTS, sizeof(UP_ARROW_SEGMENTS) / sizeof(unsigned short),
  UP_ARROW_CALC, sizeof(UP_ARROW_CALC) / sizeof(Calculation),
  UP_ARROW_DEFAULT_ADJUST, sizeof(UP_ARROW_DEFAULT_ADJUST) / sizeof(int),
  UP_ARROW_TRS, sizeof(UP_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex SEAL_32_VERTICES[] =
{
  Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE), Vertex(0x07 CALCULATED_VALUE, 0x08 CALCULATED_VALUE), Vertex(0x09 CALCULATED_VALUE, 0x0a CALCULATED_VALUE), Vertex(0x0b CALCULATED_VALUE, 0x0c CALCULATED_VALUE), Vertex(0x0d CALCULATED_VALUE, 0x0e CALCULATED_VALUE), Vertex(0x0f CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x11 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x17 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x19 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1b CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x1d CALCULATED_VALUE, 0x1e CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x23 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x25 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x27 CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0x29 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2b CALCULATED_VALUE, 0x2c CALCULATED_VALUE), Vertex(0x2d CALCULATED_VALUE, 0x2e CALCULATED_VALUE), Vertex(0x2f CALCULATED_VALUE, 0x30 CALCULATED_VALUE), Vertex(0x31 CALCULATED_VALUE, 0x32 CALCULATED_VALUE), Vertex(0x33 CALCULATED_VALUE, 0x34 CALCULATED_VALUE), Vertex(0x35 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0x37 CALCULATED_VALUE, 0x38 CALCULATED_VALUE), Vertex(0x39 CALCULATED_VALUE, 0x3a CALCULATED_VALUE), Vertex(0x3b CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(0x3d CALCULATED_VALUE, 0x3e CALCULATED_VALUE), Vertex(0x3f CALCULATED_VALUE, 0x40 CALCULATED_VALUE), Vertex(0x41 CALCULATED_VALUE, 0x42 CALCULATED_VALUE), Vertex(0x43 CALCULATED_VALUE, 0x44 CALCULATED_VALUE), Vertex(0x45 CALCULATED_VALUE, 0x46 CALCULATED_VALUE), Vertex(0x47 CALCULATED_VALUE, 0x48 CALCULATED_VALUE), Vertex(0x49 CALCULATED_VALUE, 0x4a CALCULATED_VALUE), Vertex(0x4b CALCULATED_VALUE, 0x4c CALCULATED_VALUE), Vertex(0x4d CALCULATED_VALUE, 0x4e CALCULATED_VALUE), Vertex(0x4f CALCULATED_VALUE, 0x50 CALCULATED_VALUE), Vertex(0x51 CALCULATED_VALUE, 0x52 CALCULATED_VALUE), Vertex(0x53 CALCULATED_VALUE, 0x54 CALCULATED_VALUE), Vertex(0x55 CALCULATED_VALUE, 0x56 CALCULATED_VALUE), Vertex(0x57 CALCULATED_VALUE, 0x58 CALCULATED_VALUE), Vertex(0x59 CALCULATED_VALUE, 0x5a CALCULATED_VALUE), Vertex(0x5b CALCULATED_VALUE, 0x5c CALCULATED_VALUE), Vertex(0x5d CALCULATED_VALUE, 0x5e CALCULATED_VALUE), Vertex(0x5f CALCULATED_VALUE, 0x60 CALCULATED_VALUE), Vertex(0x61 CALCULATED_VALUE, 0x62 CALCULATED_VALUE), Vertex(0x63 CALCULATED_VALUE, 0x64 CALCULATED_VALUE), Vertex(0x65 CALCULATED_VALUE, 0x66 CALCULATED_VALUE), Vertex(0x67 CALCULATED_VALUE, 0x68 CALCULATED_VALUE), Vertex(0x69 CALCULATED_VALUE, 0x6a CALCULATED_VALUE), Vertex(0x6b CALCULATED_VALUE, 0x6c CALCULATED_VALUE), Vertex(0x6d CALCULATED_VALUE, 0x6e CALCULATED_VALUE), Vertex(0x6f CALCULATED_VALUE, 0x70 CALCULATED_VALUE), Vertex(0x71 CALCULATED_VALUE, 0x72 CALCULATED_VALUE), Vertex(0x73 CALCULATED_VALUE, 0x74 CALCULATED_VALUE), Vertex(0x75 CALCULATED_VALUE, 0x76 CALCULATED_VALUE), Vertex(0x77 CALCULATED_VALUE, 0x78 CALCULATED_VALUE), Vertex(0x79 CALCULATED_VALUE, 0x7a CALCULATED_VALUE), Vertex(0x7b CALCULATED_VALUE, 0x7c CALCULATED_VALUE), Vertex(0x7d CALCULATED_VALUE, 0x7e CALCULATED_VALUE), Vertex(0x7f CALCULATED_VALUE, 0x80 CALCULATED_VALUE), Vertex(0x81 CALCULATED_VALUE, 0x82 CALCULATED_VALUE), Vertex(0x83 CALCULATED_VALUE, 0x84 CALCULATED_VALUE), Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE)
};

const Calculation SEAL_32_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2081, 0x400, 10800, 315), Calculation(0x2082, 0x400, 10800, 315), Calculation(0x2081, 0x400, 10800, 135), Calculation(0x2082, 0x400, 10800, 135), Calculation(0x0081, 0, 10800, 0), Calculation(0x0082, 0, 10800, 0), Calculation(0x2081, 0x400, 10800, 5), Calculation(0x2082, 0x400, 10800, 5), Calculation(0x0081, 0, 10800, 11), Calculation(0x0082, 0, 10800, 11), Calculation(0x2081, 0x400, 10800, 16), Calculation(0x2082, 0x400, 10800, 16), Calculation(0x0081, 0, 10800, 22), Calculation(0x0082, 0, 10800, 22), Calculation(0x2081, 0x400, 10800, 28), Calculation(0x2082, 0x400, 10800, 28), Calculation(0x0081, 0, 10800, 33), Calculation(0x0082, 0, 10800, 33), Calculation(0x2081, 0x400, 10800, 39), Calculation(0x2082, 0x400, 10800, 39), Calculation(0x0081, 0, 10800, 45), Calculation(0x0082, 0, 10800, 45), Calculation(0x2081, 0x400, 10800, 50), Calculation(0x2082, 0x400, 10800, 50), Calculation(0x0081, 0, 10800, 56), Calculation(0x0082, 0, 10800, 56), Calculation(0x2081, 0x400, 10800, 61), Calculation(0x2082, 0x400, 10800, 61), Calculation(0x0081, 0, 10800, 67), Calculation(0x0082, 0, 10800, 67), Calculation(0x2081, 0x400, 10800, 73), Calculation(0x2082, 0x400, 10800, 73), Calculation(0x0081, 0, 10800, 78), Calculation(0x0082, 0, 10800, 78), Calculation(0x2081, 0x400, 10800, 84), Calculation(0x2082, 0x400, 10800, 84), Calculation(0x0081, 0, 10800, 90), Calculation(0x0082, 0, 10800, 90), Calculation(0x2081, 0x400, 10800, 95), Calculation(0x2082, 0x400, 10800, 95), Calculation(0x0081, 0, 10800, 101), Calculation(0x0082, 0, 10800, 101), Calculation(0x2081, 0x400, 10800, 106), Calculation(0x2082, 0x400, 10800, 106), Calculation(0x0081, 0, 10800, 112), Calculation(0x0082, 0, 10800, 112), Calculation(0x2081, 0x400, 10800, 118), Calculation(0x2082, 0x400, 10800, 118), Calculation(0x0081, 0, 10800, 123), Calculation(0x0082, 0, 10800, 123), Calculation(0x2081, 0x400, 10800, 129), Calculation(0x2082, 0x400, 10800, 129), Calculation(0x0081, 0, 10800, 135), Calculation(0x0082, 0, 10800, 135), Calculation(0x2081, 0x400, 10800, 140), Calculation(0x2082, 0x400, 10800, 140), Calculation(0x0081, 0, 10800, 146), Calculation(0x0082, 0, 10800, 146), Calculation(0x2081, 0x400, 10800, 151), Calculation(0x2082, 0x400, 10800, 151), Calculation(0x0081, 0, 10800, 157), Calculation(0x0082, 0, 10800, 157), Calculation(0x2081, 0x400, 10800, 163), Calculation(0x2082, 0x400, 10800, 163), Calculation(0x0081, 0, 10800, 168), Calculation(0x0082, 0, 10800, 168), Calculation(0x2081, 0x400, 10800, 174), Calculation(0x2082, 0x400, 10800, 174), Calculation(0x0081, 0, 10800, 180), Calculation(0x0082, 0, 10800, 180), Calculation(0x2081, 0x400, 10800, 185), Calculation(0x2082, 0x400, 10800, 185), Calculation(0x0081, 0, 10800, 191), Calculation(0x0082, 0, 10800, 191), Calculation(0x2081, 0x400, 10800, 196), Calculation(0x2082, 0x400, 10800, 196), Calculation(0x0081, 0, 10800, 202), Calculation(0x0082, 0, 10800, 202), Calculation(0x2081, 0x400, 10800, 208), Calculation(0x2082, 0x400, 10800, 208), Calculation(0x0081, 0, 10800, 213), Calculation(0x0082, 0, 10800, 213), Calculation(0x2081, 0x400, 10800, 219), Calculation(0x2082, 0x400, 10800, 219), Calculation(0x0081, 0, 10800, 225), Calculation(0x0082, 0, 10800, 225), Calculation(0x2081, 0x400, 10800, 230), Calculation(0x2082, 0x400, 10800, 230), Calculation(0x0081, 0, 10800, 236), Calculation(0x0082, 0, 10800, 236), Calculation(0x2081, 0x400, 10800, 241), Calculation(0x2082, 0x400, 10800, 241), Calculation(0x0081, 0, 10800, 247), Calculation(0x0082, 0, 10800, 247), Calculation(0x2081, 0x400, 10800, 253), Calculation(0x2082, 0x400, 10800, 253), Calculation(0x0081, 0, 10800, 258), Calculation(0x0082, 0, 10800, 258), Calculation(0x2081, 0x400, 10800, 264), Calculation(0x2082, 0x400, 10800, 264), Calculation(0x0081, 0, 10800, 270), Calculation(0x0082, 0, 10800, 270), Calculation(0x2081, 0x400, 10800, 275), Calculation(0x2082, 0x400, 10800, 275), Calculation(0x0081, 0, 10800, 281), Calculation(0x0082, 0, 10800, 281), Calculation(0x2081, 0x400, 10800, 286), Calculation(0x2082, 0x400, 10800, 286), Calculation(0x0081, 0, 10800, 292), Calculation(0x0082, 0, 10800, 292), Calculation(0x2081, 0x400, 10800, 298), Calculation(0x2082, 0x400, 10800, 298), Calculation(0x0081, 0, 10800, 303), Calculation(0x0082, 0, 10800, 303), Calculation(0x2081, 0x400, 10800, 309), Calculation(0x2082, 0x400, 10800, 309), Calculation(0x0081, 0, 10800, 315), Calculation(0x0082, 0, 10800, 315), Calculation(0x2081, 0x400, 10800, 320), Calculation(0x2082, 0x400, 10800, 320), Calculation(0x0081, 0, 10800, 326), Calculation(0x0082, 0, 10800, 326), Calculation(0x2081, 0x400, 10800, 331), Calculation(0x2082, 0x400, 10800, 331), Calculation(0x0081, 0, 10800, 337), Calculation(0x0082, 0, 10800, 337), Calculation(0x2081, 0x400, 10800, 343), Calculation(0x2082, 0x400, 10800, 343), Calculation(0x0081, 0, 10800, 348), Calculation(0x0082, 0, 10800, 348), Calculation(0x2081, 0x400, 10800, 354), Calculation(0x2082, 0x400, 10800, 354)
};

const TextRectangle SEAL_32_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int SEAL_32_DEFAULT_ADJUST[] =
{
  2500
};

const CustomShape CS_SEAL_32(
  SEAL_32_VERTICES, sizeof(SEAL_32_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_32_CALC, sizeof(SEAL_32_CALC) / sizeof(Calculation),
  SEAL_32_DEFAULT_ADJUST, sizeof(SEAL_32_DEFAULT_ADJUST) / sizeof(int),
  SEAL_32_TRS, sizeof(SEAL_32_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex SEAL_16_VERTICES[] =
{
  Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE), Vertex(0x07 CALCULATED_VALUE, 0x08 CALCULATED_VALUE), Vertex(0x09 CALCULATED_VALUE, 0x0a CALCULATED_VALUE), Vertex(0x0b CALCULATED_VALUE, 0x0c CALCULATED_VALUE), Vertex(0x0d CALCULATED_VALUE, 0x0e CALCULATED_VALUE), Vertex(0x0f CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x11 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x17 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x19 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1b CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x1d CALCULATED_VALUE, 0x1e CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x23 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x25 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x27 CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0x29 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2b CALCULATED_VALUE, 0x2c CALCULATED_VALUE), Vertex(0x2d CALCULATED_VALUE, 0x2e CALCULATED_VALUE), Vertex(0x2f CALCULATED_VALUE, 0x30 CALCULATED_VALUE), Vertex(0x31 CALCULATED_VALUE, 0x32 CALCULATED_VALUE), Vertex(0x33 CALCULATED_VALUE, 0x34 CALCULATED_VALUE), Vertex(0x35 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0x37 CALCULATED_VALUE, 0x38 CALCULATED_VALUE), Vertex(0x39 CALCULATED_VALUE, 0x3a CALCULATED_VALUE), Vertex(0x3b CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(0x3d CALCULATED_VALUE, 0x3e CALCULATED_VALUE), Vertex(0x3f CALCULATED_VALUE, 0x40 CALCULATED_VALUE), Vertex(0x41 CALCULATED_VALUE, 0x42 CALCULATED_VALUE), Vertex(0x43 CALCULATED_VALUE, 0x44 CALCULATED_VALUE), Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE)
};

const Calculation SEAL_16_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2081, 0x400, 10800, 315), Calculation(0x2082, 0x400, 10800, 315), Calculation(0x2081, 0x400, 10800, 135), Calculation(0x2082, 0x400, 10800, 135), Calculation(0x0081, 0, 10800, 0), Calculation(0x0082, 0, 10800, 0), Calculation(0x2081, 0x400, 10800, 11), Calculation(0x2082, 0x400, 10800, 11), Calculation(0x0081, 0, 10800, 22), Calculation(0x0082, 0, 10800, 22), Calculation(0x2081, 0x400, 10800, 33), Calculation(0x2082, 0x400, 10800, 33), Calculation(0x0081, 0, 10800, 45), Calculation(0x0082, 0, 10800, 45), Calculation(0x2081, 0x400, 10800, 56), Calculation(0x2082, 0x400, 10800, 56), Calculation(0x0081, 0, 10800, 67), Calculation(0x0082, 0, 10800, 67), Calculation(0x2081, 0x400, 10800, 78), Calculation(0x2082, 0x400, 10800, 78), Calculation(0x0081, 0, 10800, 90), Calculation(0x0082, 0, 10800, 90), Calculation(0x2081, 0x400, 10800, 101), Calculation(0x2082, 0x400, 10800, 101), Calculation(0x0081, 0, 10800, 112), Calculation(0x0082, 0, 10800, 112), Calculation(0x2081, 0x400, 10800, 123), Calculation(0x2082, 0x400, 10800, 123), Calculation(0x0081, 0, 10800, 135), Calculation(0x0082, 0, 10800, 135), Calculation(0x2081, 0x400, 10800, 146), Calculation(0x2082, 0x400, 10800, 146), Calculation(0x0081, 0, 10800, 157), Calculation(0x0082, 0, 10800, 157), Calculation(0x2081, 0x400, 10800, 168), Calculation(0x2082, 0x400, 10800, 168), Calculation(0x0081, 0, 10800, 180), Calculation(0x0082, 0, 10800, 180), Calculation(0x2081, 0x400, 10800, 191), Calculation(0x2082, 0x400, 10800, 191), Calculation(0x0081, 0, 10800, 202), Calculation(0x0082, 0, 10800, 202), Calculation(0x2081, 0x400, 10800, 213), Calculation(0x2082, 0x400, 10800, 213), Calculation(0x0081, 0, 10800, 225), Calculation(0x0082, 0, 10800, 225), Calculation(0x2081, 0x400, 10800, 236), Calculation(0x2082, 0x400, 10800, 236), Calculation(0x0081, 0, 10800, 247), Calculation(0x0082, 0, 10800, 247), Calculation(0x2081, 0x400, 10800, 258), Calculation(0x2082, 0x400, 10800, 258), Calculation(0x0081, 0, 10800, 270), Calculation(0x0082, 0, 10800, 270), Calculation(0x2081, 0x400, 10800, 281), Calculation(0x2082, 0x400, 10800, 281), Calculation(0x0081, 0, 10800, 292), Calculation(0x0082, 0, 10800, 292), Calculation(0x2081, 0x400, 10800, 303), Calculation(0x2082, 0x400, 10800, 303), Calculation(0x0081, 0, 10800, 315), Calculation(0x0082, 0, 10800, 315), Calculation(0x2081, 0x400, 10800, 326), Calculation(0x2082, 0x400, 10800, 326), Calculation(0x0081, 0, 10800, 337), Calculation(0x0082, 0, 10800, 337), Calculation(0x2081, 0x400, 10800, 348), Calculation(0x2082, 0x400, 10800, 348)
};

const TextRectangle SEAL_16_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int SEAL_16_DEFAULT_ADJUST[] =
{
  2500
};

const CustomShape CS_SEAL_16(
  SEAL_16_VERTICES, sizeof(SEAL_16_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_16_CALC, sizeof(SEAL_16_CALC) / sizeof(Calculation),
  SEAL_16_DEFAULT_ADJUST, sizeof(SEAL_16_DEFAULT_ADJUST) / sizeof(int),
  SEAL_16_TRS, sizeof(SEAL_16_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex NO_SMOKING_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 0xa CALCULATED_VALUE), Vertex(0xb CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0xd CALCULATED_VALUE, 0xe CALCULATED_VALUE), Vertex(0xf CALCULATED_VALUE, 0x10 CALCULATED_VALUE)
};

const unsigned short NO_SMOKING_SEGMENTS[] =
{
  0xa203, 0x6000, 0xa404, 0x6000, 0xa404, 0x6000, 0x8000
};

const Calculation NO_SMOKING_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0xa080, 0x403, 0, 0x402), Calculation(0x8000, 10800, 0, 0x403), Calculation(0x4000, 10800, 0x403, 0), Calculation(0x8000, 10800, 0, 0x404), Calculation(0x4000, 10800, 0x404, 0), Calculation(0x6081, 0x405, 0x407, 45), Calculation(0x6082, 0x405, 0x407, 45), Calculation(0x6081, 0x405, 0x408, 45), Calculation(0x6082, 0x405, 0x408, 45), Calculation(0x6081, 0x406, 0x408, 45), Calculation(0x6082, 0x406, 0x408, 45), Calculation(0x6081, 0x406, 0x407, 45), Calculation(0x6082, 0x406, 0x407, 45)
};

const TextRectangle NO_SMOKING_TRS[] =
{
  TextRectangle(Vertex(3163, 3163), Vertex(18437, 18437))
};

const int NO_SMOKING_DEFAULT_ADJUST[] =
{
  2700
};

const Vertex NO_SMOKING_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_NO_SMOKING(
  NO_SMOKING_VERTICES, sizeof(NO_SMOKING_VERTICES) / sizeof(Vertex),
  NO_SMOKING_SEGMENTS, sizeof(NO_SMOKING_SEGMENTS) / sizeof(unsigned short),
  NO_SMOKING_CALC, sizeof(NO_SMOKING_CALC) / sizeof(Calculation),
  NO_SMOKING_DEFAULT_ADJUST, sizeof(NO_SMOKING_DEFAULT_ADJUST) / sizeof(int),
  NO_SMOKING_TRS, sizeof(NO_SMOKING_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NO_SMOKING_GLUE_POINTS, sizeof(NO_SMOKING_GLUE_POINTS) / sizeof(Vertex));

const Vertex SEAL_8_VERTICES[] =
{
  Vertex(5 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 18 CALCULATED_VALUE), Vertex(23 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 30 CALCULATED_VALUE), Vertex(35 CALCULATED_VALUE, 36 CALCULATED_VALUE), Vertex(41 CALCULATED_VALUE, 42 CALCULATED_VALUE), Vertex(47 CALCULATED_VALUE, 48 CALCULATED_VALUE), Vertex(53 CALCULATED_VALUE, 54 CALCULATED_VALUE), Vertex(59 CALCULATED_VALUE, 60 CALCULATED_VALUE), Vertex(65 CALCULATED_VALUE, 66 CALCULATED_VALUE), Vertex(71 CALCULATED_VALUE, 72 CALCULATED_VALUE), Vertex(77 CALCULATED_VALUE, 78 CALCULATED_VALUE), Vertex(83 CALCULATED_VALUE, 84 CALCULATED_VALUE), Vertex(89 CALCULATED_VALUE, 90 CALCULATED_VALUE), Vertex(95 CALCULATED_VALUE, 96 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 6 CALCULATED_VALUE)
};

const Calculation SEAL_8_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2081, 0x400, 10800, 315), Calculation(0x2082, 0x400, 10800, 315), Calculation(0x2081, 0x400, 10800, 135), Calculation(0x2082, 0x400, 10800, 135), Calculation(0x0081, 0, 10800, 0), Calculation(0x0082, 0, 10800, 0), Calculation(0x2081, 0x400, 10800, 7), Calculation(0x2082, 0x400, 10800, 7), Calculation(0x0081, 0, 10800, 15), Calculation(0x0082, 0, 10800, 15), Calculation(0x2081, 0x400, 10800, 22), Calculation(0x2082, 0x400, 10800, 22), Calculation(0x0081, 0, 10800, 30), Calculation(0x0082, 0, 10800, 30), Calculation(0x2081, 0x400, 10800, 37), Calculation(0x2082, 0x400, 10800, 37), Calculation(0x0081, 0, 10800, 45), Calculation(0x0082, 0, 10800, 45), Calculation(0x2081, 0x400, 10800, 52), Calculation(0x2082, 0x400, 10800, 52), Calculation(0x0081, 0, 10800, 60), Calculation(0x0082, 0, 10800, 60), Calculation(0x2081, 0x400, 10800, 67), Calculation(0x2082, 0x400, 10800, 67), Calculation(0x0081, 0, 10800, 75), Calculation(0x0082, 0, 10800, 75), Calculation(0x2081, 0x400, 10800, 82), Calculation(0x2082, 0x400, 10800, 82), Calculation(0x0081, 0, 10800, 90), Calculation(0x0082, 0, 10800, 90), Calculation(0x2081, 0x400, 10800, 97), Calculation(0x2082, 0x400, 10800, 97), Calculation(0x0081, 0, 10800, 105), Calculation(0x0082, 0, 10800, 105), Calculation(0x2081, 0x400, 10800, 112), Calculation(0x2082, 0x400, 10800, 112), Calculation(0x0081, 0, 10800, 120), Calculation(0x0082, 0, 10800, 120), Calculation(0x2081, 0x400, 10800, 127), Calculation(0x2082, 0x400, 10800, 127), Calculation(0x0081, 0, 10800, 135), Calculation(0x0082, 0, 10800, 135), Calculation(0x2081, 0x400, 10800, 142), Calculation(0x2082, 0x400, 10800, 142), Calculation(0x0081, 0, 10800, 150), Calculation(0x0082, 0, 10800, 150), Calculation(0x2081, 0x400, 10800, 157), Calculation(0x2082, 0x400, 10800, 157), Calculation(0x0081, 0, 10800, 165), Calculation(0x0082, 0, 10800, 165), Calculation(0x2081, 0x400, 10800, 172), Calculation(0x2082, 0x400, 10800, 172), Calculation(0x0081, 0, 10800, 180), Calculation(0x0082, 0, 10800, 180), Calculation(0x2081, 0x400, 10800, 187), Calculation(0x2082, 0x400, 10800, 187), Calculation(0x0081, 0, 10800, 195), Calculation(0x0082, 0, 10800, 195), Calculation(0x2081, 0x400, 10800, 202), Calculation(0x2082, 0x400, 10800, 202), Calculation(0x0081, 0, 10800, 210), Calculation(0x0082, 0, 10800, 210), Calculation(0x2081, 0x400, 10800, 217), Calculation(0x2082, 0x400, 10800, 217), Calculation(0x0081, 0, 10800, 225), Calculation(0x0082, 0, 10800, 225), Calculation(0x2081, 0x400, 10800, 232), Calculation(0x2082, 0x400, 10800, 232), Calculation(0x0081, 0, 10800, 240), Calculation(0x0082, 0, 10800, 240), Calculation(0x2081, 0x400, 10800, 247), Calculation(0x2082, 0x400, 10800, 247), Calculation(0x0081, 0, 10800, 255), Calculation(0x0082, 0, 10800, 255), Calculation(0x2081, 0x400, 10800, 262), Calculation(0x2082, 0x400, 10800, 262), Calculation(0x0081, 0, 10800, 270), Calculation(0x0082, 0, 10800, 270), Calculation(0x2081, 0x400, 10800, 277), Calculation(0x2082, 0x400, 10800, 277), Calculation(0x0081, 0, 10800, 285), Calculation(0x0082, 0, 10800, 285), Calculation(0x2081, 0x400, 10800, 292), Calculation(0x2082, 0x400, 10800, 292), Calculation(0x0081, 0, 10800, 300), Calculation(0x0082, 0, 10800, 300), Calculation(0x2081, 0x400, 10800, 307), Calculation(0x2082, 0x400, 10800, 307), Calculation(0x0081, 0, 10800, 315), Calculation(0x0082, 0, 10800, 315), Calculation(0x2081, 0x400, 10800, 322), Calculation(0x2082, 0x400, 10800, 322), Calculation(0x0081, 0, 10800, 330), Calculation(0x0082, 0, 10800, 330), Calculation(0x2081, 0x400, 10800, 337), Calculation(0x2082, 0x400, 10800, 337), Calculation(0x0081, 0, 10800, 345), Calculation(0x0082, 0, 10800, 345), Calculation(0x2081, 0x400, 10800, 352), Calculation(0x2082, 0x400, 10800, 352)
};

const TextRectangle SEAL_8_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int SEAL_8_DEFAULT_ADJUST[] =
{
  2500
};

const CustomShape CS_SEAL_8(
  SEAL_8_VERTICES, sizeof(SEAL_8_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_8_CALC, sizeof(SEAL_8_CALC) / sizeof(Calculation),
  SEAL_8_DEFAULT_ADJUST, sizeof(SEAL_8_DEFAULT_ADJUST) / sizeof(int),
  SEAL_8_TRS, sizeof(SEAL_8_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex RIBBON_2_VERTICES[] =
{
  Vertex(12 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 21600), Vertex(16 CALCULATED_VALUE, 21600), Vertex(0, 21600), Vertex(2750, 7 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 0), Vertex(17 CALCULATED_VALUE, 0), Vertex(18 CALCULATED_VALUE, 0), Vertex(19 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 2 CALCULATED_VALUE), Vertex(18850, 7 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(20 CALCULATED_VALUE, 21600), Vertex(21 CALCULATED_VALUE, 21600), Vertex(22 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 25 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 27 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(20 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(18 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 25 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 27 CALCULATED_VALUE), Vertex(18 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 25 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 25 CALCULATED_VALUE), Vertex(19 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const unsigned short RIBBON_2_SEGMENTS[] =
{
  0x4000, 0x0001, 0x2001, 0x0005, 0x2001, 0x0001, 0x2001, 0x0005, 0x2001, 0x0001, 0x6001, 0x8000, 0x4000, 0x0001, 0x2001, 0x0001, 0x2002, 0x6001, 0x8000, 0x4000, 0x0001, 0x2001, 0x0001, 0x2002, 0x6001, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation RIBBON_2_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x2001, 0x402, 1, 2), Calculation(0x2001, 0x403, 1, 2), Calculation(0x2001, 0x404, 1, 2), Calculation(0x2001, 0x401, 1, 2), Calculation(0x8000, 21600, 0, 0x406), Calculation(0x0000, 420, 0, 0), Calculation(0x2001, 0x408, 2, 1), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x6000, 0x400, 0x409, 0), Calculation(0x2000, 0x400, 2700, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x8000, 21600, 0, 0x405), Calculation(0xa000, 0x40c, 0, 0x408), Calculation(0xa000, 0x40c, 0, 0x409), Calculation(0x8000, 21600, 0, 0x40b), Calculation(0x8000, 21600, 0, 0x40a), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x8000, 21600, 0, 0x410), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x8000, 21600, 0, 0x40c), Calculation(0xa000, 0x40d, 0, 0x405), Calculation(0x6000, 0x401, 0x403, 0), Calculation(0x6000, 0x401, 0x404, 0), Calculation(0x6000, 0x419, 0x405, 0), Calculation(0xa000, 0x419, 0, 0x405)
};

const TextRectangle RIBBON_2_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0), Vertex(19 CALCULATED_VALUE, 1 CALCULATED_VALUE))
};

const int RIBBON_2_DEFAULT_ADJUST[] =
{
  5400, 18900
};

const CustomShape CS_RIBBON_2(
  RIBBON_2_VERTICES, sizeof(RIBBON_2_VERTICES) / sizeof(Vertex),
  RIBBON_2_SEGMENTS, sizeof(RIBBON_2_SEGMENTS) / sizeof(unsigned short),
  RIBBON_2_CALC, sizeof(RIBBON_2_CALC) / sizeof(Calculation),
  RIBBON_2_DEFAULT_ADJUST, sizeof(RIBBON_2_DEFAULT_ADJUST) / sizeof(int),
  RIBBON_2_TRS, sizeof(RIBBON_2_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex CHEVRON_VERTICES[] =
{
  Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 10800)
};

const unsigned short CHEVRON_SEGMENTS[] =
{
  0x4000, 0x0005, 0x6001, 0x8000
};

const Calculation CHEVRON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x0400)
};

const TextRectangle CHEVRON_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int CHEVRON_DEFAULT_ADJUST[] =
{
  16200
};

const CustomShape CS_CHEVRON(
  CHEVRON_VERTICES, sizeof(CHEVRON_VERTICES) / sizeof(Vertex),
  CHEVRON_SEGMENTS, sizeof(CHEVRON_SEGMENTS) / sizeof(unsigned short),
  CHEVRON_CALC, sizeof(CHEVRON_CALC) / sizeof(Calculation),
  CHEVRON_DEFAULT_ADJUST, sizeof(CHEVRON_DEFAULT_ADJUST) / sizeof(int),
  CHEVRON_TRS, sizeof(CHEVRON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex PENTAGON_VERTICES[] =
{
  Vertex(10800, 0), Vertex(0, 8260), Vertex(4230, 21600), Vertex(17370, 21600), Vertex(21600, 8260), Vertex(10800, 0)
};

const TextRectangle PENTAGON_TRS[] =
{
  TextRectangle(Vertex(4230, 5080), Vertex(17370, 21600))
};

const Vertex PENTAGON_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 8260), Vertex(4230, 21600), Vertex(10800, 21600), Vertex(17370, 21600), Vertex(21600, 8260)
};

const CustomShape CS_PENTAGON(
  PENTAGON_VERTICES, sizeof(PENTAGON_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  PENTAGON_TRS, sizeof(PENTAGON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  PENTAGON_GLUE_POINTS, sizeof(PENTAGON_GLUE_POINTS) / sizeof(Vertex));

const Vertex RIBBON_VERTICES[] =
{
  Vertex(0, 0), Vertex(3 CALCULATED_VALUE, 0), Vertex(4 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 0), Vertex(21600, 0), Vertex(18 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(21600, 15 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(0, 15 CALCULATED_VALUE), Vertex(2700, 14 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 15 CALCULATED_VALUE)
};

const unsigned short RIBBON_SEGMENTS[] =
{
  0x4000, 0x0001, 0xa701, 0x0003, 0xa801, 0x0005, 0xa801, 0x0001, 0xa701, 0x0003, 0x6000, 0x8000, 0x4000, 0xaa00, 0xa801, 0x0001, 0xa702, 0x0001, 0x8000, 0x4000, 0xaa00, 0xa801, 0x0001, 0xa702, 0x0001, 0x8000, 0x4000, 0xaa00, 0x0001, 0x8000, 0x4000, 0xaa00, 0x0001, 0x8000
};

const Calculation RIBBON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, 0x400, 675, 0), Calculation(0x2000, 0x401, 675, 0), Calculation(0x2000, 0x402, 675, 0), Calculation(0x2000, 0x403, 675, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x8000, 21600, 0, 0x403), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2001, 0x40a, 1, 4), Calculation(0x2001, 0x40b, 2, 1), Calculation(0x2001, 0x40b, 3, 1), Calculation(0x8000, 10800, 0, 0x40c), Calculation(0x8000, 21600, 0, 0x40a), Calculation(0x8000, 21600, 0, 0x40b), Calculation(0x0001, 21600, 1, 2), Calculation(0x0000, 21600, 0, 2700), Calculation(0x2000, 0x411, 0, 2700)
};

const TextRectangle RIBBON_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 21600))
};

const int RIBBON_DEFAULT_ADJUST[] =
{
  5400, 2700
};

const Vertex RIBBON_GLUE_POINTS[] =
{
  Vertex(17 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(2700, 14 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 21600), Vertex(18 CALCULATED_VALUE, 14 CALCULATED_VALUE)
};

const CustomShape CS_RIBBON(
  RIBBON_VERTICES, sizeof(RIBBON_VERTICES) / sizeof(Vertex),
  RIBBON_SEGMENTS, sizeof(RIBBON_SEGMENTS) / sizeof(unsigned short),
  RIBBON_CALC, sizeof(RIBBON_CALC) / sizeof(Calculation),
  RIBBON_DEFAULT_ADJUST, sizeof(RIBBON_DEFAULT_ADJUST) / sizeof(int),
  RIBBON_TRS, sizeof(RIBBON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  RIBBON_GLUE_POINTS, sizeof(RIBBON_GLUE_POINTS) / sizeof(Vertex));

const Vertex CALLOUT_2_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 5 CALCULATED_VALUE)
};

const unsigned short CALLOUT_2_SEGMENTS[] =
{
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation CALLOUT_2_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 3, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 4, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 5, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 6, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 7, 0, 0)
};

const int CALLOUT_2_DEFAULT_ADJUST[] =
{
  -10000, 24500, -3600, 4000, -1800, 4000
};

const CustomShape CS_CALLOUT_2(
  CALLOUT_2_VERTICES, sizeof(CALLOUT_2_VERTICES) / sizeof(Vertex),
  CALLOUT_2_SEGMENTS, sizeof(CALLOUT_2_SEGMENTS) / sizeof(unsigned short),
  CALLOUT_2_CALC, sizeof(CALLOUT_2_CALC) / sizeof(Calculation),
  CALLOUT_2_DEFAULT_ADJUST, sizeof(CALLOUT_2_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CALLOUT_3_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(6 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short CALLOUT_3_SEGMENTS[] =
{
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE 0x4000, 0xaa00, 0x0003, 0x8000 // NO FILL
};

const Calculation CALLOUT_3_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 3, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 4, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 5, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 6, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 7, 0, 0)
};

const int CALLOUT_3_DEFAULT_ADJUST[] =
{
  23400, 24500, 25200, 21600, 25200, 4000, 23400, 4000
};

const CustomShape CS_CALLOUT_3(
  CALLOUT_3_VERTICES, sizeof(CALLOUT_3_VERTICES) / sizeof(Vertex),
  CALLOUT_3_SEGMENTS, sizeof(CALLOUT_3_SEGMENTS) / sizeof(unsigned short),
  CALLOUT_3_CALC, sizeof(CALLOUT_3_CALC) / sizeof(Calculation),
  CALLOUT_3_DEFAULT_ADJUST, sizeof(CALLOUT_3_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CALLOUT_1_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short CALLOUT_1_SEGMENTS[] =
{
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE 0x4000, 0x0001, 0x8000
};

const Calculation CALLOUT_1_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 3, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 4, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 5, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 6, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 7, 0, 0)
};

const int CALLOUT_1_DEFAULT_ADJUST[] =
{
  -8300, 24500, -1800, 4000
};

const CustomShape CS_CALLOUT_1(
  CALLOUT_1_VERTICES, sizeof(CALLOUT_1_VERTICES) / sizeof(Vertex),
  CALLOUT_1_SEGMENTS, sizeof(CALLOUT_1_SEGMENTS) / sizeof(unsigned short),
  CALLOUT_1_CALC, sizeof(CALLOUT_1_CALC) / sizeof(Calculation),
  CALLOUT_1_DEFAULT_ADJUST, sizeof(CALLOUT_1_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CURVED_CONNECTOR_5_VERTICES[] =
{
  Vertex(0, 0), Vertex(21 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 18 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 20 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 21600), Vertex(21600, 21600)
};

const unsigned short CURVED_CONNECTOR_5_SEGMENTS[] =
{
  0x4000, 0x2004, 0x8000
};

const Calculation CURVED_CONNECTOR_5_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x6000, 0x400, 0x401, 0), Calculation(0x2001, 0x402, 1, 2), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x6000, 0x400, 0x403, 0), Calculation(0x2001, 0x405, 1, 2), Calculation(0x6000, 0x401, 0x403, 0), Calculation(0x2001, 0x407, 1, 2), Calculation(0x2000, 0x401, 21600, 0), Calculation(0x2001, 0x409, 1, 2), Calculation(0x2001, 0x404, 1, 2), Calculation(0x2001, 0x40b, 1, 2), Calculation(0x6000, 0x404, 0x40b, 0), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x2000, 0x404, 21600, 0), Calculation(0x2001, 0x40f, 1, 2), Calculation(0x6000, 0x404, 0x410, 0), Calculation(0x2001, 0x411, 1, 2), Calculation(0x2000, 0x410, 21600, 0), Calculation(0x2001, 0x413, 1, 2), Calculation(0x2001, 0x400, 1, 2)
};

const int CURVED_CONNECTOR_5_DEFAULT_ADJUST[] =
{
  10800, 10800, 10800
};

const CustomShape CS_CURVED_CONNECTOR_5(
  CURVED_CONNECTOR_5_VERTICES, sizeof(CURVED_CONNECTOR_5_VERTICES) / sizeof(Vertex),
  CURVED_CONNECTOR_5_SEGMENTS, sizeof(CURVED_CONNECTOR_5_SEGMENTS) / sizeof(unsigned short),
  CURVED_CONNECTOR_5_CALC, sizeof(CURVED_CONNECTOR_5_CALC) / sizeof(Calculation),
  CURVED_CONNECTOR_5_DEFAULT_ADJUST, sizeof(CURVED_CONNECTOR_5_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CURVED_CONNECTOR_4_VERTICES[] =
{
  Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(21600, 14 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short CURVED_CONNECTOR_4_SEGMENTS[] =
{
  0x4000, 0x2003, 0x8000
};

const Calculation CURVED_CONNECTOR_4_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 1, 2), Calculation(0x4000, 21600, 0x400, 0), Calculation(0x2001, 0x402, 1, 2), Calculation(0x6000, 0x400, 0x403, 0), Calculation(0x2001, 0x404, 1, 2), Calculation(0x2000, 0x403, 21600, 0), Calculation(0x2001, 0x406, 1, 2), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST + 1, 1, 2), Calculation(0x2001, PROP_ADJUST_VAL_FIRST + 1, 1, 4), Calculation(0x6000, 0x408, 0x409, 0), Calculation(0x2001, 0x40b, 1, 2), Calculation(0x2000, 0x408, 21600, 0), Calculation(0x2001, 0x40d, 1, 2)
};

const int CURVED_CONNECTOR_4_DEFAULT_ADJUST[] =
{
  10800, 10800
};

const CustomShape CS_CURVED_CONNECTOR_4(
  CURVED_CONNECTOR_4_VERTICES, sizeof(CURVED_CONNECTOR_4_VERTICES) / sizeof(Vertex),
  CURVED_CONNECTOR_4_SEGMENTS, sizeof(CURVED_CONNECTOR_4_SEGMENTS) / sizeof(unsigned short),
  CURVED_CONNECTOR_4_CALC, sizeof(CURVED_CONNECTOR_4_CALC) / sizeof(Calculation),
  CURVED_CONNECTOR_4_DEFAULT_ADJUST, sizeof(CURVED_CONNECTOR_4_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CURVED_CONNECTOR_3_VERTICES[] =
{
  Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 5400), Vertex(0 CALCULATED_VALUE, 10800), Vertex(0 CALCULATED_VALUE, 16200), Vertex(3 CALCULATED_VALUE, 21600), Vertex(21600, 21600)
};

const unsigned short CURVED_CONNECTOR_3_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000
};

const Calculation CURVED_CONNECTOR_3_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 1, 2), Calculation(0x2000, 0x400, 21600, 0), Calculation(0x2001, 0x402, 1, 2)
};

const int CURVED_CONNECTOR_3_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_CURVED_CONNECTOR_3(
  CURVED_CONNECTOR_3_VERTICES, sizeof(CURVED_CONNECTOR_3_VERTICES) / sizeof(Vertex),
  CURVED_CONNECTOR_3_SEGMENTS, sizeof(CURVED_CONNECTOR_3_SEGMENTS) / sizeof(unsigned short),
  CURVED_CONNECTOR_3_CALC, sizeof(CURVED_CONNECTOR_3_CALC) / sizeof(Calculation),
  CURVED_CONNECTOR_3_DEFAULT_ADJUST, sizeof(CURVED_CONNECTOR_3_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);


const Vertex BENT_CONNECTOR_5_VERTICES[] =
{
  Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21600), Vertex(21600, 21600)
};

const unsigned short BENT_CONNECTOR_5_SEGMENTS[] =
{
  0x4000, 0x0005, 0x8000
};

const Calculation BENT_CONNECTOR_5_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x6000, 0x400, 0x401, 0), Calculation(0x2001, 0x402, 1, 2), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2001, 0x404, 1, 2), Calculation(0x4000, 21600, 0x404, 0), Calculation(0x2001, 0x406, 1, 2)
};

const int BENT_CONNECTOR_5_DEFAULT_ADJUST[] =
{
  10800, 10800, 10800
};

const CustomShape CS_BENT_CONNECTOR_5(
  BENT_CONNECTOR_5_VERTICES, sizeof(BENT_CONNECTOR_5_VERTICES) / sizeof(Vertex),
  BENT_CONNECTOR_5_SEGMENTS, sizeof(BENT_CONNECTOR_5_SEGMENTS) / sizeof(unsigned short),
  BENT_CONNECTOR_5_CALC, sizeof(BENT_CONNECTOR_5_CALC) / sizeof(Calculation),
  BENT_CONNECTOR_5_DEFAULT_ADJUST, sizeof(BENT_CONNECTOR_5_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex CURVED_CONNECTOR_2_VERTICES[] =
{
  Vertex(0, 0), Vertex(10800, 0), Vertex(21600, 10800), Vertex(21600, 21600)
};

const unsigned short CURVED_CONNECTOR_2_SEGMENTS[] =
{
  0x4000, 0x2001, 0x8000
};

const CustomShape CS_CURVED_CONNECTOR_2(
  CURVED_CONNECTOR_2_VERTICES, sizeof(CURVED_CONNECTOR_2_VERTICES) / sizeof(Vertex),
  CURVED_CONNECTOR_2_SEGMENTS, sizeof(CURVED_CONNECTOR_2_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex BENT_CONNECTOR_4_VERTICES[] =
{
  Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(21600, 1 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short BENT_CONNECTOR_4_SEGMENTS[] =
{
  0x4000, 0x0004, 0x8000
};

const Calculation BENT_CONNECTOR_4_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x400, 21600, 0), Calculation(0x2001, 0x402, 1, 2), Calculation(0x2001, 0x401, 1, 2)
};

const int BENT_CONNECTOR_4_DEFAULT_ADJUST[] =
{
  10800, 10800
};

const CustomShape CS_BENT_CONNECTOR_4(
  BENT_CONNECTOR_4_VERTICES, sizeof(BENT_CONNECTOR_4_VERTICES) / sizeof(Vertex),
  BENT_CONNECTOR_4_SEGMENTS, sizeof(BENT_CONNECTOR_4_SEGMENTS) / sizeof(unsigned short),
  BENT_CONNECTOR_4_CALC, sizeof(BENT_CONNECTOR_4_CALC) / sizeof(Calculation),
  BENT_CONNECTOR_4_DEFAULT_ADJUST, sizeof(BENT_CONNECTOR_4_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex BENT_CONNECTOR_3_VERTICES[] =
{
  Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 21600), Vertex(21600, 21600)
};

const unsigned short BENT_CONNECTOR_3_SEGMENTS[] =
{
  0x4000, 0x0003, 0x8000
};

const Calculation BENT_CONNECTOR_3_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0)
};

const int BENT_CONNECTOR_3_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_BENT_CONNECTOR_3(
  BENT_CONNECTOR_3_VERTICES, sizeof(BENT_CONNECTOR_3_VERTICES) / sizeof(Vertex),
  BENT_CONNECTOR_3_SEGMENTS, sizeof(BENT_CONNECTOR_3_SEGMENTS) / sizeof(unsigned short),
  BENT_CONNECTOR_3_CALC, sizeof(BENT_CONNECTOR_3_CALC) / sizeof(Calculation),
  BENT_CONNECTOR_3_DEFAULT_ADJUST, sizeof(BENT_CONNECTOR_3_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex BENT_CONNECTOR_2_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600)
};

const unsigned short BENT_CONNECTOR_2_SEGMENTS[] =
{
  0x4000, 0x0002, 0x8000
};

const CustomShape CS_BENT_CONNECTOR_2(
  BENT_CONNECTOR_2_VERTICES, sizeof(BENT_CONNECTOR_2_VERTICES) / sizeof(Vertex),
  BENT_CONNECTOR_2_SEGMENTS, sizeof(BENT_CONNECTOR_2_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex STRAIGHT_CONNECTOR_1_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600)
};

const unsigned short STRAIGHT_CONNECTOR_1_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000
};

const CustomShape CS_STRAIGHT_CONNECTOR_1(
  STRAIGHT_CONNECTOR_1_VERTICES, sizeof(STRAIGHT_CONNECTOR_1_VERTICES) / sizeof(Vertex),
  STRAIGHT_CONNECTOR_1_SEGMENTS, sizeof(STRAIGHT_CONNECTOR_1_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex TEXT_SIMPLE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0)
};

const CustomShape CS_TEXT_SIMPLE(
  TEXT_SIMPLE_VERTICES, sizeof(TEXT_SIMPLE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex DONUT_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360), Vertex(10800, 10800), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0, 360)
};

const unsigned short DONUT_SEGMENTS[] =
{
  0xa203, 0x6000, 0xa203, 0x8000
};

const Calculation DONUT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle DONUT_TRS[] =
{
  TextRectangle(Vertex(3163, 3163), Vertex(18437, 18437))
};

const int DONUT_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex DONUT_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_DONUT(
  DONUT_VERTICES, sizeof(DONUT_VERTICES) / sizeof(Vertex),
  DONUT_SEGMENTS, sizeof(DONUT_SEGMENTS) / sizeof(unsigned short),
  DONUT_CALC, sizeof(DONUT_CALC) / sizeof(Calculation),
  DONUT_DEFAULT_ADJUST, sizeof(DONUT_DEFAULT_ADJUST) / sizeof(int),
  DONUT_TRS, sizeof(DONUT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  DONUT_GLUE_POINTS, sizeof(DONUT_GLUE_POINTS) / sizeof(Vertex));

const Vertex CAN_VERTICES[] =
{
  Vertex(44, 0), Vertex(20, 0), Vertex(0, 2 CALCULATED_VALUE), Vertex(0, 0 CALCULATED_VALUE), Vertex(0, 3 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(20, 21600), Vertex(44, 21600), Vertex(68, 21600), Vertex(88, 4 CALCULATED_VALUE), Vertex(88, 3 CALCULATED_VALUE), Vertex(88, 0 CALCULATED_VALUE), Vertex(88, 2 CALCULATED_VALUE), Vertex(68, 0), Vertex(44, 0), Vertex(44, 0), Vertex(20, 0), Vertex(0, 2 CALCULATED_VALUE), Vertex(0, 0 CALCULATED_VALUE), Vertex(0, 5 CALCULATED_VALUE), Vertex(20, 6 CALCULATED_VALUE), Vertex(44, 6 CALCULATED_VALUE), Vertex(68, 6 CALCULATED_VALUE), Vertex(88, 5 CALCULATED_VALUE), Vertex(88, 0 CALCULATED_VALUE), Vertex(88, 2 CALCULATED_VALUE), Vertex(68, 0), Vertex(44, 0)
};

const unsigned short CAN_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2002, 0x0001, 0x2001, 0x6001, 0x8000, 0x4000, 0x2004, 0x6001, 0x8000
};

const Calculation CAN_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 2, 4), Calculation(0x2001, OTHER_CALC_VAL, 6, 11), Calculation(0xA000, OTHER_CALC_VAL, 0, OTHER_CALC_VAL | 0x1), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL), Calculation(0x6000, OTHER_CALC_VAL | 0x3, OTHER_CALC_VAL | 0x1, 0), Calculation(0x6000, OTHER_CALC_VAL, OTHER_CALC_VAL | 0x1, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 2, 2), Calculation(0x0000, 44, 0, 0)
};

const TextRectangle CAN_TRS[] =
{
  TextRectangle(Vertex(0, 6 CALCULATED_VALUE), Vertex(88, 3 CALCULATED_VALUE))
};

const int CAN_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex CAN_GLUE_POINTS[] =
{
  Vertex(44, 6 CALCULATED_VALUE), Vertex(44, 0), Vertex(0, 10800), Vertex(44, 21600), Vertex(88, 10800)
};

const CustomShape CS_CAN(
  CAN_VERTICES, sizeof(CAN_VERTICES) / sizeof(Vertex),
  CAN_SEGMENTS, sizeof(CAN_SEGMENTS) / sizeof(unsigned short),
  CAN_CALC, sizeof(CAN_CALC) / sizeof(Calculation),
  CAN_DEFAULT_ADJUST, sizeof(CAN_DEFAULT_ADJUST) / sizeof(int),
  CAN_TRS, sizeof(CAN_TRS) / sizeof(TextRectangle),
  88, 21600,
  CAN_GLUE_POINTS, sizeof(CAN_GLUE_POINTS) / sizeof(Vertex));

const Vertex PLAQUE_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(0, 1 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 21600), Vertex(3 CALCULATED_VALUE, 21600), Vertex(21600, 2 CALCULATED_VALUE), Vertex(21600, 1 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 0)
};

const unsigned short PLAQUE_SEGMENTS[] =
{
  0x4000, 0xA801, 0x0001, 0xA701, 0x0001, 0xA801, 0x0001, 0xA701, 0x6000, 0x8000
};

const Calculation PLAQUE_CALC[] =
{
  Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xA000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xA000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2082, PROP_ADJUST_VAL_FIRST, 0, 45), Calculation(0x2000, OTHER_CALC_VAL | 0x4, 0, 10800), Calculation(0x8000, 0, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xA000, OTHER_CALC_VAL | 0x6, 0, OTHER_CALC_VAL | 0x5), Calculation(0xA000, PROP_GEO_LEFT, 0, OTHER_CALC_VAL | 0x7), Calculation(0xA000, PROP_GEO_TOP, 0, OTHER_CALC_VAL | 0x7), Calculation(0x6000, PROP_GEO_RIGHT, OTHER_CALC_VAL | 0x7, 0), Calculation(0x6000, PROP_GEO_BOTTOM, OTHER_CALC_VAL | 0x7, 0), Calculation(0xA000, PROP_GEO_LEFT, 0, OTHER_CALC_VAL | 0x5), Calculation(0xA000, PROP_GEO_TOP, 0, OTHER_CALC_VAL | 0x5), Calculation(0x6000, PROP_GEO_RIGHT, OTHER_CALC_VAL | 0x5, 0), Calculation(0x6000, PROP_GEO_BOTTOM, OTHER_CALC_VAL | 0x5, 0)
};

const TextRectangle PLAQUE_TRS[] =
{
  TextRectangle(Vertex(0xC CALCULATED_VALUE, 0xD CALCULATED_VALUE), Vertex(0xE CALCULATED_VALUE, 0xF CALCULATED_VALUE))
};

const int PLAQUE_DEFAULT_ADJUST[] =
{
  3600
};

const Vertex PLAQUE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_PLAQUE(
  PLAQUE_VERTICES, sizeof(PLAQUE_VERTICES) / sizeof(Vertex),
  PLAQUE_SEGMENTS, sizeof(PLAQUE_SEGMENTS) / sizeof(unsigned short),
  PLAQUE_CALC, sizeof(PLAQUE_CALC) / sizeof(Calculation),
  PLAQUE_DEFAULT_ADJUST, sizeof(PLAQUE_DEFAULT_ADJUST) / sizeof(int),
  PLAQUE_TRS, sizeof(PLAQUE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  PLAQUE_GLUE_POINTS, sizeof(PLAQUE_GLUE_POINTS) / sizeof(Vertex));


const Vertex BALLOON_VERTICES[] =
{
  Vertex(3590, 0), Vertex(0, 3590), Vertex(0, 14460), Vertex(3590, 18050), Vertex(40 CALCULATED_VALUE, 21600), Vertex(5420, 18050), Vertex(18010, 18050), Vertex(21600, 14460), Vertex(21600, 3590), Vertex(18010, 0)
};

const unsigned short BALLOON_SEGMENTS[] =
{
  0x4000, 0xA701, 0x0001, 0xA801, 0x0003, 0xA701, 0x0001, 0xA801, 0x6001, 0x8000
};

const Calculation BALLOON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 10800), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 10800), Calculation(0x6006, OTHER_CALC_VAL | 0x12, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6006, OTHER_CALC_VAL | 0x12, PROP_ADJUST_VAL_FIRST + 1, 6280), Calculation(OTHER_CALC_VAL | 0x17, PROP_ADJUST_VAL_FIRST, 0, 0x6006), Calculation(0x6006, OTHER_CALC_VAL | 0x17, PROP_ADJUST_VAL_FIRST + 1, 15320), Calculation(0x6006, OTHER_CALC_VAL | 0x1A, PROP_ADJUST_VAL_FIRST, 6280), Calculation(0x6006, OTHER_CALC_VAL | 0x1A, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x1D, PROP_ADJUST_VAL_FIRST, 15320), Calculation(0x6006, OTHER_CALC_VAL | 0x1D, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x20, PROP_ADJUST_VAL_FIRST, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x20, PROP_ADJUST_VAL_FIRST + 1, 15320), Calculation(0x6006, OTHER_CALC_VAL | 0x22, PROP_ADJUST_VAL_FIRST, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x22, PROP_ADJUST_VAL_FIRST + 1, 6280), Calculation(0x6006, OTHER_CALC_VAL | 0x24, PROP_ADJUST_VAL_FIRST, 15320), Calculation(0x6006, OTHER_CALC_VAL | 0x24, PROP_ADJUST_VAL_FIRST + 1, 0), Calculation(0x6006, OTHER_CALC_VAL | 0x26, PROP_ADJUST_VAL_FIRST, 6280), Calculation(0x6006, OTHER_CALC_VAL | 0x26, PROP_ADJUST_VAL_FIRST + 1, 0), Calculation(0xA006, PROP_ADJUST_VAL_FIRST, -1, OTHER_CALC_VAL | 0x13), Calculation(0xA006, OTHER_CALC_VAL | 0x1, -1, OTHER_CALC_VAL | 0x16), Calculation(0x2003, OTHER_CALC_VAL, 0, 0), Calculation(0x2003, OTHER_CALC_VAL | 0x1, 0, 0), Calculation(0xA000, OTHER_CALC_VAL | 0x14, 0, OTHER_CALC_VAL | 0x15), Calculation(0xA006, PROP_ADJUST_VAL_FIRST, -1, OTHER_CALC_VAL | 0x18), Calculation(0x6006, OTHER_CALC_VAL | 0x1, OTHER_CALC_VAL | 0x16, -1), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x19, OTHER_CALC_VAL | 0x1B, -1), Calculation(0xA006, OTHER_CALC_VAL, -1, OTHER_CALC_VAL | 0x1C), Calculation(0xA000, OTHER_CALC_VAL | 0x15, 0, OTHER_CALC_VAL | 0x14), Calculation(0x6006, OTHER_CALC_VAL | 0x19, OTHER_CALC_VAL | 0x1E, -1), Calculation(0x6006, OTHER_CALC_VAL, OTHER_CALC_VAL | 0x1C, -1), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 21600), Calculation(0x6006, OTHER_CALC_VAL | 0x1F, OTHER_CALC_VAL | 0x21, -1), Calculation(0x6006, OTHER_CALC_VAL | 0x1, OTHER_CALC_VAL | 0x16, -1), Calculation(0x6006, OTHER_CALC_VAL | 0x1F, OTHER_CALC_VAL | 0x23, -1), Calculation(0xA006, OTHER_CALC_VAL | 0x1, -1, OTHER_CALC_VAL | 0x16), Calculation(0xA006, PROP_ADJUST_VAL_FIRST + 1, -1, OTHER_CALC_VAL | 0x25), Calculation(0x6006, OTHER_CALC_VAL, OTHER_CALC_VAL | 0x1C, -1), Calculation(0xA006, PROP_ADJUST_VAL_FIRST + 1, -1, OTHER_CALC_VAL | 0x27), Calculation(0xA006, OTHER_CALC_VAL, -1, OTHER_CALC_VAL | 0x1C), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0)
};

const TextRectangle BALLOON_TRS[] =
{
  TextRectangle(Vertex(800, 800), Vertex(20800, 17250))
};

const int BALLOON_DEFAULT_ADJUST[] =
{
  1400, 25920
};

const CustomShape CS_BALLOON(
  BALLOON_VERTICES, sizeof(BALLOON_VERTICES) / sizeof(Vertex),
  BALLOON_SEGMENTS, sizeof(BALLOON_SEGMENTS) / sizeof(unsigned short),
  BALLOON_CALC, sizeof(BALLOON_CALC) / sizeof(Calculation),
  BALLOON_DEFAULT_ADJUST, sizeof(BALLOON_DEFAULT_ADJUST) / sizeof(int),
  BALLOON_TRS, sizeof(BALLOON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex CUBE_VERTICES[] =
{
  Vertex(0, 12 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 0), Vertex(4 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short CUBE_SEGMENTS[] =
{
  0x4000, 0x0005, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation CUBE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_TOP, OTHER_CALC_VAL, 0), Calculation(0x6000, PROP_GEO_LEFT, OTHER_CALC_VAL, 0), Calculation(0xA000, PROP_GEO_BOTTOM, 0, OTHER_CALC_VAL), Calculation(0xA000, PROP_GEO_RIGHT, 0, OTHER_CALC_VAL), Calculation(0xA000, PROP_GEO_RIGHT, 0, OTHER_CALC_VAL | 2), Calculation(0x2001, OTHER_CALC_VAL | 5, 1, 2), Calculation(0x6000, OTHER_CALC_VAL | 2, OTHER_CALC_VAL | 6, 0), Calculation(0xA000, PROP_GEO_BOTTOM, 0, OTHER_CALC_VAL | 1), Calculation(0x2001, OTHER_CALC_VAL | 8, 1, 2), Calculation(0x6000, OTHER_CALC_VAL | 1, OTHER_CALC_VAL | 9, 0), Calculation(0x2000, PROP_GEO_RIGHT, 0, 0), Calculation(0x2000, PROP_GEO_BOTTOM, 0, 0)
};

const TextRectangle CUBE_TRS[] =
{
  TextRectangle(Vertex(0, 1 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 12 CALCULATED_VALUE))
};

const int CUBE_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex CUBE_GLUE_POINTS[] =
{
  Vertex(7 CALCULATED_VALUE, 0), Vertex(6 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0, 10 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 21600), Vertex(4 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(21600, 9 CALCULATED_VALUE)
};

const CustomShape CS_CUBE(
  CUBE_VERTICES, sizeof(CUBE_VERTICES) / sizeof(Vertex),
  CUBE_SEGMENTS, sizeof(CUBE_SEGMENTS) / sizeof(unsigned short),
  CUBE_CALC, sizeof(CUBE_CALC) / sizeof(Calculation),
  CUBE_DEFAULT_ADJUST, sizeof(CUBE_DEFAULT_ADJUST) / sizeof(int),
  CUBE_TRS, sizeof(CUBE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  CUBE_GLUE_POINTS, sizeof(CUBE_GLUE_POINTS) / sizeof(Vertex));

const Vertex HOME_PLATE_VERTICES[] =
{
  Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 21600)
};

const unsigned short HOME_PLATE_SEGMENTS[] =
{
  0x4000, 0x0004, 0x6001, 0x8000
};

const Calculation HOME_PLATE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0)
};

const TextRectangle HOME_PLATE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int HOME_PLATE_DEFAULT_ADJUST[] =
{
  16200
};

const CustomShape CS_HOME_PLATE(
  HOME_PLATE_VERTICES, sizeof(HOME_PLATE_VERTICES) / sizeof(Vertex),
  HOME_PLATE_SEGMENTS, sizeof(HOME_PLATE_SEGMENTS) / sizeof(unsigned short),
  HOME_PLATE_CALC, sizeof(HOME_PLATE_CALC) / sizeof(Calculation),
  HOME_PLATE_DEFAULT_ADJUST, sizeof(HOME_PLATE_DEFAULT_ADJUST) / sizeof(int),
  HOME_PLATE_TRS, sizeof(HOME_PLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ARROW_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE ), Vertex(1 CALCULATED_VALUE , 0), Vertex(21600, 10800), Vertex(1 CALCULATED_VALUE , 21600), Vertex(1 CALCULATED_VALUE , 2 CALCULATED_VALUE ), Vertex(0, 2 CALCULATED_VALUE )
};

const unsigned short ARROW_SEGMENTS[] =
{
  0x4000, 0x0006, 0x6001, 0x8000
};

const Calculation ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL  | 1), Calculation(0x6001, OTHER_CALC_VAL | 3, OTHER_CALC_VAL, 10800), Calculation(0x6000, OTHER_CALC_VAL | 1, OTHER_CALC_VAL | 4, 0), Calculation(0x6001, OTHER_CALC_VAL | 1, OTHER_CALC_VAL, 10800), Calculation(0xA000, OTHER_CALC_VAL | 1, 0, OTHER_CALC_VAL | 6)
};

const TextRectangle ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 2 CALCULATED_VALUE))
};

const int ARROW_DEFAULT_ADJUST[] =
{
  16200, 5400
};

const CustomShape CS_ARROW(
  ARROW_VERTICES, sizeof(ARROW_VERTICES) / sizeof(Vertex),
  ARROW_SEGMENTS, sizeof(ARROW_SEGMENTS) / sizeof(unsigned short),
  ARROW_CALC, sizeof(ARROW_CALC) / sizeof(Calculation),
  ARROW_DEFAULT_ADJUST, sizeof(ARROW_DEFAULT_ADJUST) / sizeof(int),
  ARROW_TRS, sizeof(ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex OCTAGON_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(2 CALCULATED_VALUE, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(21600, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 3 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE)
};

const unsigned short OCTAGON_SEGMENTS[] =
{
  0x4000, 0x0007, 0x6001, 0x8000
};

const Calculation OCTAGON_CALC[] =
{
  Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xA000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xA000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x6000, PROP_GEO_LEFT, OTHER_CALC_VAL | 4, 0), Calculation(0x6000, PROP_GEO_TOP, OTHER_CALC_VAL | 4, 0), Calculation(0xA000, PROP_GEO_RIGHT, 0, OTHER_CALC_VAL | 4), Calculation(0xA000, PROP_GEO_BOTTOM, 0, OTHER_CALC_VAL | 4)
};

const TextRectangle OCTAGON_TRS[] =
{
  TextRectangle(Vertex(5 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 8 CALCULATED_VALUE))
};

const int OCTAGON_DEFAULT_ADJUST[] =
{
  5000
};

const Vertex OCTAGON_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_OCTAGON(
  OCTAGON_VERTICES, sizeof(OCTAGON_VERTICES) / sizeof(Vertex),
  OCTAGON_SEGMENTS, sizeof(OCTAGON_SEGMENTS) / sizeof(unsigned short),
  OCTAGON_CALC, sizeof(OCTAGON_CALC) / sizeof(Calculation),
  OCTAGON_DEFAULT_ADJUST, sizeof(OCTAGON_DEFAULT_ADJUST) / sizeof(int),
  OCTAGON_TRS, sizeof(OCTAGON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  OCTAGON_GLUE_POINTS, sizeof(OCTAGON_GLUE_POINTS) / sizeof(Vertex));

const Vertex ROUND_RECTANGLE_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0), Vertex(0, 8 CALCULATED_VALUE), Vertex(0, 9 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 21600), Vertex(10 CALCULATED_VALUE, 21600), Vertex(21600, 9 CALCULATED_VALUE), Vertex(21600, 8 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 0)
};

const unsigned short ROUND_RECTANGLE_SEGMENTS[] =
{
  0x4000, 0xA701, 0x0001, 0xA801, 0x0001, 0xA701, 0x0001, 0xA801, 0x6000, 0x8000
};

const Calculation ROUND_RECTANGLE_CALC[] =
{
  Calculation(0x000E, 0, 45, 0), Calculation(0x6009, PROP_ADJUST_VAL_FIRST, OTHER_CALC_VAL, 0), Calculation(0x2001, OTHER_CALC_VAL | 1, 3163, 7636), Calculation(0x6000, PROP_GEO_LEFT, OTHER_CALC_VAL | 2, 0), Calculation(0x6000, PROP_GEO_TOP, OTHER_CALC_VAL | 2, 0), Calculation(0xA000, PROP_GEO_RIGHT, 0, OTHER_CALC_VAL | 2), Calculation(0xA000, PROP_GEO_BOTTOM, 0, OTHER_CALC_VAL | 2), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xA000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xA000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle ROUND_RECTANGLE_TRS[] =
{
  TextRectangle(Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 6 CALCULATED_VALUE))
};

const int ROUND_RECTANGLE_DEFAULT_ADJUST[] =
{
  3600
};

const CustomShape CS_ROUND_RECTANGLE(
  ROUND_RECTANGLE_VERTICES, sizeof(ROUND_RECTANGLE_VERTICES) / sizeof(Vertex),
  ROUND_RECTANGLE_SEGMENTS, sizeof(ROUND_RECTANGLE_SEGMENTS) / sizeof(unsigned short),
  ROUND_RECTANGLE_CALC, sizeof(ROUND_RECTANGLE_CALC) / sizeof(Calculation),
  ROUND_RECTANGLE_DEFAULT_ADJUST, sizeof(ROUND_RECTANGLE_DEFAULT_ADJUST) / sizeof(int),
  ROUND_RECTANGLE_TRS, sizeof(ROUND_RECTANGLE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex STAR_VERTICES[] =
{
  Vertex(10797, 0), Vertex(8278, 8256), Vertex(0, 8256), Vertex(6722, 13405), Vertex(4198, 21600), Vertex(10797, 16580), Vertex(17401, 21600), Vertex(14878, 13405), Vertex(21600, 8256), Vertex(13321, 8256), Vertex(10797, 0)
};

const TextRectangle STAR_TRS[] =
{
  TextRectangle(Vertex(6722, 8256), Vertex(14878, 15460))
};

const CustomShape CS_STAR(
  STAR_VERTICES, sizeof(STAR_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  STAR_TRS, sizeof(STAR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex PLUS_VERTICES[] =
{
  Vertex(1 CALCULATED_VALUE, 0), Vertex(2 CALCULATED_VALUE, 0), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(21600, 1 CALCULATED_VALUE), Vertex(21600, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 3 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0)
};

const Calculation PLUS_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 10799, 10800), Calculation(0x2000, OTHER_CALC_VAL, 0, 0), Calculation(0xA000, PROP_GEO_RIGHT, 0, OTHER_CALC_VAL), Calculation(0xA000, PROP_GEO_BOTTOM, 0, OTHER_CALC_VAL)
};

const TextRectangle PLUS_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE))
};

const int PLUS_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex PLUS_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_PLUS(
  PLUS_VERTICES, sizeof(PLUS_VERTICES) / sizeof(Vertex),
  NULL, 0,
  PLUS_CALC, sizeof(PLUS_CALC) / sizeof(Calculation),
  PLUS_DEFAULT_ADJUST, sizeof(PLUS_DEFAULT_ADJUST) / sizeof(int),
  PLUS_TRS, sizeof(PLUS_TRS) / sizeof(TextRectangle),
  21600, 21600,
  PLUS_GLUE_POINTS, sizeof(PLUS_GLUE_POINTS) / sizeof(Vertex));


const Vertex TRAPEZOID_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 21600)
};

const unsigned short TRAPEZOID_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation TRAPEZOID_CALC[] =
{
  Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 10, 18), Calculation(0x2000, OTHER_CALC_VAL | 2, 1750, 0), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 3), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 5)
};

const TextRectangle TRAPEZOID_TRS[] =
{
  TextRectangle(Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int TRAPEZOID_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex TRAPEZOID_GLUE_POINTS[] =
{
  Vertex(6 CALCULATED_VALUE, 10800), Vertex(10800, 21600), Vertex(5 CALCULATED_VALUE, 10800), Vertex(10800, 0)
};

const CustomShape CS_TRAPEZOID(
  TRAPEZOID_VERTICES, sizeof(TRAPEZOID_VERTICES) / sizeof(Vertex),
  TRAPEZOID_SEGMENTS, sizeof(TRAPEZOID_SEGMENTS) / sizeof(unsigned short),
  TRAPEZOID_CALC, sizeof(TRAPEZOID_CALC) / sizeof(Calculation),
  TRAPEZOID_DEFAULT_ADJUST, sizeof(TRAPEZOID_DEFAULT_ADJUST) / sizeof(int),
  TRAPEZOID_TRS, sizeof(TRAPEZOID_TRS) / sizeof(TextRectangle),
  21600, 21600,
  TRAPEZOID_GLUE_POINTS, sizeof(TRAPEZOID_GLUE_POINTS) / sizeof(Vertex));

const Vertex DIAMOND_VERTICES[] =
{
  Vertex(10800, 0), Vertex(21600, 10800), Vertex(10800, 21600), Vertex(0, 10800), Vertex(10800, 0)
};

const TextRectangle DIAMOND_TRS[] =
{
  TextRectangle(Vertex(5400, 5400), Vertex(16200, 16200))
};

const Vertex DIAMOND_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_DIAMOND(
  DIAMOND_VERTICES, sizeof(DIAMOND_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  DIAMOND_TRS, sizeof(DIAMOND_TRS) / sizeof(TextRectangle),
  21600, 21600,
  DIAMOND_GLUE_POINTS, sizeof(DIAMOND_GLUE_POINTS) / sizeof(Vertex));

const Vertex RIGHT_TRIANGLE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0)
};

const TextRectangle RIGHT_TRIANGLE_TRS[] =
{
  TextRectangle(Vertex(1900, 12700), Vertex(12700, 19700))
};

const Vertex RIGHT_TRIANGLE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(5400, 10800), Vertex(0, 21600), Vertex(10800, 21600), Vertex(21600, 21600), Vertex(16200, 10800)
};

const CustomShape CS_RIGHT_TRIANGLE(
  RIGHT_TRIANGLE_VERTICES, sizeof(RIGHT_TRIANGLE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  RIGHT_TRIANGLE_TRS, sizeof(RIGHT_TRIANGLE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  RIGHT_TRIANGLE_GLUE_POINTS, sizeof(RIGHT_TRIANGLE_GLUE_POINTS) / sizeof(Vertex));


const Vertex RECTANGLE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0)
};

const CustomShape CS_RECTANGLE(
  RECTANGLE_VERTICES, sizeof(RECTANGLE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

const Vertex ELLIPSE_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360)
};

const unsigned short ELLIPSE_SEGMENTS[] =
{
  0xA203, 0x6000, 0x8000
};

const TextRectangle ELLIPSE_TRS[] =
{
  TextRectangle(Vertex(3163, 3163), Vertex(18437, 18437))
};

const Vertex ELLIPSE_GLUE_PTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600),
  Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_ELLIPSE(
  ELLIPSE_VERTICES, sizeof(ELLIPSE_VERTICES) / sizeof(Vertex),
  ELLIPSE_SEGMENTS, sizeof(ELLIPSE_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  ELLIPSE_TRS, sizeof(ELLIPSE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  ELLIPSE_GLUE_PTS, sizeof(ELLIPSE_GLUE_PTS) / sizeof(Vertex));

const Vertex SEAL_4_VERTICES[] =
{
  Vertex(0, 10800), Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE),
  Vertex(10800, 0), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE),
  Vertex(21600, 10800), Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE),
  Vertex(10800, 21600), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE),
  Vertex(0, 10800)
};

const Calculation SEAL_4_CALC[] =
{
  Calculation(0x0000, 7600, 0, 0),
  Calculation(0x6001, OTHER_CALC_VAL, PROP_ADJUST_VAL_FIRST, 10800),
  Calculation(0xA000, OTHER_CALC_VAL, 0, OTHER_CALC_VAL | 1),
  Calculation(0x4000, 10800, OTHER_CALC_VAL | 2, 0),
  Calculation(0x8000, 10800, 0, OTHER_CALC_VAL | 2)
};

const TextRectangle SEAL_4_TRS[] =
{
  TextRectangle(Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE))
};

const int SEAL_4_DEFAULT_ADJUST[] =
{
  8100
};

const CustomShape CS_SEAL_4(
  SEAL_4_VERTICES, sizeof(SEAL_4_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_4_CALC, sizeof(SEAL_4_CALC) / sizeof(Calculation),
  SEAL_4_DEFAULT_ADJUST, sizeof(SEAL_4_DEFAULT_ADJUST) / sizeof(int),
  SEAL_4_TRS, sizeof(SEAL_4_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ISOCELES_TRIANGLE_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 21600), Vertex(0, 21600)
};

const unsigned short ISOCELES_TRIANGLE_SEGMENTS[] =
{
  0x4000, 0x0002, 0x6001, 0x8000
};

const Calculation ISOCELES_TRIANGLE_CALC[] =
{
  Calculation(0x4000, 0, PROP_ADJUST_VAL_FIRST, 0),
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2),
  Calculation(0x2000, OTHER_CALC_VAL | 1, 10800, 0),
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 2, 3),
  Calculation(0x2000, OTHER_CALC_VAL | 3, 7200, 0),
  Calculation(0x8000, 21600, 0, OTHER_CALC_VAL),
  Calculation(0x2001, OTHER_CALC_VAL | 5, 1, 2),
  Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 6)
};

const TextRectangle ISOCELES_TRIANGLE_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 10800), Vertex(2 CALCULATED_VALUE, 18000)),
  TextRectangle(Vertex(3 CALCULATED_VALUE, 7200), Vertex(4 CALCULATED_VALUE, 21600))
};

const Vertex ISOCELES_TRIANGLE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(1 CALCULATED_VALUE, 10800),
  Vertex(0, 21600), Vertex(10800, 21600),
  Vertex(21600, 21600), Vertex(7 CALCULATED_VALUE, 10800)
};

const int ISOCELES_TRIANGLE_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_ISOCELES_TRIANGLE(
  ISOCELES_TRIANGLE_VERTICES, sizeof(ISOCELES_TRIANGLE_VERTICES) / sizeof(Vertex),
  ISOCELES_TRIANGLE_SEGMENTS, sizeof(ISOCELES_TRIANGLE_SEGMENTS) / sizeof(unsigned short),
  ISOCELES_TRIANGLE_CALC, sizeof(ISOCELES_TRIANGLE_CALC) / sizeof(Calculation),
  ISOCELES_TRIANGLE_DEFAULT_ADJUST, sizeof(ISOCELES_TRIANGLE_DEFAULT_ADJUST) / sizeof(int),
  ISOCELES_TRIANGLE_TRS, sizeof(ISOCELES_TRIANGLE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  ISOCELES_TRIANGLE_GLUE_POINTS, sizeof(ISOCELES_TRIANGLE_GLUE_POINTS) / sizeof(Vertex));

const Vertex PARALLELOGRAM_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 0), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0, 21600)
};

const unsigned short PARALLELOGRAM_SEGMENTS[] =
{
  0x4000, 0x003, 0x6001, 0x8000
};

const Calculation PARALLELOGRAM_CALC[] =
{
  Calculation(0x4000, 0, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x8000, 0, 21600, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 10, 24), Calculation(0x2000, OTHER_CALC_VAL | 2, 1750, 0), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 3), Calculation(0x2001, OTHER_CALC_VAL, 1, 2), Calculation(0x4000, 10800, OTHER_CALC_VAL | 5, 0), Calculation(0x2000, OTHER_CALC_VAL, 0, 10800), Calculation(0x6006, OTHER_CALC_VAL | 7, OTHER_CALC_VAL | 0xC, 21600), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 5), Calculation(0x8001, 21600, 10800, OTHER_CALC_VAL), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 0xC)
};

const TextRectangle PARALLELOGRAM_TRS[] =
{
  TextRectangle(Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int PARALLELOGRAM_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex PARALLELOGRAM_GLUE_POINTS[] =
{
  Vertex(6 CALCULATED_VALUE, 0), Vertex(10800, 8 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 10800), Vertex(9 CALCULATED_VALUE, 21600), Vertex(10800, 10 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 10800)
};

const CustomShape CS_PARALLELOGRAM(
  PARALLELOGRAM_VERTICES, sizeof(PARALLELOGRAM_VERTICES) / sizeof(Vertex),
  PARALLELOGRAM_SEGMENTS, sizeof(PARALLELOGRAM_SEGMENTS) / sizeof(unsigned short),
  PARALLELOGRAM_CALC, sizeof(PARALLELOGRAM_CALC) / sizeof(Calculation),
  PARALLELOGRAM_DEFAULT_ADJUST, sizeof(PARALLELOGRAM_DEFAULT_ADJUST) / sizeof(int),
  PARALLELOGRAM_TRS, sizeof(PARALLELOGRAM_TRS) / sizeof(TextRectangle),
  21600, 21600,
  PARALLELOGRAM_GLUE_POINTS, sizeof(PARALLELOGRAM_GLUE_POINTS) / sizeof(Vertex));

const Vertex HEXAGON_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 10800)
};

const unsigned short HEXAGON_SEGMENTS[] =
{
  0x4000, 0x0005, 0x6001, 0x8000
};

const Calculation HEXAGON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 100, 234), Calculation(0x2000, OTHER_CALC_VAL | 2, 1700, 0), Calculation(0x8000, 21600, 0, OTHER_CALC_VAL | 3)
};

const TextRectangle HEXAGON_TRS[] =
{
  TextRectangle(Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int HEXAGON_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex HEXAGON_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_HEXAGON(
  HEXAGON_VERTICES, sizeof(HEXAGON_VERTICES) / sizeof(Vertex),
  HEXAGON_SEGMENTS, sizeof(HEXAGON_SEGMENTS) / sizeof(unsigned short),
  HEXAGON_CALC, sizeof(HEXAGON_CALC) / sizeof(Calculation),
  HEXAGON_DEFAULT_ADJUST, sizeof(HEXAGON_DEFAULT_ADJUST) / sizeof(int),
  HEXAGON_TRS, sizeof(HEXAGON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  HEXAGON_GLUE_POINTS, sizeof(HEXAGON_GLUE_POINTS) / sizeof(Vertex));


const CustomShape *libmspub::getCustomShape(ShapeType type)
{
  switch(type)
  {
  case RECTANGLE:
    return &CS_RECTANGLE;
  case ELLIPSE:
    return &CS_ELLIPSE;
  case ISOCELES_TRIANGLE:
    return &CS_ISOCELES_TRIANGLE;
  case SEAL_4:
    return &CS_SEAL_4;
  case PARALLELOGRAM:
    return &CS_PARALLELOGRAM;
  case RIGHT_TRIANGLE:
    return &CS_RIGHT_TRIANGLE;
  case DIAMOND:
    return &CS_DIAMOND;
  case TRAPEZOID:
    return &CS_TRAPEZOID;
  case HEXAGON:
    return &CS_HEXAGON;
  case STAR:
    return &CS_STAR;
  case PLUS:
    return &CS_PLUS;
  case ROUND_RECTANGLE:
    return &CS_ROUND_RECTANGLE;
  case OCTAGON:
    return &CS_OCTAGON;
  case ARROW:
    return &CS_ARROW;
  case HOME_PLATE:
    return &CS_HOME_PLATE;
  case CUBE:
    return &CS_CUBE;
  case BALLOON:
    return &CS_BALLOON;
  case PLAQUE:
    return &CS_PLAQUE;
  case CAN:
    return &CS_CAN;
  case DONUT:
    return &CS_DONUT;
  case TEXT_SIMPLE:
    return &CS_TEXT_SIMPLE;
  case STRAIGHT_CONNECTOR_1:
    return &CS_STRAIGHT_CONNECTOR_1;
  case BENT_CONNECTOR_2:
    return &CS_BENT_CONNECTOR_2;
  case BENT_CONNECTOR_3:
    return &CS_BENT_CONNECTOR_3;
  case BENT_CONNECTOR_4:
    return &CS_BENT_CONNECTOR_4;
  case BENT_CONNECTOR_5:
    return &CS_BENT_CONNECTOR_5;
  case CURVED_CONNECTOR_2:
    return &CS_CURVED_CONNECTOR_2;
  case CURVED_CONNECTOR_3:
    return &CS_CURVED_CONNECTOR_3;
  case CURVED_CONNECTOR_4:
    return &CS_CURVED_CONNECTOR_4;
  case CURVED_CONNECTOR_5:
    return &CS_CURVED_CONNECTOR_5;
  case CALLOUT_1:
    return &CS_CALLOUT_1;
  case CALLOUT_2:
    return &CS_CALLOUT_2;
  case CALLOUT_3:
    return &CS_CALLOUT_3;
  case RIBBON:
    return &CS_RIBBON;
  case RIBBON_2:
    return &CS_RIBBON_2;
  case CHEVRON:
    return &CS_CHEVRON;
  case PENTAGON:
    return &CS_PENTAGON;
  case NO_SMOKING:
    return &CS_NO_SMOKING;
  case SEAL_8:
    return &CS_SEAL_8;
  case SEAL_16:
    return &CS_SEAL_16;
  case SEAL_32:
    return &CS_SEAL_32;
  case WAVE:
    return &CS_WAVE;
  case FOLDED_CORNER:
    return &CS_FOLDED_CORNER;
  case LEFT_ARROW:
    return &CS_LEFT_ARROW;
  case DOWN_ARROW:
    return &CS_DOWN_ARROW;
  case UP_ARROW:
    return &CS_UP_ARROW;
  default:
    return NULL;
  }
}

enum Command
{
  MOVETO,
  LINETO,
  CURVETO,
  NOFILL,
  NOSTROKE,
  ANGLEELLIPSE,
  CLOSESUBPATH,
  ENDSUBPATH,
  ELLIPTICALQUADRANTX,
  ELLIPTICALQUADRANTY
};

struct ShapeElementCommand
{
  Command m_command;
  unsigned char m_count;
  ShapeElementCommand(Command command, unsigned char count) : m_command(command), m_count(count) { }
};

ShapeElementCommand getCommandFromBinary(unsigned short binary)
{
  Command cmd;
  unsigned count = 0;
  switch(binary >> 8)
  {
  case 0xAB:
    cmd = NOFILL;
    break;
  case 0xAA:
    cmd = NOSTROKE;
    break;
  case 0x20:
    cmd = CURVETO;
    count = (binary & 0xFF);
    break;
  case 0xA2:
    cmd = ANGLEELLIPSE;
    count = (binary & 0xFF) / 3;
    break;
  case 0xA7:
    cmd = ELLIPTICALQUADRANTX;
    count = (binary & 0xFF);
    break;
  case 0xA8:
    cmd = ELLIPTICALQUADRANTY;
    count = (binary & 0xFF);
    break;
  case 0x0:
    cmd = LINETO;
    count = (binary & 0xFF);
    break;
  case 0x40:
    cmd = MOVETO;
    count = (binary & 0xFF);
    count = count ? count : 1;
    break;
  case 0x80:
    cmd = CLOSESUBPATH;
    break;
  case 0x60:
    cmd = ENDSUBPATH;
    break;
  default:
    MSPUB_DEBUG_MSG(("unknown segment command 0x%x\n", binary >> 8));
    cmd = MOVETO;
    count = 1;
    break;
  }
  return ShapeElementCommand(cmd, count);
}

double getSpecialIfNecessary(const libmspub::GeometricShape *caller, int val)
{
  bool special = val & 0x80000000;
  return special ? caller->getCalculationValue(val ^ 0x80000000) : val;
}

Coordinate libmspub::CustomShape::getTextRectangle(double x, double y, double width, double height, const libmspub::GeometricShape *caller) const
{
  double divisorX = m_coordWidth / width;
  double divisorY = m_coordHeight / height;
  if (m_numTextRectangles == 0)
  {
    return Coordinate(x, y, x + width, y + height);
  }
  const Vertex &start = mp_textRectangles[0].first;
  const Vertex &end = mp_textRectangles[0].second;
  double startX = x + getSpecialIfNecessary(caller, start.m_x) / divisorX;
  double startY = y + getSpecialIfNecessary(caller, start.m_y) / divisorY;
  double endX = x + getSpecialIfNecessary(caller, end.m_x) / divisorX;
  double endY = y + getSpecialIfNecessary(caller, end.m_y) / divisorY;
  return Coordinate(startX, startY, endX, endY);
}

void libmspub::writeCustomShape(const CustomShape *shape, const WPXPropertyList & /* props */, libwpg::WPGPaintInterface *painter, double x, double y, double height, double width, const libmspub::GeometricShape *caller)
{
  if (width == 0 || height == 0)
  {
    return;
  }
  double divisorX = shape->m_coordWidth / width;
  double divisorY = shape->m_coordHeight / height;
  WPXPropertyListVector vertices;
  if (shape->mp_elements == NULL)
  {
    for (unsigned i = 0; i < shape->m_numVertices; ++i)
    {
      WPXPropertyList vertex;
      double vertexX = getSpecialIfNecessary(caller, shape->mp_vertices[i].m_x);
      double vertexY = getSpecialIfNecessary(caller, shape->mp_vertices[i].m_y);
      vertex.insert("svg:x", vertexX / divisorX + x);
      vertex.insert("svg:y", vertexY / divisorY + y);
      vertices.append(vertex);
    }
    painter->drawPolygon(vertices);
  }
  else
  {
    unsigned vertexIndex = 0;
    for (unsigned i = 0; i < shape->m_numElements; ++i)
    {
      ShapeElementCommand cmd = getCommandFromBinary(shape->mp_elements[i]);
      switch (cmd.m_command)
      {
      case ELLIPTICALQUADRANTX:
      case ELLIPTICALQUADRANTY:
      {
        bool firstDirection = true;
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex < shape->m_numVertices); ++j, ++vertexIndex)
        {
          bool modifier = cmd.m_command == ELLIPTICALQUADRANTX ? true : false;
          const Vertex &curr = shape->mp_vertices[vertexIndex];
          double currX = x + getSpecialIfNecessary(caller, curr.m_x) / divisorX;
          double currY = y + getSpecialIfNecessary(caller, curr.m_y) / divisorY;
          if (vertexIndex)
          {
            const Vertex &prev = shape->mp_vertices[vertexIndex - 1];
            double prevX = x + getSpecialIfNecessary(caller, prev.m_x) / divisorX;
            double prevY = y + getSpecialIfNecessary(caller, prev.m_y) / divisorY;
            double tmpX = currX - prevX;
            double tmpY = currY - prevY;
            if ((tmpX < 0 && tmpY >= 0) || (tmpX >= 0 && tmpY < 0))
            {
              if (j == 0)
              {
                firstDirection = true;
              }
              else if (! firstDirection)
              {
                modifier = !modifier;
              }
            }
            else
            {
              if (j == 0)
              {
                firstDirection = false;
              }
              else if (firstDirection)
              {
                modifier = !modifier;
              }
            }
            if (modifier)
            {
              tmpX = currX;
              tmpY = prevY;
            }
            else
            {
              tmpX = prevX;
              tmpY = currY;
            }
            double vecX = (tmpX - prevX) / 2;
            double vecY = (tmpY - prevY) / 2;
            double x1 = prevX + vecX;
            double y1 = prevY + vecY;
            vecX = (tmpX - currX) / 2;
            vecY = (tmpY - currY) / 2;
            double x2 = currX + vecX;
            double y2 = currY + vecY;
            WPXPropertyList bezier;
            bezier.insert("libwpg:path-action", "C");
            bezier.insert("svg:x1", x1);
            bezier.insert("svg:x2", x2);
            bezier.insert("svg:y1", y1);
            bezier.insert("svg:y2", y2);
            bezier.insert("svg:x", currX);
            bezier.insert("svg:y", currY);
            vertices.append(bezier);
          }
          else
          {
            //something is broken, just move
            if (vertexIndex < shape->m_numVertices)
            {
              WPXPropertyList moveVertex;
              double newX = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
              double newY = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
              moveVertex.insert("svg:x", newX / divisorX + x);
              moveVertex.insert("svg:y", newY / divisorY + y);
              moveVertex.insert("libwpg:path-action", "M");
              vertices.append(moveVertex);
              ++vertexIndex;
            }
          }
        }
      }
      break;
      case ANGLEELLIPSE:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex + 2 < shape->m_numVertices); ++j, vertexIndex += 3)
        {
          WPXPropertyList vertex;
          double startAngle = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_x);
          double endAngle = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_y);
          double cx = x + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x) / divisorX;
          double cy = y + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y) / divisorY;
          double rx = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_x) / divisorX;
          double ry = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_y) / divisorY;

          // FIXME: Are angles supposed to be the actual angle of the point with the x-axis,
          // or the eccentric anomaly, or something else?
          //
          // assuming eccentric anomaly for now
          WPXPropertyList moveVertex;
          double startX = cx + rx * cos(startAngle * PI / 180);
          double startY = cy + ry * sin(startAngle * PI / 180);
          moveVertex.insert("libwpg:path-action", "M");
          moveVertex.insert("svg:x", startX);
          moveVertex.insert("svg:y", startY);
          vertices.append(moveVertex);
          double halfX = cx + rx * cos(endAngle * PI / 360);
          double halfY = cy + ry * sin(endAngle * PI / 360);
          WPXPropertyList halfVertex;
          halfVertex.insert("libwpg:path-action", "A");
          halfVertex.insert("svg:x", halfX);
          halfVertex.insert("svg:y", halfY);
          halfVertex.insert("svg:rx", rx);
          halfVertex.insert("svg:ry", ry);
          vertices.append(halfVertex);
          double endX = cx + rx * cos(endAngle * PI / 180);
          double endY = cy + ry * sin(endAngle * PI / 180);
          vertex.insert("svg:x", endX);
          vertex.insert("svg:y", endY);
          vertex.insert("svg:rx", rx);
          vertex.insert("svg:ry", ry);
          vertex.insert("libwpg:path-action", "A");
          vertices.append(vertex);
        }
        break;
      case MOVETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex < shape->m_numVertices); ++j, ++vertexIndex)
        {
          WPXPropertyList moveVertex;
          double newX = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double newY = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          moveVertex.insert("svg:x", newX / divisorX + x);
          moveVertex.insert("svg:y", newY / divisorY + y);
          moveVertex.insert("libwpg:path-action", "M");
          vertices.append(moveVertex);
        }
        break;
      case LINETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex < shape->m_numVertices); ++j, ++vertexIndex)
        {
          WPXPropertyList vertex;
          double vertexX = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double vertexY = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          vertex.insert("svg:x", vertexX / divisorX + x);
          vertex.insert("svg:y", vertexY / divisorY + y);
          vertex.insert("libwpg:path-action", "L");
          vertices.append(vertex);
        }
        break;
      case CURVETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex + 2 < shape->m_numVertices); ++j, vertexIndex += 3)
        {
          double firstCtrlX = x + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x) / divisorX;
          double firstCtrlY = y + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y) / divisorY;
          double secondCtrlX = x + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_x) / divisorX;
          double secondCtrlY = y + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_y) / divisorY;
          double endX = x + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_x) / divisorX;
          double endY = y + getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_y) / divisorY;
          WPXPropertyList bezier;
          bezier.insert("libwpg:path-action", "C");
          bezier.insert("svg:x1", firstCtrlX);
          bezier.insert("svg:x2", secondCtrlX);
          bezier.insert("svg:y1", firstCtrlY);
          bezier.insert("svg:y2", secondCtrlY);
          bezier.insert("svg:x", endX);
          bezier.insert("svg:y", endY);
          vertices.append(bezier);
        }
      case CLOSESUBPATH:
      {
        WPXPropertyList end;
        end.insert("libwpg:path-action", "Z");
        vertices.append(end);
      }
      break;
      default:
        break;
      }
    }
    painter->drawPath(vertices);
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
