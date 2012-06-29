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

const Vertex LINE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600)
};

const CustomShape CS_LINE(
  LINE_VERTICES, sizeof(LINE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  NULL, 0);

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

const Vertex UP_DOWN_ARROW_VERTICES[] =
{
  Vertex(0, 1 CALCULATED_VALUE), Vertex(10800, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(21600, 3 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(0, 3 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short UP_DOWN_ARROW_SEGMENTS[] =
{
  0x4000, 0x0009, 0x6001, 0x8000
};

const Calculation UP_DOWN_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x6001, PROP_ADJUST_VAL_FIRST, 0x404, 10800), Calculation(0x8000, 21600, 0, 0x405), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x6001, PROP_ADJUST_VAL_FIRST + 1, 0x407, 10800), Calculation(0x8000, 21600, 0, 0x408)
};

const TextRectangle UP_DOWN_ARROW_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 9 CALCULATED_VALUE))
};

const int UP_DOWN_ARROW_DEFAULT_ADJUST[] =
{
  5400, 4300
};

const CustomShape CS_UP_DOWN_ARROW(
  UP_DOWN_ARROW_VERTICES, sizeof(UP_DOWN_ARROW_VERTICES) / sizeof(Vertex),
  UP_DOWN_ARROW_SEGMENTS, sizeof(UP_DOWN_ARROW_SEGMENTS) / sizeof(unsigned short),
  UP_DOWN_ARROW_CALC, sizeof(UP_DOWN_ARROW_CALC) / sizeof(Calculation),
  UP_DOWN_ARROW_DEFAULT_ADJUST, sizeof(UP_DOWN_ARROW_DEFAULT_ADJUST) / sizeof(int),
  UP_DOWN_ARROW_TRS, sizeof(UP_DOWN_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex IRREGULAR_SEAL_1_VERTICES[] =
{
  Vertex(10901, 5905), Vertex(8458, 2399), Vertex(7417, 6425), Vertex(476, 2399), Vertex(4732, 7722), Vertex(106, 8718), Vertex(3828, 11880), Vertex(243, 14689), Vertex(5772, 14041), Vertex(4868, 17719), Vertex(7819, 15730), Vertex(8590, 21600), Vertex(10637, 15038), Vertex(13349, 19840), Vertex(14125, 14561), Vertex(18248, 18195), Vertex(16938, 13044), Vertex(21600, 13393), Vertex(17710, 10579), Vertex(21198, 8242), Vertex(16806, 7417), Vertex(18482, 4560), Vertex(14257, 5429), Vertex(14623, 106), Vertex(10901, 5905)
};

const TextRectangle IRREGULAR_SEAL_1_TRS[] =
{
  TextRectangle(Vertex(5400, 6570), Vertex(14160, 15290))
};

const Vertex IRREGULAR_SEAL_1_GLUE_POINTS[] =
{
  Vertex(9722, 1887), Vertex(0, 12875), Vertex(11614, 18844), Vertex(21600, 6646)
};

const CustomShape CS_IRREGULAR_SEAL_1(
  IRREGULAR_SEAL_1_VERTICES, sizeof(IRREGULAR_SEAL_1_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  IRREGULAR_SEAL_1_TRS, sizeof(IRREGULAR_SEAL_1_TRS) / sizeof(TextRectangle),
  21600, 21600,
  IRREGULAR_SEAL_1_GLUE_POINTS, sizeof(IRREGULAR_SEAL_1_GLUE_POINTS) / sizeof(Vertex));

const Vertex LIGHTNING_BOLT_VERTICES[] =
{
  Vertex(8458, 0), Vertex(0, 3923), Vertex(7564, 8416), Vertex(4993, 9720), Vertex(12197, 13904), Vertex(9987, 14934), Vertex(21600, 21600), Vertex(14768, 12911), Vertex(16558, 12016), Vertex(11030, 6840), Vertex(12831, 6120), Vertex(8458, 0)
};

const TextRectangle LIGHTNING_BOLT_TRS[] =
{
  TextRectangle(Vertex(8680, 7410), Vertex(13970, 14190))
};

const Vertex LIGHTNING_BOLT_GLUE_POINTS[] =
{
  Vertex(8458, 0), Vertex(0, 3923), Vertex(4993, 9720), Vertex(9987, 14934), Vertex(21600, 21600), Vertex(16558, 12016), Vertex(12831, 6120)
};

const CustomShape CS_LIGHTNING_BOLT(
  LIGHTNING_BOLT_VERTICES, sizeof(LIGHTNING_BOLT_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  LIGHTNING_BOLT_TRS, sizeof(LIGHTNING_BOLT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  LIGHTNING_BOLT_GLUE_POINTS, sizeof(LIGHTNING_BOLT_GLUE_POINTS) / sizeof(Vertex));

const Vertex IRREGULAR_SEAL_2_VERTICES[] =
{
  Vertex(11464, 4340), Vertex(9722, 1887), Vertex(8548, 6383), Vertex(4503, 3626), Vertex(5373, 7816), Vertex(1174, 8270), Vertex(3934, 11592), Vertex(0, 12875), Vertex(3329, 15372), Vertex(1283, 17824), Vertex(4804, 18239), Vertex(4918, 21600), Vertex(7525, 18125), Vertex(8698, 19712), Vertex(9871, 17371), Vertex(11614, 18844), Vertex(12178, 15937), Vertex(14943, 17371), Vertex(14640, 14348), Vertex(18878, 15632), Vertex(16382, 12311), Vertex(18270, 11292), Vertex(16986, 9404), Vertex(21600, 6646), Vertex(16382, 6533), Vertex(18005, 3172), Vertex(14524, 5778), Vertex(14789, 0), Vertex(11464, 4340)
};

const TextRectangle IRREGULAR_SEAL_2_TRS[] =
{
  TextRectangle(Vertex(5400, 6570), Vertex(14160, 15290))
};

const Vertex IRREGULAR_SEAL_2_GLUE_POINTS[] =
{
  Vertex(9722, 1887), Vertex(0, 12875), Vertex(11614, 18844), Vertex(21600, 6646)
};

const CustomShape CS_IRREGULAR_SEAL_2(
  IRREGULAR_SEAL_2_VERTICES, sizeof(IRREGULAR_SEAL_2_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  IRREGULAR_SEAL_2_TRS, sizeof(IRREGULAR_SEAL_2_TRS) / sizeof(TextRectangle),
  21600, 21600,
  IRREGULAR_SEAL_2_GLUE_POINTS, sizeof(IRREGULAR_SEAL_2_GLUE_POINTS) / sizeof(Vertex));


const Vertex HEART_VERTICES[] =
{
  Vertex(10800, 21599), Vertex(321, 6886), Vertex(70, 6036), Vertex(-9, 5766), Vertex(-1, 5474), Vertex(2, 5192), Vertex(6, 4918), Vertex(43, 4641), Vertex(101, 4370), Vertex(159, 4103), Vertex(245, 3837), Vertex(353, 3582), Vertex(460, 3326), Vertex(591, 3077), Vertex(741, 2839), Vertex(892, 2598), Vertex(1066, 2369), Vertex(1253, 2155), Vertex(1443, 1938), Vertex(1651, 1732), Vertex(1874, 1543), Vertex(2097, 1351), Vertex(2337, 1174), Vertex(2587, 1014), Vertex(2839, 854), Vertex(3106, 708), Vertex(3380, 584), Vertex(3656, 459), Vertex(3945, 350), Vertex(4237, 264), Vertex(4533, 176), Vertex(4838, 108), Vertex(5144, 66), Vertex(5454, 22), Vertex(5771, 1), Vertex(6086, 3), Vertex(6407, 7), Vertex(6731, 35), Vertex(7048, 89), Vertex(7374, 144), Vertex(7700, 226), Vertex(8015, 335), Vertex(8344, 447), Vertex(8667, 590), Vertex(8972, 756), Vertex(9297, 932), Vertex(9613, 1135), Vertex(9907, 1363), Vertex(10224, 1609), Vertex(10504, 1900), Vertex(10802, 2169), Vertex(11697, 1363), Vertex(11971, 1116), Vertex(12304, 934), Vertex(12630, 756), Vertex(12935, 590), Vertex(13528, 450), Vertex(13589, 335), Vertex(13901, 226), Vertex(14227, 144), Vertex(14556, 89), Vertex(14872, 35), Vertex(15195, 7), Vertex(15517, 3), Vertex(15830, 0), Vertex(16147, 22), Vertex(16458, 66), Vertex(16764, 109), Vertex(17068, 177), Vertex(17365, 264), Vertex(17658, 349), Vertex(17946, 458), Vertex(18222, 584), Vertex(18496, 708), Vertex(18762, 854), Vertex(19015, 1014), Vertex(19264, 1172), Vertex(19504, 1349), Vertex(19730, 1543), Vertex(19950, 1731), Vertex(20158, 1937), Vertex(20350, 2155), Vertex(20536, 2369), Vertex(20710, 2598), Vertex(20861, 2839), Vertex(21010, 3074), Vertex(21143, 3323), Vertex(21251, 3582), Vertex(21357, 3835), Vertex(21443, 4099), Vertex(21502, 4370), Vertex(21561, 4639), Vertex(21595, 4916), Vertex(21600, 5192), Vertex(21606, 5474), Vertex(21584, 5760), Vertex(21532, 6036), Vertex(21478, 6326), Vertex(21366, 6603), Vertex(21282, 6887), Vertex(10802, 21602)
};

const unsigned short HEART_SEGMENTS[] =
{
  0x4000, 0x0002, 0x2010, 0x0001, 0x2010, 0x0001, 0x6001, 0x8000
};

const TextRectangle HEART_TRS[] =
{
  TextRectangle(Vertex(5080, 2540), Vertex(16520, 13550))
};

const Vertex HEART_GLUE_POINTS[] =
{
  Vertex(10800, 2180), Vertex(3090, 10800), Vertex(10800, 21600), Vertex(18490, 10800)
};

const CustomShape CS_HEART(
  HEART_VERTICES, sizeof(HEART_VERTICES) / sizeof(Vertex),
  HEART_SEGMENTS, sizeof(HEART_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  HEART_TRS, sizeof(HEART_TRS) / sizeof(TextRectangle),
  21600, 21600,
  HEART_GLUE_POINTS, sizeof(HEART_GLUE_POINTS) / sizeof(Vertex));

const Vertex QUAD_ARROW_VERTICES[] =
{
  Vertex(0, 10800), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(10800, 0), Vertex(3 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(21600, 10800), Vertex(5 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(1 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short QUAD_ARROW_SEGMENTS[] =
{
  0x4000, 0x0017, 0x6001, 0x8000
};

const Calculation QUAD_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 2)
};

const TextRectangle QUAD_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int QUAD_ARROW_DEFAULT_ADJUST[] =
{
  6500, 8600, 4300
};

const CustomShape CS_QUAD_ARROW(
  QUAD_ARROW_VERTICES, sizeof(QUAD_ARROW_VERTICES) / sizeof(Vertex),
  QUAD_ARROW_SEGMENTS, sizeof(QUAD_ARROW_SEGMENTS) / sizeof(unsigned short),
  QUAD_ARROW_CALC, sizeof(QUAD_ARROW_CALC) / sizeof(Calculation),
  QUAD_ARROW_DEFAULT_ADJUST, sizeof(QUAD_ARROW_DEFAULT_ADJUST) / sizeof(int),
  QUAD_ARROW_TRS, sizeof(QUAD_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex BEVEL_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const unsigned short BEVEL_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation BEVEL_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 21599, 21600), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x400), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400)
};

const TextRectangle BEVEL_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE))
};

const int BEVEL_DEFAULT_ADJUST[] =
{
  2700
};

const CustomShape CS_BEVEL(
  BEVEL_VERTICES, sizeof(BEVEL_VERTICES) / sizeof(Vertex),
  BEVEL_SEGMENTS, sizeof(BEVEL_SEGMENTS) / sizeof(unsigned short),
  BEVEL_CALC, sizeof(BEVEL_CALC) / sizeof(Calculation),
  BEVEL_DEFAULT_ADJUST, sizeof(BEVEL_DEFAULT_ADJUST) / sizeof(int),
  BEVEL_TRS, sizeof(BEVEL_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex LEFT_BRACKET_VERTICES[] =
{
  Vertex(21600, 0), Vertex(10800, 0), Vertex(0, 3 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(21600, 21600)
};

const unsigned short LEFT_BRACKET_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2001, 0x8000
};

const Calculation LEFT_BRACKET_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x6000, PROP_GEO_TOP, 0x400, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400)
};

const TextRectangle LEFT_BRACKET_TRS[] =
{
  TextRectangle(Vertex(6350, 3 CALCULATED_VALUE), Vertex(21600, 4 CALCULATED_VALUE))
};

const int LEFT_BRACKET_DEFAULT_ADJUST[] =
{
  1800
};

const Vertex LEFT_BRACKET_GLUE_POINTS[] =
{
  Vertex(21600, 0), Vertex(0, 10800), Vertex(21600, 21600)
};

const CustomShape CS_LEFT_BRACKET(
  LEFT_BRACKET_VERTICES, sizeof(LEFT_BRACKET_VERTICES) / sizeof(Vertex),
  LEFT_BRACKET_SEGMENTS, sizeof(LEFT_BRACKET_SEGMENTS) / sizeof(unsigned short),
  LEFT_BRACKET_CALC, sizeof(LEFT_BRACKET_CALC) / sizeof(Calculation),
  LEFT_BRACKET_DEFAULT_ADJUST, sizeof(LEFT_BRACKET_DEFAULT_ADJUST) / sizeof(int),
  LEFT_BRACKET_TRS, sizeof(LEFT_BRACKET_TRS) / sizeof(TextRectangle),
  21600, 21600,
  LEFT_BRACKET_GLUE_POINTS, sizeof(LEFT_BRACKET_GLUE_POINTS) / sizeof(Vertex));

const Vertex RIGHT_BRACKET_VERTICES[] =
{
  Vertex(0, 0), Vertex(10800, 0), Vertex(21600, 3 CALCULATED_VALUE), Vertex(21600, 1 CALCULATED_VALUE), Vertex(21600, 2 CALCULATED_VALUE), Vertex(21600, 4 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(0, 21600)
};

const unsigned short RIGHT_BRACKET_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2001, 0x8000
};

const Calculation RIGHT_BRACKET_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x6000, PROP_GEO_TOP, 0x400, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400)
};

const TextRectangle RIGHT_BRACKET_TRS[] =
{
  TextRectangle(Vertex(0, 3 CALCULATED_VALUE), Vertex(15150, 4 CALCULATED_VALUE))
};

const int RIGHT_BRACKET_DEFAULT_ADJUST[] =
{
  1800
};

const Vertex RIGHT_BRACKET_GLUE_POINTS[] =
{
  Vertex(0, 0), Vertex(0, 21600), Vertex(21600, 10800)
};

const CustomShape CS_RIGHT_BRACKET(
  RIGHT_BRACKET_VERTICES, sizeof(RIGHT_BRACKET_VERTICES) / sizeof(Vertex),
  RIGHT_BRACKET_SEGMENTS, sizeof(RIGHT_BRACKET_SEGMENTS) / sizeof(unsigned short),
  RIGHT_BRACKET_CALC, sizeof(RIGHT_BRACKET_CALC) / sizeof(Calculation),
  RIGHT_BRACKET_DEFAULT_ADJUST, sizeof(RIGHT_BRACKET_DEFAULT_ADJUST) / sizeof(int),
  RIGHT_BRACKET_TRS, sizeof(RIGHT_BRACKET_TRS) / sizeof(TextRectangle),
  21600, 21600,
  RIGHT_BRACKET_GLUE_POINTS, sizeof(RIGHT_BRACKET_GLUE_POINTS) / sizeof(Vertex));

const Vertex LEFT_BRACE_VERTICES[] =
{
  Vertex(21600, 0), Vertex(16200, 0), Vertex(10800, 0 CALCULATED_VALUE), Vertex(10800, 1 CALCULATED_VALUE), Vertex(10800, 2 CALCULATED_VALUE), Vertex(10800, 3 CALCULATED_VALUE), Vertex(5400, 4 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(5400, 4 CALCULATED_VALUE), Vertex(10800, 5 CALCULATED_VALUE), Vertex(10800, 6 CALCULATED_VALUE), Vertex(10800, 7 CALCULATED_VALUE), Vertex(10800, 8 CALCULATED_VALUE), Vertex(16200, 21600), Vertex(21600, 21600)
};

const unsigned short LEFT_BRACE_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2002, 0x0001, 0x2001, 0x8000
};

const Calculation LEFT_BRACE_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0xa000, 0x404, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x404, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x6000, 0x404, 0x400, 0), Calculation(0x6000, 0x404, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 10000, 31953), Calculation(0x8000, 21600, 0, 0x409)
};

const TextRectangle LEFT_BRACE_TRS[] =
{
  TextRectangle(Vertex(13800, 9 CALCULATED_VALUE), Vertex(21600, 10 CALCULATED_VALUE))
};

const int LEFT_BRACE_DEFAULT_ADJUST[] =
{
  1800, 10800
};

const Vertex LEFT_BRACE_GLUE_POINTS[] =
{
  Vertex(21600, 0), Vertex(0, 10800), Vertex(21600, 21600)
};

const CustomShape CS_LEFT_BRACE(
  LEFT_BRACE_VERTICES, sizeof(LEFT_BRACE_VERTICES) / sizeof(Vertex),
  LEFT_BRACE_SEGMENTS, sizeof(LEFT_BRACE_SEGMENTS) / sizeof(unsigned short),
  LEFT_BRACE_CALC, sizeof(LEFT_BRACE_CALC) / sizeof(Calculation),
  LEFT_BRACE_DEFAULT_ADJUST, sizeof(LEFT_BRACE_DEFAULT_ADJUST) / sizeof(int),
  LEFT_BRACE_TRS, sizeof(LEFT_BRACE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  LEFT_BRACE_GLUE_POINTS, sizeof(LEFT_BRACE_GLUE_POINTS) / sizeof(Vertex));

const Vertex RIGHT_BRACE_VERTICES[] =
{
  Vertex(0, 0), Vertex(5400, 0), Vertex(10800, 0 CALCULATED_VALUE), Vertex(10800, 1 CALCULATED_VALUE), Vertex(10800, 2 CALCULATED_VALUE), Vertex(10800, 3 CALCULATED_VALUE), Vertex(16200, 4 CALCULATED_VALUE), Vertex(21600, 4 CALCULATED_VALUE), Vertex(16200, 4 CALCULATED_VALUE), Vertex(10800, 5 CALCULATED_VALUE), Vertex(10800, 6 CALCULATED_VALUE), Vertex(10800, 7 CALCULATED_VALUE), Vertex(10800, 8 CALCULATED_VALUE), Vertex(5400, 21600), Vertex(0, 21600)
};

const unsigned short RIGHT_BRACE_SEGMENTS[] =
{
  0x4000, 0x2001, 0x0001, 0x2002, 0x0001, 0x2001, 0x8000
};

const Calculation RIGHT_BRACE_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0xa000, 0x404, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x404, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x6000, 0x404, 0x400, 0), Calculation(0x6000, 0x404, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 10000, 31953), Calculation(0x8000, 21600, 0, 0x409)
};

const TextRectangle RIGHT_BRACE_TRS[] =
{
  TextRectangle(Vertex(0, 9 CALCULATED_VALUE), Vertex(7800, 10 CALCULATED_VALUE))
};

const int RIGHT_BRACE_DEFAULT_ADJUST[] =
{
  1800, 10800
};

const Vertex RIGHT_BRACE_GLUE_POINTS[] =
{
  Vertex(0, 0), Vertex(0, 21600), Vertex(21600, 10800)
};

const CustomShape CS_RIGHT_BRACE(
  RIGHT_BRACE_VERTICES, sizeof(RIGHT_BRACE_VERTICES) / sizeof(Vertex),
  RIGHT_BRACE_SEGMENTS, sizeof(RIGHT_BRACE_SEGMENTS) / sizeof(unsigned short),
  RIGHT_BRACE_CALC, sizeof(RIGHT_BRACE_CALC) / sizeof(Calculation),
  RIGHT_BRACE_DEFAULT_ADJUST, sizeof(RIGHT_BRACE_DEFAULT_ADJUST) / sizeof(int),
  RIGHT_BRACE_TRS, sizeof(RIGHT_BRACE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  RIGHT_BRACE_GLUE_POINTS, sizeof(RIGHT_BRACE_GLUE_POINTS) / sizeof(Vertex));

const Vertex LEFT_UP_ARROW_VERTICES[] =
{
  Vertex(0, 5 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 0), Vertex(21600, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 21600)
};

const unsigned short LEFT_UP_ARROW_SEGMENTS[] =
{
  0x4000, 0x000b, 0x6001, 0x8000
};

const Calculation LEFT_UP_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, 0x0404, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, 0x0406, 0), Calculation(0x8000, 21600, 0, 0x406), Calculation(0xa000, 0x408, 0, 0x406)
};

const TextRectangle LEFT_UP_ARROW_TRS[] =
{
  TextRectangle(Vertex(2 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE)), TextRectangle(Vertex(7 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE))
};

const int LEFT_UP_ARROW_DEFAULT_ADJUST[] =
{
  9340, 18500, 6200
};

const CustomShape CS_LEFT_UP_ARROW(
  LEFT_UP_ARROW_VERTICES, sizeof(LEFT_UP_ARROW_VERTICES) / sizeof(Vertex),
  LEFT_UP_ARROW_SEGMENTS, sizeof(LEFT_UP_ARROW_SEGMENTS) / sizeof(unsigned short),
  LEFT_UP_ARROW_CALC, sizeof(LEFT_UP_ARROW_CALC) / sizeof(Calculation),
  LEFT_UP_ARROW_DEFAULT_ADJUST, sizeof(LEFT_UP_ARROW_DEFAULT_ADJUST) / sizeof(int),
  LEFT_UP_ARROW_TRS, sizeof(LEFT_UP_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex SMILEY_FACE_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360), Vertex(7305, 7515), Vertex(1165, 1165), Vertex(0, 360), Vertex(14295, 7515), Vertex(1165, 1165), Vertex(0, 360), Vertex(4870, 1 CALCULATED_VALUE), Vertex(8680, 2 CALCULATED_VALUE), Vertex(12920, 2 CALCULATED_VALUE), Vertex(16730, 1 CALCULATED_VALUE)
};

const unsigned short SMILEY_FACE_SEGMENTS[] =
{
  0xa203, 0x6000, 0x8000, 0xa203, 0x6000, 0x8000, 0xa203, 0x6000, 0x8000, 0x4000, 0x2001, 0xaa00, 0x8000
};

const Calculation SMILEY_FACE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 15510), Calculation(0x8000, 17520, 0, 0x400), Calculation(0x4000, 15510, 0x400, 0)
};

const TextRectangle SMILEY_FACE_TRS[] =
{
  TextRectangle(Vertex(3163, 3163), Vertex(18437, 18437))
};

const int SMILEY_FACE_DEFAULT_ADJUST[] =
{
  17520
};

const Vertex SMILEY_FACE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const Vertex VERTICAL_SCROLL_VERTICES[] =
{
  Vertex(1 CALCULATED_VALUE, 21600), Vertex(0, 11 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0), Vertex(2 CALCULATED_VALUE, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 21600), Vertex(6 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0, 11 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0), Vertex(6 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE)
};

const unsigned short VERTICAL_SCROLL_SEGMENTS[] =
{
  0x4000, 0xa702, 0x0002, 0xa801, 0x0001, 0xa702, 0x0002, 0xa801, 0x6001, 0x8000, 0x4000, 0xa801, 0xa702, 0x6000, 0x8000, 0x4000, 0xa803, 0xa702, 0x6001, 0x8000, 0x4000, 0xa701, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation VERTICAL_SCROLL_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 1, 2), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x401), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x400), Calculation(0x6000, 0x400, 0x401, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x404), Calculation(0x2001, 0x400, 2, 1), Calculation(0x2001, 0x401, 1, 2), Calculation(0x6000, 0x400, 0x407, 0), Calculation(0x6000, 0x401, 0x407, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x409), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x401), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x404)
};

const TextRectangle VERTICAL_SCROLL_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 12 CALCULATED_VALUE))
};

const int VERTICAL_SCROLL_DEFAULT_ADJUST[] =
{
  2700
};

const Vertex HORIZONTAL_SCROLL_VERTICES[] =
{
  Vertex(0, 4 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(21600, 13 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0, 11 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 11 CALCULATED_VALUE)
};

const unsigned short HORIZONTAL_SCROLL_SEGMENTS[] =
{
  0x4000, 0xa801, 0x0002, 0xa802, 0x0001, 0xa801, 0x0002, 0xa802, 0x6001, 0x8000, 0x4000, 0xa803, 0x6000, 0x8000, 0x4000, 0xa803, 0xa702, 0x6000, 0x8000, 0x4000, 0xa701, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation HORIZONTAL_SCROLL_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 1, 2), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x401), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x400), Calculation(0x6000, 0x400, 0x401, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x404), Calculation(0x2001, 0x400, 2, 1), Calculation(0x2001, 0x401, 1, 2), Calculation(0x6000, 0x400, 0x407, 0), Calculation(0x6000, 0x401, 0x407, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x409), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x401), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x404)
};

const TextRectangle HORIZONTAL_SCROLL_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 12 CALCULATED_VALUE))
};

const int HORIZONTAL_SCROLL_DEFAULT_ADJUST[] =
{
  2700
};

const Vertex CIRCULAR_ARROW_VERTICES[] =
{
  Vertex(0x03 CALCULATED_VALUE, 0x03 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x11 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0, 0), Vertex(21600, 21600), Vertex(0x09 CALCULATED_VALUE, 0x08 CALCULATED_VALUE), Vertex(0x0B CALCULATED_VALUE, 0x0A CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x17 CALCULATED_VALUE), Vertex(0x2F CALCULATED_VALUE, 0x2E CALCULATED_VALUE), Vertex(0x1D CALCULATED_VALUE, 0x1C CALCULATED_VALUE)
};

const unsigned short CIRCULAR_ARROW_SEGMENTS[] =
{
  0xa404, 0xa504, 0x0003, 0x6001, 0x8000
};

const Calculation CIRCULAR_ARROW_CALC[] =
{
  Calculation(0x2000, 0x0147, 0x0000, 0x0000), Calculation(0x2000, 0x0148, 0x0000, 0x0000), Calculation(0x2000, 0x0149, 0x0000, 0x0000), Calculation(0x4000, 0x2A30, 0x0149, 0x0000), Calculation(0x4009, 0x2A30, 0x0147, 0x0000), Calculation(0x400A, 0x2A30, 0x0147, 0x0000), Calculation(0x4009, 0x2A30, 0x0148, 0x0000), Calculation(0x400A, 0x2A30, 0x0148, 0x0000), Calculation(0x2000, 0x0404, 0x2A30, 0x0000), Calculation(0x2000, 0x0405, 0x2A30, 0x0000), Calculation(0x2000, 0x0406, 0x2A30, 0x0000), Calculation(0x2000, 0x0407, 0x2A30, 0x0000), Calculation(0x6009, 0x0403, 0x0147, 0x0000), Calculation(0x600A, 0x0403, 0x0147, 0x0000), Calculation(0x6009, 0x0403, 0x0148, 0x0000), Calculation(0x600A, 0x0403, 0x0148, 0x0000), Calculation(0x2000, 0x040C, 0x2A30, 0x0000), Calculation(0x2000, 0x040D, 0x2A30, 0x0000), Calculation(0x2000, 0x040E, 0x2A30, 0x0000), Calculation(0x2000, 0x040F, 0x2A30, 0x0000), Calculation(0x8000, 0x5460, 0x0000, 0x0403), Calculation(0x4009, 0x34BC, 0x0148, 0x0000), Calculation(0x400A, 0x34BC, 0x0148, 0x0000), Calculation(0x2000, 0x0415, 0x2A30, 0x0000), Calculation(0x2000, 0x0416, 0x2A30, 0x0000), Calculation(0x2000, 0x0149, 0x0000, 0x0A8C), Calculation(0x6009, 0x0419, 0x0148, 0x0000), Calculation(0x600A, 0x0419, 0x0148, 0x0000), Calculation(0x2000, 0x041A, 0x2A30, 0x0000), Calculation(0x2000, 0x041B, 0x2A30, 0x0000), Calculation(0xA000, 0x041D, 0x0000, 0x0418), Calculation(0xA000, 0x041D, 0x0000, 0x0418), Calculation(0x6001, 0x041E, 0x041F, 0x0001), Calculation(0xA000, 0x041C, 0x0000, 0x0417), Calculation(0xA000, 0x041C, 0x0000, 0x0417), Calculation(0x6001, 0x0421, 0x0422, 0x0001), Calculation(0x6000, 0x0420, 0x0423, 0x0000), Calculation(0x200D, 0x0424, 0x0000, 0x0000), Calculation(0x200E, 0x0148, 0x002D, 0x0000), Calculation(0x6009, 0x0425, 0x0426, 0x0000), Calculation(0x200E, 0x0148, 0x002D, 0x0000), Calculation(0x600A, 0x0425, 0x0428, 0x0000), Calculation(0x000E, 0x0000, 0x002D, 0x0000), Calculation(0x6009, 0x0427, 0x042A, 0x0000), Calculation(0x000E, 0x0000, 0x002D, 0x0000), Calculation(0x6009, 0x0429, 0x042C, 0x0000), Calculation(0x6000, 0x041C, 0x042B, 0x0000), Calculation(0x6000, 0x041D, 0x042D, 0x0000)
};

const TextRectangle CIRCULAR_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int CIRCULAR_ARROW_DEFAULT_ADJUST[] =
{
  180, 0, 5550
};

const CustomShape CS_CIRCULAR_ARROW(
  CIRCULAR_ARROW_VERTICES, sizeof(CIRCULAR_ARROW_VERTICES) / sizeof(Vertex),
  CIRCULAR_ARROW_SEGMENTS, sizeof(CIRCULAR_ARROW_SEGMENTS) / sizeof(unsigned short),
  CIRCULAR_ARROW_CALC, sizeof(CIRCULAR_ARROW_CALC) / sizeof(Calculation),
  CIRCULAR_ARROW_DEFAULT_ADJUST, sizeof(CIRCULAR_ARROW_DEFAULT_ADJUST) / sizeof(int),
  CIRCULAR_ARROW_TRS, sizeof(CIRCULAR_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex U_TURN_ARROW_VERTICES[] =
{
  Vertex(0, 21600), Vertex(0, 8550), Vertex(0, 3540), Vertex(4370, 0), Vertex(9270, 0), Vertex(13890, 0), Vertex(18570, 3230), Vertex(18600, 8300), Vertex(21600, 8300), Vertex(15680, 14260), Vertex(9700, 8300), Vertex(12500, 8300), Vertex(12320, 6380), Vertex(10870, 5850), Vertex(9320, 5850), Vertex(7770, 5850), Vertex(6040, 6410), Vertex(6110, 8520), Vertex(6110, 21600)
};

const unsigned short U_TURN_ARROW_SEGMENTS[] =
{
  0x4000, 0x0001, 0x2002, 0x0004, 0x2002, 0x0001, 0x6000, 0x8000
};

const TextRectangle U_TURN_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 8280), Vertex(6110, 21600))
};

const CustomShape CS_U_TURN_ARROW(
  U_TURN_ARROW_VERTICES, sizeof(U_TURN_ARROW_VERTICES) / sizeof(Vertex),
  U_TURN_ARROW_SEGMENTS, sizeof(U_TURN_ARROW_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  U_TURN_ARROW_TRS, sizeof(U_TURN_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex CURVED_RIGHT_ARROW_VERTICES[] =
{
  Vertex(0, 0), Vertex(23 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 0), Vertex(0, 4 CALCULATED_VALUE), Vertex(0, 15 CALCULATED_VALUE), Vertex(23 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0, 7 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(0, 0), Vertex(23 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(26 CALCULATED_VALUE, 17 CALCULATED_VALUE), Vertex(0, 15 CALCULATED_VALUE), Vertex(23 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(26 CALCULATED_VALUE, 17 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(0, 0), Vertex(23 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 0), Vertex(0, 4 CALCULATED_VALUE), Vertex(0, 0), Vertex(23 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(26 CALCULATED_VALUE, 17 CALCULATED_VALUE), Vertex(0, 15 CALCULATED_VALUE), Vertex(23 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(26 CALCULATED_VALUE, 17 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 15 CALCULATED_VALUE)
};

const unsigned short CURVED_RIGHT_ARROW_SEGMENTS[] =
{
  0xa408, 0x0003, 0xa508, 0x6000, 0x8000, 0xa404, 0xa304, 0xa504, 0x6000, 0x8000
};

const Calculation CURVED_RIGHT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0xa000, PROP_ADJUST_VAL_FIRST, 21600, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x2001, 0x403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0xe000, 0x405, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x406, 1, 2), Calculation(0x4002, 21600, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 2), Calculation(0xa00f, 0x409, 21600, 0x404), Calculation(0x6000, 0x404, 0x40a, 0), Calculation(0x6000, 0x40b, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6000, 0x407, 0x40a, 0), Calculation(0xa000, 0x40c, 21600, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x405, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x40f, 1, 2), Calculation(0x6002, 0x404, 0x407, 0), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x2001, 0x412, 1, 2), Calculation(0xa000, 0x411, 0, 0x413), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 21600, 0, 0), Calculation(0x0001, 21600, 2, 1), Calculation(0xa000, 0x411, 0, 0x404), Calculation(0x600f, 0x418, 0x404, 21600), Calculation(0x8000, 21600, 0, 0x419), Calculation(0x2000, 0x408, 128, 0), Calculation(0x2001, 0x405, 1, 2), Calculation(0x2000, 0x405, 0, 128), Calculation(0xe000, PROP_ADJUST_VAL_FIRST, 0x411, 0x40c), Calculation(0x600f, 0x414, 0x404, 21600), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x420, 1, 2), Calculation(0x0001, 21600, 21600, 1), Calculation(0x6001, 0x409, 0x409, 1), Calculation(0xa000, 0x422, 0, 0x423), Calculation(0x200d, 0x424, 0, 0), Calculation(0x2000, 0x425, 21600, 0), Calculation(0x8001, 21600, 21600, 0x426), Calculation(0x2000, 0x427, 64, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x600f, 0x421, 0x429, 21600), Calculation(0x8000, 21600, 0, 0x42a), Calculation(0x2000, 0x42b, 64, 0), Calculation(0x2001, 0x404, 1, 2), Calculation(0xa000, PROP_ADJUST_VAL_FIRST + 1, 0, 0x42d), Calculation(0x0001, 21600, 2195, 16384), Calculation(0x0001, 21600, 14189, 16384)
};

const TextRectangle CURVED_RIGHT_ARROW_TRS[] =
{
  TextRectangle(Vertex(47 CALCULATED_VALUE, 45 CALCULATED_VALUE), Vertex(48 CALCULATED_VALUE, 46 CALCULATED_VALUE))
};

const int CURVED_RIGHT_ARROW_DEFAULT_ADJUST[] =
{
  12960, 19440, 14400
};

const Vertex CURVED_RIGHT_ARROW_GLUE_POINTS[] =
{
  Vertex(0, 17 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 16 CALCULATED_VALUE)
};

const CustomShape CS_CURVED_RIGHT_ARROW(
  CURVED_RIGHT_ARROW_VERTICES, sizeof(CURVED_RIGHT_ARROW_VERTICES) / sizeof(Vertex),
  CURVED_RIGHT_ARROW_SEGMENTS, sizeof(CURVED_RIGHT_ARROW_SEGMENTS) / sizeof(unsigned short),
  CURVED_RIGHT_ARROW_CALC, sizeof(CURVED_RIGHT_ARROW_CALC) / sizeof(Calculation),
  CURVED_RIGHT_ARROW_DEFAULT_ADJUST, sizeof(CURVED_RIGHT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  CURVED_RIGHT_ARROW_TRS, sizeof(CURVED_RIGHT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  CURVED_RIGHT_ARROW_GLUE_POINTS, sizeof(CURVED_RIGHT_ARROW_GLUE_POINTS) / sizeof(Vertex));

const Vertex CURVED_LEFT_ARROW_VERTICES[] =
{
  Vertex(22 CALCULATED_VALUE, 0), Vertex(21 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 0), Vertex(21 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(0, 8 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 0), Vertex(21 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(0, 14 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 0), Vertex(21 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 0), Vertex(21 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(22 CALCULATED_VALUE, 14 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(0, 14 CALCULATED_VALUE)
};

const unsigned short CURVED_LEFT_ARROW_SEGMENTS[] =
{
  0xa608, 0x0003, 0xa308, 0x6000, 0x8000, 0xa604, 0xa308, 0x6000, 0x8000
};

const Calculation CURVED_LEFT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0xa000, PROP_ADJUST_VAL_FIRST, 21600, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x2001, 0x403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0xe000, 0x405, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x406, 1, 2), Calculation(0x4002, 21600, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa00f, PROP_ADJUST_VAL_FIRST + 2, 21600, 0x404), Calculation(0x6000, 0x404, 0x409, 0), Calculation(0x6000, 0x40a, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6000, 0x407, 0x409, 0), Calculation(0xa000, 0x40b, 21600, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x405, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x40e, 1, 2), Calculation(0x6002, 0x404, 0x407, 0), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x2001, 0x411, 1, 2), Calculation(0xa000, 0x410, 0, 0x412), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 0, 0, 21600), Calculation(0xa000, 0x410, 0, 0x404), Calculation(0x600f, 0x417, 0x404, 21600), Calculation(0x2000, 0x408, 128, 0), Calculation(0x2001, 0x405, 1, 2), Calculation(0x2000, 0x405, 0, 128), Calculation(0xe000, PROP_ADJUST_VAL_FIRST, 0x410, 0x40b), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x41d, 1, 2), Calculation(0x0001, 21600, 21600, 1), Calculation(0x6001, PROP_ADJUST_VAL_FIRST + 2, PROP_ADJUST_VAL_FIRST + 2, 1), Calculation(0xa000, 0x41f, 0, 0x420), Calculation(0x200d, 0x421, 0, 0), Calculation(0x2000, 0x422, 21600, 0), Calculation(0x8001, 21600, 21600, 0x423), Calculation(0x2000, 0x424, 64, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x600f, 0x41e, 0x426, 21600), Calculation(0x2000, 0x427, 0, 64), Calculation(0x2001, 0x404, 1, 2), Calculation(0xa000, PROP_ADJUST_VAL_FIRST + 1, 0, 0x429), Calculation(0x0001, 21600, 2195, 16384), Calculation(0x0001, 21600, 14189, 16384)
};

const TextRectangle CURVED_LEFT_ARROW_TRS[] =
{
  TextRectangle(Vertex(43 CALCULATED_VALUE, 41 CALCULATED_VALUE), Vertex(44 CALCULATED_VALUE, 42 CALCULATED_VALUE))
};

const int CURVED_LEFT_ARROW_DEFAULT_ADJUST[] =
{
  12960, 19440, 7200
};

const Vertex CURVED_LEFT_ARROW_GLUE_POINTS[] =
{
  Vertex(0, 15 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(0, 8 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(21 CALCULATED_VALUE, 16 CALCULATED_VALUE)
};


const Vertex CURVED_DOWN_ARROW_VERTICES[] =
{
  Vertex(0, 0), Vertex(3 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(0, 22 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0), Vertex(15 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0), Vertex(13 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0, 0), Vertex(3 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(0, 0), Vertex(3 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(0, 22 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0), Vertex(0, 0), Vertex(3 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 0), Vertex(17 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 23 CALCULATED_VALUE), Vertex(17 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 22 CALCULATED_VALUE)
};

const unsigned short CURVED_DOWN_ARROW_SEGMENTS[] =
{
  0xa608, 0x0003, 0xa308, 0x6000, 0x8000, 0xa604, 0xa504, 0xa304, 0x6000, 0x8000
};

const Calculation CURVED_DOWN_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0xa000, PROP_ADJUST_VAL_FIRST, 21600, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x2001, 0x403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0xe000, 0x405, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x406, 1, 2), Calculation(0x4002, 21600, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 2), Calculation(0xa00f, 0x409, 21600, 0x404), Calculation(0x6000, 0x404, 0x40a, 0), Calculation(0x6000, 0x40b, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6000, 0x407, 0x40a, 0), Calculation(0xa000, 0x40c, 21600, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x405, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x40f, 1, 2), Calculation(0x6002, 0x404, 0x407, 0), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x2001, 0x412, 1, 2), Calculation(0xa000, 0x411, 0, 0x413), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 21600, 0, 0), Calculation(0x0001, 21600, 2, 1), Calculation(0xa000, 0x411, 0, 0x404), Calculation(0x600f, 0x418, 0x404, 21600), Calculation(0x8000, 21600, 0, 0x419), Calculation(0x2000, 0x408, 128, 0), Calculation(0x2001, 0x405, 1, 2), Calculation(0x2000, 0x405, 0, 128), Calculation(0xe000, PROP_ADJUST_VAL_FIRST, 0x411, 0x40c), Calculation(0x600f, 0x414, 0x404, 21600), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x420, 1, 2), Calculation(0x0001, 21600, 21600, 1), Calculation(0x6001, 0x409, 0x409, 1), Calculation(0xa000, 0x422, 0, 0x423), Calculation(0x200d, 0x424, 0, 0), Calculation(0x2000, 0x425, 21600, 0), Calculation(0x8001, 21600, 21600, 0x426), Calculation(0x2000, 0x427, 64, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x600f, 0x421, 0x429, 21600), Calculation(0x8000, 21600, 0, 0x42a), Calculation(0x2000, 0x42b, 64, 0), Calculation(0x2001, 0x404, 1, 2), Calculation(0xa000, PROP_ADJUST_VAL_FIRST + 1, 0, 0x42d), Calculation(0x0001, 21600, 2195, 16384), Calculation(0x0001, 21600, 14189, 16384)
};

const TextRectangle CURVED_DOWN_ARROW_TRS[] =
{
  TextRectangle(Vertex(45 CALCULATED_VALUE, 47 CALCULATED_VALUE), Vertex(46 CALCULATED_VALUE, 48 CALCULATED_VALUE))
};

const int CURVED_DOWN_ARROW_DEFAULT_ADJUST[] =
{
  12960, 19440, 14400
};

const Vertex CURVED_DOWN_ARROW_GLUE_POINTS[] =
{
  Vertex(17 CALCULATED_VALUE, 0), Vertex(16 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const CustomShape CS_CURVED_DOWN_ARROW(
  CURVED_DOWN_ARROW_VERTICES, sizeof(CURVED_DOWN_ARROW_VERTICES) / sizeof(Vertex),
  CURVED_DOWN_ARROW_SEGMENTS, sizeof(CURVED_DOWN_ARROW_SEGMENTS) / sizeof(unsigned short),
  CURVED_DOWN_ARROW_CALC, sizeof(CURVED_DOWN_ARROW_CALC) / sizeof(Calculation),
  CURVED_DOWN_ARROW_DEFAULT_ADJUST, sizeof(CURVED_DOWN_ARROW_DEFAULT_ADJUST) / sizeof(int),
  CURVED_DOWN_ARROW_TRS, sizeof(CURVED_DOWN_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  CURVED_DOWN_ARROW_GLUE_POINTS, sizeof(CURVED_DOWN_ARROW_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_PREDEFINED_PROCESS_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(2540, 0), Vertex(2540, 21600), Vertex(21600 - 2540, 0), Vertex(21600 - 2540, 21600)
};

const unsigned short FLOW_CHART_PREDEFINED_PROCESS_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6000, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const TextRectangle FLOW_CHART_PREDEFINED_PROCESS_TRS[] =
{
  TextRectangle(Vertex(2540, 0), Vertex(21600 - 2540, 21600))
};

const Vertex FLOW_CHART_INTERNAL_STORAGE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(4230, 0), Vertex(4230, 21600), Vertex(0, 4230), Vertex(21600, 4230)
};

const unsigned short FLOW_CHART_INTERNAL_STORAGE_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6000, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const TextRectangle FLOW_CHART_INTERNAL_STORAGE_TRS[] =
{
  TextRectangle(Vertex(4230, 4230), Vertex(21600, 21600))
};

const CustomShape CS_FLOW_CHART_INTERNAL_STORAGE(
  FLOW_CHART_INTERNAL_STORAGE_VERTICES, sizeof(FLOW_CHART_INTERNAL_STORAGE_VERTICES) / sizeof(Vertex),
  FLOW_CHART_INTERNAL_STORAGE_SEGMENTS, sizeof(FLOW_CHART_INTERNAL_STORAGE_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_INTERNAL_STORAGE_TRS, sizeof(FLOW_CHART_INTERNAL_STORAGE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex FLOW_CHART_DOCUMENT_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 17360), Vertex(13050, 17220), Vertex(13340, 20770), Vertex(5620, 21600), Vertex(2860, 21100), Vertex(1850, 20700), Vertex(0, 20120)
};

const unsigned short FLOW_CHART_DOCUMENT_SEGMENTS[] =
{
  0x4000, 0x0002, 0x2002, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_DOCUMENT_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 17360))
};

const Vertex FLOW_CHART_DOCUMENT_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 20320), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_DOCUMENT(
  FLOW_CHART_DOCUMENT_VERTICES, sizeof(FLOW_CHART_DOCUMENT_VERTICES) / sizeof(Vertex),
  FLOW_CHART_DOCUMENT_SEGMENTS, sizeof(FLOW_CHART_DOCUMENT_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_DOCUMENT_TRS, sizeof(FLOW_CHART_DOCUMENT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_DOCUMENT_GLUE_POINTS, sizeof(FLOW_CHART_DOCUMENT_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MULTI_DOCUMENT_VERTICES[] =
{
  Vertex(0, 3600), Vertex(1500, 3600), Vertex(1500, 1800), Vertex(3000, 1800), Vertex(3000, 0), Vertex(21600, 0), Vertex(21600, 14409), Vertex(21600 - 1500, 14409), Vertex(21600 - 1500, 14409 + 1800), Vertex(21600 - 3000, 14409 + 1800), Vertex(21600 - 3000, 14409 + 3600), Vertex(11610, 14293 + 3600), Vertex(11472, 17239 + 3600), Vertex(4833, 17928 + 3600), Vertex(2450, 17513 + 3600), Vertex(1591, 17181 + 3600), Vertex(0, 16700 + 3600), Vertex(1500, 3600), Vertex(21600 - 3000, 3600), Vertex(21600 - 3000, 14409 + 1800), Vertex(3000, 1800), Vertex(21600 - 1500, 1800), Vertex(21600 - 1500, 14409)
};

const unsigned short FLOW_CHART_MULTI_DOCUMENT_SEGMENTS[] =
{
  0x4000, 0x000a, 0x2002, 0x6000, 0x8000, 0x4000, 0xaa00, 0x0002, 0x8000, 0x4000, 0xaa00, 0x0002, 0x8000
};

const TextRectangle FLOW_CHART_MULTI_DOCUMENT_TRS[] =
{
  TextRectangle(Vertex(0, 3600), Vertex(21600 - 3600, 14409 + 3600))
};

const Vertex FLOW_CHART_MULTI_DOCUMENT_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 19890), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_MULTI_DOCUMENT(
  FLOW_CHART_MULTI_DOCUMENT_VERTICES, sizeof(FLOW_CHART_MULTI_DOCUMENT_VERTICES) / sizeof(Vertex),
  FLOW_CHART_MULTI_DOCUMENT_SEGMENTS, sizeof(FLOW_CHART_MULTI_DOCUMENT_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MULTI_DOCUMENT_TRS, sizeof(FLOW_CHART_MULTI_DOCUMENT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MULTI_DOCUMENT_GLUE_POINTS, sizeof(FLOW_CHART_MULTI_DOCUMENT_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_TERMINATOR_VERTICES[] =
{
  Vertex(3470, 21600), Vertex(0, 10800), Vertex(3470, 0), Vertex(18130, 0), Vertex(21600, 10800), Vertex(18130, 21600)
};

const unsigned short FLOW_CHART_TERMINATOR_SEGMENTS[] =
{
  0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_TERMINATOR_TRS[] =
{
  TextRectangle(Vertex(1060, 3180), Vertex(20540, 18420))
};

const Vertex FLOW_CHART_TERMINATOR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_TERMINATOR(
  FLOW_CHART_TERMINATOR_VERTICES, sizeof(FLOW_CHART_TERMINATOR_VERTICES) / sizeof(Vertex),
  FLOW_CHART_TERMINATOR_SEGMENTS, sizeof(FLOW_CHART_TERMINATOR_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_TERMINATOR_TRS, sizeof(FLOW_CHART_TERMINATOR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_TERMINATOR_GLUE_POINTS, sizeof(FLOW_CHART_TERMINATOR_GLUE_POINTS) / sizeof(Vertex));

const CustomShape CS_FLOW_CHART_PREDEFINED_PROCESS(
  FLOW_CHART_PREDEFINED_PROCESS_VERTICES, sizeof(FLOW_CHART_PREDEFINED_PROCESS_VERTICES) / sizeof(Vertex),
  FLOW_CHART_PREDEFINED_PROCESS_SEGMENTS, sizeof(FLOW_CHART_PREDEFINED_PROCESS_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_PREDEFINED_PROCESS_TRS, sizeof(FLOW_CHART_PREDEFINED_PROCESS_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex FLOW_CHART_IO_VERTICES[] =
{
  Vertex(4230, 0), Vertex(21600, 0), Vertex(17370, 21600), Vertex(0, 21600), Vertex(4230, 0)
};

const TextRectangle FLOW_CHART_IO_TRS[] =
{
  TextRectangle(Vertex(4230, 0), Vertex(17370, 21600))
};

const Vertex FLOW_CHART_IO_GLUE_POINTS[] =
{
  Vertex(12960, 0), Vertex(10800, 0), Vertex(2160, 10800), Vertex(8600, 21600), Vertex(10800, 21600), Vertex(19400, 10800)
};

const CustomShape CS_FLOW_CHART_IO(
  FLOW_CHART_IO_VERTICES, sizeof(FLOW_CHART_IO_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_IO_TRS, sizeof(FLOW_CHART_IO_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_IO_GLUE_POINTS, sizeof(FLOW_CHART_IO_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_PROCESS_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0)
};

const Vertex FLOW_CHART_PROCESS_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const Vertex FLOW_CHART_DECISION_VERTICES[] =
{
  Vertex(0, 10800), Vertex(10800, 0), Vertex(21600, 10800), Vertex(10800, 21600), Vertex(0, 10800)
};

const TextRectangle FLOW_CHART_DECISION_TRS[] =
{
  TextRectangle(Vertex(5400, 5400), Vertex(16200, 16200))
};

const Vertex FLOW_CHART_DECISION_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_DECISION(
  FLOW_CHART_DECISION_VERTICES, sizeof(FLOW_CHART_DECISION_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_DECISION_TRS, sizeof(FLOW_CHART_DECISION_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_DECISION_GLUE_POINTS, sizeof(FLOW_CHART_DECISION_GLUE_POINTS) / sizeof(Vertex));

const CustomShape CS_FLOW_CHART_PROCESS(
  FLOW_CHART_PROCESS_VERTICES, sizeof(FLOW_CHART_PROCESS_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  NULL, 0,
  21600, 21600,
  FLOW_CHART_PROCESS_GLUE_POINTS, sizeof(FLOW_CHART_PROCESS_GLUE_POINTS) / sizeof(Vertex));

const Vertex CURVED_UP_ARROW_VERTICES[] =
{
  Vertex(0, 22 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(0, 0), Vertex(4 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(13 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0, 22 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 0), Vertex(0, 22 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(0, 0), Vertex(4 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 24 CALCULATED_VALUE), Vertex(14 CALCULATED_VALUE, 0)
};

const unsigned short CURVED_UP_ARROW_SEGMENTS[] =
{
  0xa408, 0x0003, 0xa508, 0x6000, 0x8000, 0xa404, 0xa508, 0xa504, 0x6000, 0x8000
};

const Calculation CURVED_UP_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0xa000, PROP_ADJUST_VAL_FIRST, 21600, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x2001, 0x403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0xe000, 0x405, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x406, 1, 2), Calculation(0x4002, 21600, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa00f, PROP_ADJUST_VAL_FIRST + 2, 21600, 0x404), Calculation(0x6000, 0x404, 0x409, 0), Calculation(0x6000, 0x40a, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x6000, 0x407, 0x409, 0), Calculation(0xa000, 0x40b, 21600, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x405, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x40e, 1, 2), Calculation(0x6002, 0x404, 0x407, 0), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, PROP_ADJUST_VAL_FIRST + 1, 21600), Calculation(0x2001, 0x411, 1, 2), Calculation(0xa000, 0x410, 0, 0x412), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 21600, 0, 0), Calculation(0x0000, 0, 0, 21600), Calculation(0xa000, 0x410, 0, 0x404), Calculation(0x600f, 0x417, 0x404, 21600), Calculation(0x2000, 0x408, 128, 0), Calculation(0x2001, 0x405, 1, 2), Calculation(0x2000, 0x405, 0, 128), Calculation(0xe000, PROP_ADJUST_VAL_FIRST, 0x410, 0x40b), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x41d, 1, 2), Calculation(0x0001, 21600, 21600, 1), Calculation(0x6001, PROP_ADJUST_VAL_FIRST + 2, PROP_ADJUST_VAL_FIRST + 2, 1), Calculation(0xa000, 0x41f, 0, 0x420), Calculation(0x200d, 0x421, 0, 0), Calculation(0x2000, 0x422, 21600, 0), Calculation(0x8001, 21600, 21600, 0x423), Calculation(0x2000, 0x424, 64, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x600f, 0x41e, 0x426, 21600), Calculation(0x2000, 0x427, 0, 64), Calculation(0x2001, 0x404, 1, 2), Calculation(0xa000, PROP_ADJUST_VAL_FIRST + 1, 0, 0x429), Calculation(0x0001, 21600, 2195, 16384), Calculation(0x0001, 21600, 14189, 16384)
};

const TextRectangle CURVED_UP_ARROW_TRS[] =
{
  TextRectangle(Vertex(41 CALCULATED_VALUE, 43 CALCULATED_VALUE), Vertex(42 CALCULATED_VALUE, 44 CALCULATED_VALUE))
};

const int CURVED_UP_ARROW_DEFAULT_ADJUST[] =
{
  12960, 19440, 7200
};

const Vertex CURVED_UP_ARROW_GLUE_POINTS[] =
{
  Vertex(8 CALCULATED_VALUE, 0), Vertex(11 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 0), Vertex(16 CALCULATED_VALUE, 21 CALCULATED_VALUE), Vertex(13 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const CustomShape CS_CURVED_UP_ARROW(
  CURVED_UP_ARROW_VERTICES, sizeof(CURVED_UP_ARROW_VERTICES) / sizeof(Vertex),
  CURVED_UP_ARROW_SEGMENTS, sizeof(CURVED_UP_ARROW_SEGMENTS) / sizeof(unsigned short),
  CURVED_UP_ARROW_CALC, sizeof(CURVED_UP_ARROW_CALC) / sizeof(Calculation),
  CURVED_UP_ARROW_DEFAULT_ADJUST, sizeof(CURVED_UP_ARROW_DEFAULT_ADJUST) / sizeof(int),
  CURVED_UP_ARROW_TRS, sizeof(CURVED_UP_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  CURVED_UP_ARROW_GLUE_POINTS, sizeof(CURVED_UP_ARROW_GLUE_POINTS) / sizeof(Vertex));

const CustomShape CS_CURVED_LEFT_ARROW(
  CURVED_LEFT_ARROW_VERTICES, sizeof(CURVED_LEFT_ARROW_VERTICES) / sizeof(Vertex),
  CURVED_LEFT_ARROW_SEGMENTS, sizeof(CURVED_LEFT_ARROW_SEGMENTS) / sizeof(unsigned short),
  CURVED_LEFT_ARROW_CALC, sizeof(CURVED_LEFT_ARROW_CALC) / sizeof(Calculation),
  CURVED_LEFT_ARROW_DEFAULT_ADJUST, sizeof(CURVED_LEFT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  CURVED_LEFT_ARROW_TRS, sizeof(CURVED_LEFT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  CURVED_LEFT_ARROW_GLUE_POINTS, sizeof(CURVED_LEFT_ARROW_GLUE_POINTS) / sizeof(Vertex));

const CustomShape CS_HORIZONTAL_SCROLL(
  HORIZONTAL_SCROLL_VERTICES, sizeof(HORIZONTAL_SCROLL_VERTICES) / sizeof(Vertex),
  HORIZONTAL_SCROLL_SEGMENTS, sizeof(HORIZONTAL_SCROLL_SEGMENTS) / sizeof(unsigned short),
  HORIZONTAL_SCROLL_CALC, sizeof(HORIZONTAL_SCROLL_CALC) / sizeof(Calculation),
  HORIZONTAL_SCROLL_DEFAULT_ADJUST, sizeof(HORIZONTAL_SCROLL_DEFAULT_ADJUST) / sizeof(int),
  HORIZONTAL_SCROLL_TRS, sizeof(HORIZONTAL_SCROLL_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const CustomShape CS_VERTICAL_SCROLL(
  VERTICAL_SCROLL_VERTICES, sizeof(VERTICAL_SCROLL_VERTICES) / sizeof(Vertex),
  VERTICAL_SCROLL_SEGMENTS, sizeof(VERTICAL_SCROLL_SEGMENTS) / sizeof(unsigned short),
  VERTICAL_SCROLL_CALC, sizeof(VERTICAL_SCROLL_CALC) / sizeof(Calculation),
  VERTICAL_SCROLL_DEFAULT_ADJUST, sizeof(VERTICAL_SCROLL_DEFAULT_ADJUST) / sizeof(int),
  VERTICAL_SCROLL_TRS, sizeof(VERTICAL_SCROLL_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const CustomShape CS_SMILEY_FACE(
  SMILEY_FACE_VERTICES, sizeof(SMILEY_FACE_VERTICES) / sizeof(Vertex),
  SMILEY_FACE_SEGMENTS, sizeof(SMILEY_FACE_SEGMENTS) / sizeof(unsigned short),
  SMILEY_FACE_CALC, sizeof(SMILEY_FACE_CALC) / sizeof(Calculation),
  SMILEY_FACE_DEFAULT_ADJUST, sizeof(SMILEY_FACE_DEFAULT_ADJUST) / sizeof(int),
  SMILEY_FACE_TRS, sizeof(SMILEY_FACE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  SMILEY_FACE_GLUE_POINTS, sizeof(SMILEY_FACE_GLUE_POINTS) / sizeof(Vertex));

const Vertex BLOCK_ARC_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(9 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 10 CALCULATED_VALUE)
};

const unsigned short BLOCK_ARC_SEGMENTS[] =
{
  0xA404, 0xa504, 0x6001, 0x8000
};

const Calculation BLOCK_ARC_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0),
  Calculation(0x2000, 0x400, 10800, 0),
  Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402),
  Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1),
  Calculation(0x4000, 10800, PROP_ADJUST_VAL_FIRST + 1, 0),
  Calculation(0x600a, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST, 0),
  Calculation(0x6009, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST, 0),
  Calculation(0x4000, 10800, 0x407, 0), Calculation(0x4000, 10800, 0x408, 0), Calculation(0x8000, 10800,0, 0x407)
};

const int BLOCK_ARC_DEFAULT_ADJUST[] =
{
  180 << 16, 5400
};

const CustomShape CS_BLOCK_ARC(
  BLOCK_ARC_VERTICES, sizeof(BLOCK_ARC_VERTICES) / sizeof(Vertex),
  BLOCK_ARC_SEGMENTS, sizeof(BLOCK_ARC_SEGMENTS) / sizeof(unsigned short),
  BLOCK_ARC_CALC, sizeof(BLOCK_ARC_CALC) / sizeof(Calculation),
  BLOCK_ARC_DEFAULT_ADJUST, sizeof(BLOCK_ARC_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  NULL, 0, 0x1);

const Vertex NOTCHED_RIGHT_ARROW_VERTICES[] =
{
  Vertex(0, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 10800), Vertex(0, 1 CALCULATED_VALUE)
};

const Calculation NOTCHED_RIGHT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x6001, 0x403, 0x404, 10800)
};

const TextRectangle NOTCHED_RIGHT_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int NOTCHED_RIGHT_ARROW_DEFAULT_ADJUST[] =
{
  16200, 5400
};

const CustomShape CS_NOTCHED_RIGHT_ARROW(
  NOTCHED_RIGHT_ARROW_VERTICES, sizeof(NOTCHED_RIGHT_ARROW_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NOTCHED_RIGHT_ARROW_CALC, sizeof(NOTCHED_RIGHT_ARROW_CALC) / sizeof(Calculation),
  NOTCHED_RIGHT_ARROW_DEFAULT_ADJUST, sizeof(NOTCHED_RIGHT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  NOTCHED_RIGHT_ARROW_TRS, sizeof(NOTCHED_RIGHT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex STRIPED_RIGHT_ARROW_VERTICES[] =
{
  Vertex(3375, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(1 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3375, 2 CALCULATED_VALUE), Vertex(0, 0 CALCULATED_VALUE), Vertex(675, 0 CALCULATED_VALUE), Vertex(675, 2 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(1350, 0 CALCULATED_VALUE), Vertex(2700, 0 CALCULATED_VALUE), Vertex(2700, 2 CALCULATED_VALUE), Vertex(1350, 2 CALCULATED_VALUE)
};

const unsigned short STRIPED_RIGHT_ARROW_SEGMENTS[] =
{
  0x4000, 0x0006, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation STRIPED_RIGHT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x401), Calculation(0x6001, 0x403, 0x400, 10800), Calculation(0x6000, 0x401, 0x404, 0)
};

const TextRectangle STRIPED_RIGHT_ARROW_TRS[] =
{
  TextRectangle(Vertex(3375, 0 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 2 CALCULATED_VALUE))
};

const int STRIPED_RIGHT_ARROW_DEFAULT_ADJUST[] =
{
  16200, 5400
};

const CustomShape CS_STRIPED_RIGHT_ARROW(
  STRIPED_RIGHT_ARROW_VERTICES, sizeof(STRIPED_RIGHT_ARROW_VERTICES) / sizeof(Vertex),
  STRIPED_RIGHT_ARROW_SEGMENTS, sizeof(STRIPED_RIGHT_ARROW_SEGMENTS) / sizeof(unsigned short),
  STRIPED_RIGHT_ARROW_CALC, sizeof(STRIPED_RIGHT_ARROW_CALC) / sizeof(Calculation),
  STRIPED_RIGHT_ARROW_DEFAULT_ADJUST, sizeof(STRIPED_RIGHT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  STRIPED_RIGHT_ARROW_TRS, sizeof(STRIPED_RIGHT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex SEAL_24_VERTICES[] =
{
  Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE), Vertex(0x07 CALCULATED_VALUE, 0x08 CALCULATED_VALUE), Vertex(0x09 CALCULATED_VALUE, 0x0a CALCULATED_VALUE), Vertex(0x0b CALCULATED_VALUE, 0x0c CALCULATED_VALUE), Vertex(0x0d CALCULATED_VALUE, 0x0e CALCULATED_VALUE), Vertex(0x0f CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x11 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x17 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x19 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1b CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x1d CALCULATED_VALUE, 0x1e CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x23 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x25 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x27 CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0x29 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2b CALCULATED_VALUE, 0x2c CALCULATED_VALUE), Vertex(0x2d CALCULATED_VALUE, 0x2e CALCULATED_VALUE), Vertex(0x2f CALCULATED_VALUE, 0x30 CALCULATED_VALUE), Vertex(0x31 CALCULATED_VALUE, 0x32 CALCULATED_VALUE), Vertex(0x33 CALCULATED_VALUE, 0x34 CALCULATED_VALUE), Vertex(0x35 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0x37 CALCULATED_VALUE, 0x38 CALCULATED_VALUE), Vertex(0x39 CALCULATED_VALUE, 0x3a CALCULATED_VALUE), Vertex(0x3b CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(0x3d CALCULATED_VALUE, 0x3e CALCULATED_VALUE), Vertex(0x3f CALCULATED_VALUE, 0x40 CALCULATED_VALUE), Vertex(0x41 CALCULATED_VALUE, 0x42 CALCULATED_VALUE), Vertex(0x43 CALCULATED_VALUE, 0x44 CALCULATED_VALUE), Vertex(0x45 CALCULATED_VALUE, 0x46 CALCULATED_VALUE), Vertex(0x47 CALCULATED_VALUE, 0x48 CALCULATED_VALUE), Vertex(0x49 CALCULATED_VALUE, 0x4a CALCULATED_VALUE), Vertex(0x4b CALCULATED_VALUE, 0x4c CALCULATED_VALUE), Vertex(0x4d CALCULATED_VALUE, 0x4e CALCULATED_VALUE), Vertex(0x4f CALCULATED_VALUE, 0x50 CALCULATED_VALUE), Vertex(0x51 CALCULATED_VALUE, 0x52 CALCULATED_VALUE), Vertex(0x53 CALCULATED_VALUE, 0x54 CALCULATED_VALUE), Vertex(0x55 CALCULATED_VALUE, 0x56 CALCULATED_VALUE), Vertex(0x57 CALCULATED_VALUE, 0x58 CALCULATED_VALUE), Vertex(0x59 CALCULATED_VALUE, 0x5a CALCULATED_VALUE), Vertex(0x5b CALCULATED_VALUE, 0x5c CALCULATED_VALUE), Vertex(0x5d CALCULATED_VALUE, 0x5e CALCULATED_VALUE), Vertex(0x5f CALCULATED_VALUE, 0x60 CALCULATED_VALUE), Vertex(0x61 CALCULATED_VALUE, 0x62 CALCULATED_VALUE), Vertex(0x63 CALCULATED_VALUE, 0x64 CALCULATED_VALUE), Vertex(0x05 CALCULATED_VALUE, 0x06 CALCULATED_VALUE)
};

const Calculation SEAL_24_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2081, 0x400, 10800, 315), Calculation(0x2082, 0x400, 10800, 315), Calculation(0x2081, 0x400, 10800, 135), Calculation(0x2082, 0x400, 10800, 135), Calculation(0x0081, 0, 10800, 0), Calculation(0x0082, 0, 10800, 0), Calculation(0x2081, 0x400, 10800, 7), Calculation(0x2082, 0x400, 10800, 7), Calculation(0x0081, 0, 10800, 15), Calculation(0x0082, 0, 10800, 15), Calculation(0x2081, 0x400, 10800, 22), Calculation(0x2082, 0x400, 10800, 22), Calculation(0x0081, 0, 10800, 30), Calculation(0x0082, 0, 10800, 30), Calculation(0x2081, 0x400, 10800, 37), Calculation(0x2082, 0x400, 10800, 37), Calculation(0x0081, 0, 10800, 45), Calculation(0x0082, 0, 10800, 45), Calculation(0x2081, 0x400, 10800, 52), Calculation(0x2082, 0x400, 10800, 52), Calculation(0x0081, 0, 10800, 60), Calculation(0x0082, 0, 10800, 60), Calculation(0x2081, 0x400, 10800, 67), Calculation(0x2082, 0x400, 10800, 67), Calculation(0x0081, 0, 10800, 75), Calculation(0x0082, 0, 10800, 75), Calculation(0x2081, 0x400, 10800, 82), Calculation(0x2082, 0x400, 10800, 82), Calculation(0x0081, 0, 10800, 90), Calculation(0x0082, 0, 10800, 90), Calculation(0x2081, 0x400, 10800, 97), Calculation(0x2082, 0x400, 10800, 97), Calculation(0x0081, 0, 10800, 105), Calculation(0x0082, 0, 10800, 105), Calculation(0x2081, 0x400, 10800, 112), Calculation(0x2082, 0x400, 10800, 112), Calculation(0x0081, 0, 10800, 120), Calculation(0x0082, 0, 10800, 120), Calculation(0x2081, 0x400, 10800, 127), Calculation(0x2082, 0x400, 10800, 127), Calculation(0x0081, 0, 10800, 135), Calculation(0x0082, 0, 10800, 135), Calculation(0x2081, 0x400, 10800, 142), Calculation(0x2082, 0x400, 10800, 142), Calculation(0x0081, 0, 10800, 150), Calculation(0x0082, 0, 10800, 150), Calculation(0x2081, 0x400, 10800, 157), Calculation(0x2082, 0x400, 10800, 157), Calculation(0x0081, 0, 10800, 165), Calculation(0x0082, 0, 10800, 165), Calculation(0x2081, 0x400, 10800, 172), Calculation(0x2082, 0x400, 10800, 172), Calculation(0x0081, 0, 10800, 180), Calculation(0x0082, 0, 10800, 180), Calculation(0x2081, 0x400, 10800, 187), Calculation(0x2082, 0x400, 10800, 187), Calculation(0x0081, 0, 10800, 195), Calculation(0x0082, 0, 10800, 195), Calculation(0x2081, 0x400, 10800, 202), Calculation(0x2082, 0x400, 10800, 202), Calculation(0x0081, 0, 10800, 210), Calculation(0x0082, 0, 10800, 210), Calculation(0x2081, 0x400, 10800, 217), Calculation(0x2082, 0x400, 10800, 217), Calculation(0x0081, 0, 10800, 225), Calculation(0x0082, 0, 10800, 225), Calculation(0x2081, 0x400, 10800, 232), Calculation(0x2082, 0x400, 10800, 232), Calculation(0x0081, 0, 10800, 240), Calculation(0x0082, 0, 10800, 240), Calculation(0x2081, 0x400, 10800, 247), Calculation(0x2082, 0x400, 10800, 247), Calculation(0x0081, 0, 10800, 255), Calculation(0x0082, 0, 10800, 255), Calculation(0x2081, 0x400, 10800, 262), Calculation(0x2082, 0x400, 10800, 262), Calculation(0x0081, 0, 10800, 270), Calculation(0x0082, 0, 10800, 270), Calculation(0x2081, 0x400, 10800, 277), Calculation(0x2082, 0x400, 10800, 277), Calculation(0x0081, 0, 10800, 285), Calculation(0x0082, 0, 10800, 285), Calculation(0x2081, 0x400, 10800, 292), Calculation(0x2082, 0x400, 10800, 292), Calculation(0x0081, 0, 10800, 300), Calculation(0x0082, 0, 10800, 300), Calculation(0x2081, 0x400, 10800, 307), Calculation(0x2082, 0x400, 10800, 307), Calculation(0x0081, 0, 10800, 315), Calculation(0x0082, 0, 10800, 315), Calculation(0x2081, 0x400, 10800, 322), Calculation(0x2082, 0x400, 10800, 322), Calculation(0x0081, 0, 10800, 330), Calculation(0x0082, 0, 10800, 330), Calculation(0x2081, 0x400, 10800, 337), Calculation(0x2082, 0x400, 10800, 337), Calculation(0x0081, 0, 10800, 345), Calculation(0x0082, 0, 10800, 345), Calculation(0x2081, 0x400, 10800, 352), Calculation(0x2082, 0x400, 10800, 352)
};

const TextRectangle SEAL_24_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int SEAL_24_DEFAULT_ADJUST[] =
{
  2500
};

const CustomShape CS_SEAL_24(
  SEAL_24_VERTICES, sizeof(SEAL_24_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_24_CALC, sizeof(SEAL_24_CALC) / sizeof(Calculation),
  SEAL_24_DEFAULT_ADJUST, sizeof(SEAL_24_DEFAULT_ADJUST) / sizeof(int),
  SEAL_24_TRS, sizeof(SEAL_24_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex BENT_ARROW_VERTICES[] =
{
  Vertex(0, 21600), Vertex(0, 12160), Vertex(12427, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0), Vertex(21600, 6079), Vertex(0 CALCULATED_VALUE, 12158), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(12427, 2 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 12160), Vertex(4 CALCULATED_VALUE, 21600)
};

const unsigned short BENT_ARROW_SEGMENTS[] =
{
  0x4000, 0x0001, 0xa801, 0x0006, 0xa701, 0x0001, 0x6001, 0x8000
};

const Calculation BENT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 12158, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 6079, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x2001, 0x0403, 2, 1)
};

const TextRectangle BENT_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int BENT_ARROW_DEFAULT_ADJUST[] =
{
  15100, 2900
};

const CustomShape CS_BENT_ARROW(
  BENT_ARROW_VERTICES, sizeof(BENT_ARROW_VERTICES) / sizeof(Vertex),
  BENT_ARROW_SEGMENTS, sizeof(BENT_ARROW_SEGMENTS) / sizeof(unsigned short),
  BENT_ARROW_CALC, sizeof(BENT_ARROW_CALC) / sizeof(Calculation),
  BENT_ARROW_DEFAULT_ADJUST, sizeof(BENT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  BENT_ARROW_TRS, sizeof(BENT_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex BENT_UP_ARROW_VERTICES[] =
{
  Vertex(0, 8 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 0), Vertex(21600, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0, 21600)
};

const unsigned short BENT_UP_ARROW_SEGMENTS[] =
{
  0x4000, 0x0008, 0x6001, 0x8000
};

const Calculation BENT_UP_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 2, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0403, 1, 2), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, 0x0404, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x6000, PROP_ADJUST_VAL_FIRST, 0x0406, 0), Calculation(0x6000, 0x0407, 0x0406, 0), Calculation(0x8000, 21600, 0, 0x406), Calculation(0xa000, 0x409, 0, 0x406)
};

const TextRectangle BENT_UP_ARROW_TRS[] =
{
  TextRectangle(Vertex(2 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE)), TextRectangle(Vertex(7 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE))
};

const int BENT_UP_ARROW_DEFAULT_ADJUST[] =
{
  9340, 18500, 7200
};

const CustomShape CS_BENT_UP_ARROW(
  BENT_UP_ARROW_VERTICES, sizeof(BENT_UP_ARROW_VERTICES) / sizeof(Vertex),
  BENT_UP_ARROW_SEGMENTS, sizeof(BENT_UP_ARROW_SEGMENTS) / sizeof(unsigned short),
  BENT_UP_ARROW_CALC, sizeof(BENT_UP_ARROW_CALC) / sizeof(Calculation),
  BENT_UP_ARROW_DEFAULT_ADJUST, sizeof(BENT_UP_ARROW_DEFAULT_ADJUST) / sizeof(int),
  BENT_UP_ARROW_TRS, sizeof(BENT_UP_ARROW_TRS) / sizeof(TextRectangle),
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

const Vertex LEFT_RIGHT_ARROW_VERTICES[] =
{
  Vertex(0, 10800), Vertex(0 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 0), Vertex(21600, 10800), Vertex(2 CALCULATED_VALUE, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 21600)
};

const unsigned short LEFT_RIGHT_ARROW_SEGMENTS[] =
{
  0x4000, 0x0009, 0x6001, 0x8000
};

const Calculation LEFT_RIGHT_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x6001, PROP_ADJUST_VAL_FIRST, 0x404, 10800), Calculation(0x8000, 21600, 0, 0x405), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x6001, PROP_ADJUST_VAL_FIRST + 1, 0x407, 10800), Calculation(0x8000, 21600, 0, 0x408)
};

const TextRectangle LEFT_RIGHT_ARROW_TRS[] =
{
  TextRectangle(Vertex(5 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 3 CALCULATED_VALUE))
};

const int LEFT_RIGHT_ARROW_DEFAULT_ADJUST[] =
{
  4300, 5400
};

const CustomShape CS_LEFT_RIGHT_ARROW(
  LEFT_RIGHT_ARROW_VERTICES, sizeof(LEFT_RIGHT_ARROW_VERTICES) / sizeof(Vertex),
  LEFT_RIGHT_ARROW_SEGMENTS, sizeof(LEFT_RIGHT_ARROW_SEGMENTS) / sizeof(unsigned short),
  LEFT_RIGHT_ARROW_CALC, sizeof(LEFT_RIGHT_ARROW_CALC) / sizeof(Calculation),
  LEFT_RIGHT_ARROW_DEFAULT_ADJUST, sizeof(LEFT_RIGHT_ARROW_DEFAULT_ADJUST) / sizeof(int),
  LEFT_RIGHT_ARROW_TRS, sizeof(LEFT_RIGHT_ARROW_TRS) / sizeof(TextRectangle),
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
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
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
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE
  0x4000, 0xaa00, 0x0003, 0x8000 // NO FILL
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
  0x4000, 0xab00, 0x0003, 0x6000, 0x8000, // NO STROKE
  0x4000, 0x0001, 0x8000
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

const Vertex ARC_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(10800, 10800), Vertex(0, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 5 CALCULATED_VALUE)
};

const unsigned short ARC_SEGMENTS[] =
{
  0xa504, 0xab00, 0x0001, 0x6001, 0x8000, 0xa504, 0xaa00, 0x8000
};

const Calculation ARC_CALC[] =
{
  Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x402, 10800, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST + 1, 0), Calculation(0x2000, 0x404, 10800, 0), Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST + 1, 0), Calculation(0x2000, 0x406, 10800, 0)
};

const int ARC_DEFAULT_ADJUST[] =
{
  270 << 16, 0
};

const Vertex ARC_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_ARC(
  ARC_VERTICES, sizeof(ARC_VERTICES) / sizeof(Vertex),
  ARC_SEGMENTS, sizeof(ARC_SEGMENTS) / sizeof(unsigned short),
  ARC_CALC, sizeof(ARC_CALC) / sizeof(Calculation),
  ARC_DEFAULT_ADJUST, sizeof(ARC_DEFAULT_ADJUST) / sizeof(int),
  NULL, 0,
  21600, 21600,
  ARC_GLUE_POINTS, sizeof(ARC_GLUE_POINTS) / sizeof(Vertex), 0x3);

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

const Vertex FLOW_CHART_SUMMING_JUNCTION_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360), Vertex(3100, 3100), Vertex(18500, 18500), Vertex(3100, 18500), Vertex(18500, 3100)
};

const unsigned short FLOW_CHART_SUMMING_JUNCTION_SEGMENTS[] =
{
  0xa203, 0x6000, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const TextRectangle FLOW_CHART_SUMMING_JUNCTION_TRS[] =
{
  TextRectangle(Vertex(3100, 3100), Vertex(18500, 18500))
};

const Vertex FLOW_CHART_SUMMING_JUNCTION_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_FLOW_CHART_SUMMING_JUNCTION(
  FLOW_CHART_SUMMING_JUNCTION_VERTICES, sizeof(FLOW_CHART_SUMMING_JUNCTION_VERTICES) / sizeof(Vertex),
  FLOW_CHART_SUMMING_JUNCTION_SEGMENTS, sizeof(FLOW_CHART_SUMMING_JUNCTION_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_SUMMING_JUNCTION_TRS, sizeof(FLOW_CHART_SUMMING_JUNCTION_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_SUMMING_JUNCTION_GLUE_POINTS, sizeof(FLOW_CHART_SUMMING_JUNCTION_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_PREPARATION_VERTICES[] =
{
  Vertex(4350, 0), Vertex(17250, 0), Vertex(21600, 10800), Vertex(17250, 21600), Vertex(4350, 21600), Vertex(0, 10800), Vertex(4350, 0)
};

const TextRectangle FLOW_CHART_PREPARATION_TRS[] =
{
  TextRectangle(Vertex(4350, 0), Vertex(17250, 21600))
};

const Vertex FLOW_CHART_PREPARATION_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_PREPARATION(
  FLOW_CHART_PREPARATION_VERTICES, sizeof(FLOW_CHART_PREPARATION_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_PREPARATION_TRS, sizeof(FLOW_CHART_PREPARATION_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_PREPARATION_GLUE_POINTS, sizeof(FLOW_CHART_PREPARATION_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_PUNCHED_TAPE_VERTICES[] =
{
  Vertex(0, 2230), Vertex(820, 3990), Vertex(3410, 3980), Vertex(5370, 4360), Vertex(7430, 4030), Vertex(10110, 3890), Vertex(10690, 2270), Vertex(11440, 300), Vertex(14200, 160), Vertex(16150, 0), Vertex(18670, 170), Vertex(20690, 390), Vertex(21600, 2230), Vertex(21600, 19420), Vertex(20640, 17510), Vertex(18320, 17490), Vertex(16140, 17240), Vertex(14710, 17370), Vertex(11310, 17510), Vertex(10770, 19430), Vertex(10150, 21150), Vertex(7380, 21290), Vertex(5290, 21600), Vertex(3220, 21250), Vertex(610, 21130), Vertex(0, 19420)
};

const unsigned short FLOW_CHART_PUNCHED_TAPE_SEGMENTS[] =
{
  0x4000, 0x2004, 0x0001, 0x2004, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_PUNCHED_TAPE_TRS[] =
{
  TextRectangle(Vertex(0, 4360), Vertex(21600, 17240))
};

const Vertex FLOW_CHART_PUNCHED_TAPE_GLUE_POINTS[] =
{
  Vertex(10800, 2020), Vertex(0, 10800), Vertex(10800, 19320), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_PUNCHED_TAPE(
  FLOW_CHART_PUNCHED_TAPE_VERTICES, sizeof(FLOW_CHART_PUNCHED_TAPE_VERTICES) / sizeof(Vertex),
  FLOW_CHART_PUNCHED_TAPE_SEGMENTS, sizeof(FLOW_CHART_PUNCHED_TAPE_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_PUNCHED_TAPE_TRS, sizeof(FLOW_CHART_PUNCHED_TAPE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_PUNCHED_TAPE_GLUE_POINTS, sizeof(FLOW_CHART_PUNCHED_TAPE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_PUNCHED_CARD_VERTICES[] =
{
  Vertex(4300, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 4300), Vertex(4300, 0)
};

const TextRectangle FLOW_CHART_PUNCHED_CARD_TRS[] =
{
  TextRectangle(Vertex(0, 4300), Vertex(21600, 21600))
};

const Vertex FLOW_CHART_PUNCHED_CARD_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_PUNCHED_CARD(
  FLOW_CHART_PUNCHED_CARD_VERTICES, sizeof(FLOW_CHART_PUNCHED_CARD_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_PUNCHED_CARD_TRS, sizeof(FLOW_CHART_PUNCHED_CARD_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_PUNCHED_CARD_GLUE_POINTS, sizeof(FLOW_CHART_PUNCHED_CARD_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_CONNECTOR_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360)
};

const unsigned short FLOW_CHART_CONNECTOR_SEGMENTS[] =
{
  0xa203, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_CONNECTOR_TRS[] =
{
  TextRectangle(Vertex(3180, 3180), Vertex(18420, 18420))
};

const Vertex FLOW_CHART_CONNECTOR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_FLOW_CHART_CONNECTOR(
  FLOW_CHART_CONNECTOR_VERTICES, sizeof(FLOW_CHART_CONNECTOR_VERTICES) / sizeof(Vertex),
  FLOW_CHART_CONNECTOR_SEGMENTS, sizeof(FLOW_CHART_CONNECTOR_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_CONNECTOR_TRS, sizeof(FLOW_CHART_CONNECTOR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_CONNECTOR_GLUE_POINTS, sizeof(FLOW_CHART_CONNECTOR_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MANUAL_OPERATION_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(17250, 21600), Vertex(4350, 21600), Vertex(0, 0)
};

const TextRectangle FLOW_CHART_MANUAL_OPERATION_TRS[] =
{
  TextRectangle(Vertex(4350, 0), Vertex(17250, 21600))
};

const Vertex FLOW_CHART_MANUAL_OPERATION_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(2160, 10800), Vertex(10800, 21600), Vertex(19440, 10800)
};

const CustomShape CS_FLOW_CHART_MANUAL_OPERATION(
  FLOW_CHART_MANUAL_OPERATION_VERTICES, sizeof(FLOW_CHART_MANUAL_OPERATION_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MANUAL_OPERATION_TRS, sizeof(FLOW_CHART_MANUAL_OPERATION_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MANUAL_OPERATION_GLUE_POINTS, sizeof(FLOW_CHART_MANUAL_OPERATION_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MANUAL_INPUT_VERTICES[] =
{
  Vertex(0, 4300), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 4300)
};

const TextRectangle FLOW_CHART_MANUAL_INPUT_TRS[] =
{
  TextRectangle(Vertex(0, 4300), Vertex(21600, 21600))
};

const Vertex FLOW_CHART_MANUAL_INPUT_GLUE_POINTS[] =
{
  Vertex(10800, 2150), Vertex(0, 10800), Vertex(10800, 19890), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_MANUAL_INPUT(
  FLOW_CHART_MANUAL_INPUT_VERTICES, sizeof(FLOW_CHART_MANUAL_INPUT_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MANUAL_INPUT_TRS, sizeof(FLOW_CHART_MANUAL_INPUT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MANUAL_INPUT_GLUE_POINTS, sizeof(FLOW_CHART_MANUAL_INPUT_GLUE_POINTS) / sizeof(Vertex));


const Vertex FLOW_CHART_OR_VERTICES[] =
{
  Vertex(10800, 10800), Vertex(10800, 10800), Vertex(0, 360), Vertex(0, 10800), Vertex(21600, 10800), Vertex(10800, 0), Vertex(10800, 21600)
};

const unsigned short FLOW_CHART_OR_SEGMENTS[] =
{
  0xa203, 0x6000, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const TextRectangle FLOW_CHART_OR_TRS[] =
{
  TextRectangle(Vertex(3100, 3100), Vertex(18500, 18500))
};

const Vertex FLOW_CHART_OR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(3163, 3163), Vertex(0, 10800), Vertex(3163, 18437), Vertex(10800, 21600), Vertex(18437, 18437), Vertex(21600, 10800), Vertex(18437, 3163)
};

const CustomShape CS_FLOW_CHART_OR(
  FLOW_CHART_OR_VERTICES, sizeof(FLOW_CHART_OR_VERTICES) / sizeof(Vertex),
  FLOW_CHART_OR_SEGMENTS, sizeof(FLOW_CHART_OR_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_OR_TRS, sizeof(FLOW_CHART_OR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_OR_GLUE_POINTS, sizeof(FLOW_CHART_OR_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_COLLATE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(21600, 0), Vertex(0, 0)
};

const TextRectangle FLOW_CHART_COLLATE_TRS[] =
{
  TextRectangle(Vertex(5400, 5400), Vertex(16200, 16200))
};

const Vertex FLOW_CHART_COLLATE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(10800, 10800), Vertex(10800, 21600)
};

const CustomShape CS_FLOW_CHART_COLLATE(
  FLOW_CHART_COLLATE_VERTICES, sizeof(FLOW_CHART_COLLATE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_COLLATE_TRS, sizeof(FLOW_CHART_COLLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_COLLATE_GLUE_POINTS, sizeof(FLOW_CHART_COLLATE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_SORT_VERTICES[] =
{
  Vertex(0, 10800), Vertex(10800, 0), Vertex(21600, 10800), Vertex(10800, 21600), Vertex(0, 10800), Vertex(21600, 10800)
};

const unsigned short FLOW_CHART_SORT_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6000, 0x8000, 0x4000, 0x0001, 0x8000
};

const TextRectangle FLOW_CHART_SORT_TRS[] =
{
  TextRectangle(Vertex(5400, 5400), Vertex(16200, 16200))
};

const CustomShape CS_FLOW_CHART_SORT(
  FLOW_CHART_SORT_VERTICES, sizeof(FLOW_CHART_SORT_VERTICES) / sizeof(Vertex),
  FLOW_CHART_SORT_SEGMENTS, sizeof(FLOW_CHART_SORT_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_SORT_TRS, sizeof(FLOW_CHART_SORT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex FLOW_CHART_EXTRACT_VERTICES[] =
{
  Vertex(10800, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(10800, 0)
};

const TextRectangle FLOW_CHART_EXTRACT_TRS[] =
{
  TextRectangle(Vertex(5400, 10800), Vertex(16200, 21600))
};

const Vertex FLOW_CHART_EXTRACT_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(5400, 10800), Vertex(10800, 21600), Vertex(16200, 10800)
};

const CustomShape CS_FLOW_CHART_EXTRACT(
  FLOW_CHART_EXTRACT_VERTICES, sizeof(FLOW_CHART_EXTRACT_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_EXTRACT_TRS, sizeof(FLOW_CHART_EXTRACT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_EXTRACT_GLUE_POINTS, sizeof(FLOW_CHART_EXTRACT_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MERGE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(10800, 21600), Vertex(0, 0)
};

const TextRectangle FLOW_CHART_MERGE_TRS[] =
{
  TextRectangle(Vertex(5400, 0), Vertex(16200, 10800))
};

const Vertex FLOW_CHART_MERGE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(5400, 10800), Vertex(10800, 21600), Vertex(16200, 10800)
};

const CustomShape CS_FLOW_CHART_MERGE(
  FLOW_CHART_MERGE_VERTICES, sizeof(FLOW_CHART_MERGE_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MERGE_TRS, sizeof(FLOW_CHART_MERGE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MERGE_GLUE_POINTS, sizeof(FLOW_CHART_MERGE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_ONLINE_STORAGE_VERTICES[] =
{
  Vertex(3600, 21600), Vertex(0, 10800), Vertex(3600, 0), Vertex(21600, 0), Vertex(18000, 10800), Vertex(21600, 21600)
};

const unsigned short FLOW_CHART_ONLINE_STORAGE_SEGMENTS[] =
{
  0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_ONLINE_STORAGE_TRS[] =
{
  TextRectangle(Vertex(3600, 0), Vertex(18000, 21600))
};

const Vertex FLOW_CHART_ONLINE_STORAGE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(18000, 10800)
};

const CustomShape CS_FLOW_CHART_ONLINE_STORAGE(
  FLOW_CHART_ONLINE_STORAGE_VERTICES, sizeof(FLOW_CHART_ONLINE_STORAGE_VERTICES) / sizeof(Vertex),
  FLOW_CHART_ONLINE_STORAGE_SEGMENTS, sizeof(FLOW_CHART_ONLINE_STORAGE_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_ONLINE_STORAGE_TRS, sizeof(FLOW_CHART_ONLINE_STORAGE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_ONLINE_STORAGE_GLUE_POINTS, sizeof(FLOW_CHART_ONLINE_STORAGE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MAGNETIC_TAPE_VERTICES[] =
{
  Vertex(20980, 18150), Vertex(20980, 21600), Vertex(10670, 21600), Vertex(4770, 21540), Vertex(0, 16720), Vertex(0, 10800), Vertex(0, 4840), Vertex(4840, 0), Vertex(10800, 0), Vertex(16740, 0), Vertex(21600, 4840), Vertex(21600, 10800), Vertex(21600, 13520), Vertex(20550, 16160), Vertex(18670, 18170)
};

const unsigned short FLOW_CHART_MAGNETIC_TAPE_SEGMENTS[] =
{
  0x4000, 0x0002, 0x2004, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_MAGNETIC_TAPE_TRS[] =
{
  TextRectangle(Vertex(3100, 3100), Vertex(18500, 18500))
};

const Vertex FLOW_CHART_MAGNETIC_TAPE_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_MAGNETIC_TAPE(
  FLOW_CHART_MAGNETIC_TAPE_VERTICES, sizeof(FLOW_CHART_MAGNETIC_TAPE_VERTICES) / sizeof(Vertex),
  FLOW_CHART_MAGNETIC_TAPE_SEGMENTS, sizeof(FLOW_CHART_MAGNETIC_TAPE_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MAGNETIC_TAPE_TRS, sizeof(FLOW_CHART_MAGNETIC_TAPE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MAGNETIC_TAPE_GLUE_POINTS, sizeof(FLOW_CHART_MAGNETIC_TAPE_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MAGNETIC_DISK_VERTICES[] =
{
  Vertex(0, 3400), Vertex(10800, 0), Vertex(21600, 3400), Vertex(21600, 18200), Vertex(10800, 21600), Vertex(0, 18200), Vertex(0, 3400), Vertex(10800, 6800), Vertex(21600, 3400)
};

const unsigned short FLOW_CHART_MAGNETIC_DISK_SEGMENTS[] =
{
  0x4000, 0xa802, 0x0001, 0xa802, 0x6000, 0x8000, 0x4000, 0xa802, 0x8000
};

const TextRectangle FLOW_CHART_MAGNETIC_DISK_TRS[] =
{
  TextRectangle(Vertex(0, 6800), Vertex(21600, 18200))
};

const Vertex FLOW_CHART_MAGNETIC_DISK_GLUE_POINTS[] =
{
  Vertex(10800, 6800), Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_MAGNETIC_DISK(
  FLOW_CHART_MAGNETIC_DISK_VERTICES, sizeof(FLOW_CHART_MAGNETIC_DISK_VERTICES) / sizeof(Vertex),
  FLOW_CHART_MAGNETIC_DISK_SEGMENTS, sizeof(FLOW_CHART_MAGNETIC_DISK_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MAGNETIC_DISK_TRS, sizeof(FLOW_CHART_MAGNETIC_DISK_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MAGNETIC_DISK_GLUE_POINTS, sizeof(FLOW_CHART_MAGNETIC_DISK_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_MAGNETIC_DRUM_VERTICES[] =
{
  Vertex(18200, 0), Vertex(21600, 10800), Vertex(18200, 21600), Vertex(3400, 21600), Vertex(0, 10800), Vertex(3400, 0), Vertex(18200, 0), Vertex(14800, 10800), Vertex(18200, 21600)
};

const unsigned short FLOW_CHART_MAGNETIC_DRUM_SEGMENTS[] =
{
  0x4000, 0xa702, 0x0001, 0xa702, 0x6000, 0x8000, 0x4000, 0xa702, 0x8000
};

const TextRectangle FLOW_CHART_MAGNETIC_DRUM_TRS[] =
{
  TextRectangle(Vertex(3400, 0), Vertex(14800, 21600))
};

const Vertex FLOW_CHART_MAGNETIC_DRUM_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(14800, 10800), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_MAGNETIC_DRUM(
  FLOW_CHART_MAGNETIC_DRUM_VERTICES, sizeof(FLOW_CHART_MAGNETIC_DRUM_VERTICES) / sizeof(Vertex),
  FLOW_CHART_MAGNETIC_DRUM_SEGMENTS, sizeof(FLOW_CHART_MAGNETIC_DRUM_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_MAGNETIC_DRUM_TRS, sizeof(FLOW_CHART_MAGNETIC_DRUM_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_MAGNETIC_DRUM_GLUE_POINTS, sizeof(FLOW_CHART_MAGNETIC_DRUM_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_DIPSLAY_VERTICES[] =
{
  Vertex(3600, 0), Vertex(17800, 0), Vertex(21600, 10800), Vertex(17800, 21600), Vertex(3600, 21600), Vertex(0, 10800)
};

const unsigned short FLOW_CHART_DIPSLAY_SEGMENTS[] =
{
  0x4000, 0x0001, 0xa702, 0x0002, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_DIPSLAY_TRS[] =
{
  TextRectangle(Vertex(3600, 0), Vertex(17800, 21600))
};

const Vertex FLOW_CHART_DIPSLAY_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_DIPSLAY(
  FLOW_CHART_DIPSLAY_VERTICES, sizeof(FLOW_CHART_DIPSLAY_VERTICES) / sizeof(Vertex),
  FLOW_CHART_DIPSLAY_SEGMENTS, sizeof(FLOW_CHART_DIPSLAY_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_DIPSLAY_TRS, sizeof(FLOW_CHART_DIPSLAY_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_DIPSLAY_GLUE_POINTS, sizeof(FLOW_CHART_DIPSLAY_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_DELAY_VERTICES[] =
{
  Vertex(10800, 0), Vertex(21600, 10800), Vertex(10800, 21600), Vertex(0, 21600), Vertex(0, 0)
};

const unsigned short FLOW_CHART_DELAY_SEGMENTS[] =
{
  0x4000, 0xa702, 0x0002, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_DELAY_TRS[] =
{
  TextRectangle(Vertex(0, 3100), Vertex(18500, 18500))
};

const Vertex FLOW_CHART_DELAY_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_DELAY(
  FLOW_CHART_DELAY_VERTICES, sizeof(FLOW_CHART_DELAY_VERTICES) / sizeof(Vertex),
  FLOW_CHART_DELAY_SEGMENTS, sizeof(FLOW_CHART_DELAY_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_DELAY_TRS, sizeof(FLOW_CHART_DELAY_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_DELAY_GLUE_POINTS, sizeof(FLOW_CHART_DELAY_GLUE_POINTS) / sizeof(Vertex));

const Vertex TEXT_PLAIN_TEXT_VERTICES[] =
{
  Vertex(3 CALCULATED_VALUE, 0), Vertex(5 CALCULATED_VALUE, 0), Vertex(6 CALCULATED_VALUE, 21600), Vertex(7 CALCULATED_VALUE, 21600)
};

const unsigned short TEXT_PLAIN_TEXT_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_PLAIN_TEXT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 10800), Calculation(0x2001, 0x400, 2, 1), Calculation(0x2003, 0x401, 0, 0), Calculation(0xa006, 0x401, 0, 0x402), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x6006, 0x401, 0x404, 21600), Calculation(0x6006, 0x401, 0x402, 0), Calculation(0xa006, 0x401, 21600, 0x404)
};

const TextRectangle TEXT_PLAIN_TEXT_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_PLAIN_TEXT_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_TEXT_PLAIN_TEXT(
  TEXT_PLAIN_TEXT_VERTICES, sizeof(TEXT_PLAIN_TEXT_VERTICES) / sizeof(Vertex),
  TEXT_PLAIN_TEXT_SEGMENTS, sizeof(TEXT_PLAIN_TEXT_SEGMENTS) / sizeof(unsigned short),
  TEXT_PLAIN_TEXT_CALC, sizeof(TEXT_PLAIN_TEXT_CALC) / sizeof(Calculation),
  TEXT_PLAIN_TEXT_DEFAULT_ADJUST, sizeof(TEXT_PLAIN_TEXT_DEFAULT_ADJUST) / sizeof(int),
  TEXT_PLAIN_TEXT_TRS, sizeof(TEXT_PLAIN_TEXT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_STOP_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(7200, 0), Vertex(14400, 0), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 1 CALCULATED_VALUE), Vertex(7200, 21600), Vertex(14400, 21600), Vertex(21600, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_STOP_SEGMENTS[] =
{
  0x4000, 0x0003, 0x8000, 0x4000, 0x0003, 0x8000
};

const Calculation TEXT_STOP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_STOP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_STOP_DEFAULT_ADJUST[] =
{
  2700
};

const CustomShape CS_TEXT_STOP(
  TEXT_STOP_VERTICES, sizeof(TEXT_STOP_VERTICES) / sizeof(Vertex),
  TEXT_STOP_SEGMENTS, sizeof(TEXT_STOP_SEGMENTS) / sizeof(unsigned short),
  TEXT_STOP_CALC, sizeof(TEXT_STOP_CALC) / sizeof(Calculation),
  TEXT_STOP_DEFAULT_ADJUST, sizeof(TEXT_STOP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_STOP_TRS, sizeof(TEXT_STOP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_TRIANGLE_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(10800, 0), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_TRIANGLE_SEGMENTS[] =
{
  0x4000, 0x0002, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_TRIANGLE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0)
};

const TextRectangle TEXT_TRIANGLE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_TRIANGLE_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_TEXT_TRIANGLE(
  TEXT_TRIANGLE_VERTICES, sizeof(TEXT_TRIANGLE_VERTICES) / sizeof(Vertex),
  TEXT_TRIANGLE_SEGMENTS, sizeof(TEXT_TRIANGLE_SEGMENTS) / sizeof(unsigned short),
  TEXT_TRIANGLE_CALC, sizeof(TEXT_TRIANGLE_CALC) / sizeof(Calculation),
  TEXT_TRIANGLE_DEFAULT_ADJUST, sizeof(TEXT_TRIANGLE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_TRIANGLE_TRS, sizeof(TEXT_TRIANGLE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_TRIANGLE_INVERTED_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0, 0 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(21600, 0 CALCULATED_VALUE)
};

const unsigned short TEXT_TRIANGLE_INVERTED_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0002, 0x8000
};

const Calculation TEXT_TRIANGLE_INVERTED_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0)
};

const TextRectangle TEXT_TRIANGLE_INVERTED_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_TRIANGLE_INVERTED_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_TEXT_TRIANGLE_INVERTED(
  TEXT_TRIANGLE_INVERTED_VERTICES, sizeof(TEXT_TRIANGLE_INVERTED_VERTICES) / sizeof(Vertex),
  TEXT_TRIANGLE_INVERTED_SEGMENTS, sizeof(TEXT_TRIANGLE_INVERTED_SEGMENTS) / sizeof(unsigned short),
  TEXT_TRIANGLE_INVERTED_CALC, sizeof(TEXT_TRIANGLE_INVERTED_CALC) / sizeof(Calculation),
  TEXT_TRIANGLE_INVERTED_DEFAULT_ADJUST, sizeof(TEXT_TRIANGLE_INVERTED_DEFAULT_ADJUST) / sizeof(int),
  TEXT_TRIANGLE_INVERTED_TRS, sizeof(TEXT_TRIANGLE_INVERTED_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex FLOW_CHART_DISPLAY_VERTICES[] =
{
  Vertex(3600, 0), Vertex(17800, 0), Vertex(21600, 10800), Vertex(17800, 21600), Vertex(3600, 21600), Vertex(0, 10800)
};

const unsigned short FLOW_CHART_DISPLAY_SEGMENTS[] =
{
  0x4000, 0x0001, 0xa702, 0x0002, 0x6000, 0x8000
};

const TextRectangle FLOW_CHART_DISPLAY_TRS[] =
{
  TextRectangle(Vertex(3600, 0), Vertex(17800, 21600))
};

const Vertex FLOW_CHART_DISPLAY_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_DISPLAY(
  FLOW_CHART_DISPLAY_VERTICES, sizeof(FLOW_CHART_DISPLAY_VERTICES) / sizeof(Vertex),
  FLOW_CHART_DISPLAY_SEGMENTS, sizeof(FLOW_CHART_DISPLAY_SEGMENTS) / sizeof(unsigned short),
  NULL, 0,
  NULL, 0,
  FLOW_CHART_DISPLAY_TRS, sizeof(FLOW_CHART_DISPLAY_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_DISPLAY_GLUE_POINTS, sizeof(FLOW_CHART_DISPLAY_GLUE_POINTS) / sizeof(Vertex));

const Vertex TEXT_CHEVRON_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(10800, 0), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 21600), Vertex(10800, 1 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_CHEVRON_SEGMENTS[] =
{
  0x4000, 0x0002, 0x8000, 0x4000, 0x0002, 0x8000
};

const Calculation TEXT_CHEVRON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_CHEVRON_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CHEVRON_DEFAULT_ADJUST[] =
{
  5400
};

const CustomShape CS_TEXT_CHEVRON(
  TEXT_CHEVRON_VERTICES, sizeof(TEXT_CHEVRON_VERTICES) / sizeof(Vertex),
  TEXT_CHEVRON_SEGMENTS, sizeof(TEXT_CHEVRON_SEGMENTS) / sizeof(unsigned short),
  TEXT_CHEVRON_CALC, sizeof(TEXT_CHEVRON_CALC) / sizeof(Calculation),
  TEXT_CHEVRON_DEFAULT_ADJUST, sizeof(TEXT_CHEVRON_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CHEVRON_TRS, sizeof(TEXT_CHEVRON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CHEVRON_INVERTED_VERTICES[] =
{
  Vertex(0, 0), Vertex(10800, 1 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 0 CALCULATED_VALUE), Vertex(10800, 21600), Vertex(21600, 0 CALCULATED_VALUE)
};

const unsigned short TEXT_CHEVRON_INVERTED_SEGMENTS[] =
{
  0x4000, 0x0002, 0x8000, 0x4000, 0x0002, 0x8000
};

const Calculation TEXT_CHEVRON_INVERTED_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_CHEVRON_INVERTED_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CHEVRON_INVERTED_DEFAULT_ADJUST[] =
{
  16200
};

const Vertex TEXT_CHEVRON_INVERTED_GLUE_POINTS[] =
{
  Vertex(0, 0), Vertex(21600, 21600)
};

const CustomShape CS_TEXT_CHEVRON_INVERTED(
  TEXT_CHEVRON_INVERTED_VERTICES, sizeof(TEXT_CHEVRON_INVERTED_VERTICES) / sizeof(Vertex),
  TEXT_CHEVRON_INVERTED_SEGMENTS, sizeof(TEXT_CHEVRON_INVERTED_SEGMENTS) / sizeof(unsigned short),
  TEXT_CHEVRON_INVERTED_CALC, sizeof(TEXT_CHEVRON_INVERTED_CALC) / sizeof(Calculation),
  TEXT_CHEVRON_INVERTED_DEFAULT_ADJUST, sizeof(TEXT_CHEVRON_INVERTED_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CHEVRON_INVERTED_TRS, sizeof(TEXT_CHEVRON_INVERTED_TRS) / sizeof(TextRectangle),
  21600, 21600,
  TEXT_CHEVRON_INVERTED_GLUE_POINTS, sizeof(TEXT_CHEVRON_INVERTED_GLUE_POINTS) / sizeof(Vertex));

const Vertex TEXT_RING_OUTSIDE_VERTICES[] =
{
  Vertex(10800, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(180, 359), Vertex(10800, 1 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(180, 359)
};

const unsigned short TEXT_RING_OUTSIDE_SEGMENTS[] =
{
  0xA203, 0x8000, 0xA203, 0x8000
};

const Calculation TEXT_RING_OUTSIDE_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 2), Calculation(0x8000, 21600, 0, 0x400)
};

const TextRectangle TEXT_RING_OUTSIDE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_RING_OUTSIDE_DEFAULT_ADJUST[] =
{
  16200
};

const CustomShape CS_TEXT_RING_OUTSIDE(
  TEXT_RING_OUTSIDE_VERTICES, sizeof(TEXT_RING_OUTSIDE_VERTICES) / sizeof(Vertex),
  TEXT_RING_OUTSIDE_SEGMENTS, sizeof(TEXT_RING_OUTSIDE_SEGMENTS) / sizeof(unsigned short),
  TEXT_RING_OUTSIDE_CALC, sizeof(TEXT_RING_OUTSIDE_CALC) / sizeof(Calculation),
  TEXT_RING_OUTSIDE_DEFAULT_ADJUST, sizeof(TEXT_RING_OUTSIDE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_RING_OUTSIDE_TRS, sizeof(TEXT_RING_OUTSIDE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_ARCH_UP_CURVE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short TEXT_ARCH_UP_CURVE_SEGMENTS[] =
{
  0xA504, 0x8000
};

const Calculation TEXT_ARCH_UP_CURVE_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402)
};

const TextRectangle TEXT_ARCH_UP_CURVE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_ARCH_UP_CURVE_DEFAULT_ADJUST[] =
{
  180
};

const CustomShape CS_TEXT_ARCH_UP_CURVE(
  TEXT_ARCH_UP_CURVE_VERTICES, sizeof(TEXT_ARCH_UP_CURVE_VERTICES) / sizeof(Vertex),
  TEXT_ARCH_UP_CURVE_SEGMENTS, sizeof(TEXT_ARCH_UP_CURVE_SEGMENTS) / sizeof(unsigned short),
  TEXT_ARCH_UP_CURVE_CALC, sizeof(TEXT_ARCH_UP_CURVE_CALC) / sizeof(Calculation),
  TEXT_ARCH_UP_CURVE_DEFAULT_ADJUST, sizeof(TEXT_ARCH_UP_CURVE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_ARCH_UP_CURVE_TRS, sizeof(TEXT_ARCH_UP_CURVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_ARCH_UP_POUR_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 9 CALCULATED_VALUE)
};

const unsigned short TEXT_ARCH_UP_POUR_SEGMENTS[] =
{
  0xA504, 0x8000, 0xA504, 0x8000
};

const Calculation TEXT_ARCH_UP_POUR_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x600a, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6009, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x406, 10800, 0), Calculation(0x2000, 0x407, 10800, 0), Calculation(0x8000, 21600, 0, 0x408), Calculation(0x8000, 21600, 0, 0x405)
};

const TextRectangle TEXT_ARCH_UP_POUR_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_ARCH_UP_POUR_DEFAULT_ADJUST[] =
{
  180, 5400
};

const CustomShape CS_TEXT_ARCH_UP_POUR(
  TEXT_ARCH_UP_POUR_VERTICES, sizeof(TEXT_ARCH_UP_POUR_VERTICES) / sizeof(Vertex),
  TEXT_ARCH_UP_POUR_SEGMENTS, sizeof(TEXT_ARCH_UP_POUR_SEGMENTS) / sizeof(unsigned short),
  TEXT_ARCH_UP_POUR_CALC, sizeof(TEXT_ARCH_UP_POUR_CALC) / sizeof(Calculation),
  TEXT_ARCH_UP_POUR_DEFAULT_ADJUST, sizeof(TEXT_ARCH_UP_POUR_DEFAULT_ADJUST) / sizeof(int),
  TEXT_ARCH_UP_POUR_TRS, sizeof(TEXT_ARCH_UP_POUR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_ARCH_DOWN_CURVE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short TEXT_ARCH_DOWN_CURVE_SEGMENTS[] =
{
  0xA304, 0x8000
};

const Calculation TEXT_ARCH_DOWN_CURVE_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402)
};

const TextRectangle TEXT_ARCH_DOWN_CURVE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_ARCH_DOWN_CURVE_DEFAULT_ADJUST[] =
{
  0
};

const CustomShape CS_TEXT_ARCH_DOWN_CURVE(
  TEXT_ARCH_DOWN_CURVE_VERTICES, sizeof(TEXT_ARCH_DOWN_CURVE_VERTICES) / sizeof(Vertex),
  TEXT_ARCH_DOWN_CURVE_SEGMENTS, sizeof(TEXT_ARCH_DOWN_CURVE_SEGMENTS) / sizeof(unsigned short),
  TEXT_ARCH_DOWN_CURVE_CALC, sizeof(TEXT_ARCH_DOWN_CURVE_CALC) / sizeof(Calculation),
  TEXT_ARCH_DOWN_CURVE_DEFAULT_ADJUST, sizeof(TEXT_ARCH_DOWN_CURVE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_ARCH_DOWN_CURVE_TRS, sizeof(TEXT_ARCH_DOWN_CURVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CIRCLE_CURVE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 4 CALCULATED_VALUE)
};

const unsigned short TEXT_CIRCLE_CURVE_SEGMENTS[] =
{
  0xA504, 0x8000
};

const Calculation TEXT_CIRCLE_CURVE_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x403)
};

const TextRectangle TEXT_CIRCLE_CURVE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CIRCLE_CURVE_DEFAULT_ADJUST[] =
{
  -179
};

const CustomShape CS_TEXT_CIRCLE_CURVE(
  TEXT_CIRCLE_CURVE_VERTICES, sizeof(TEXT_CIRCLE_CURVE_VERTICES) / sizeof(Vertex),
  TEXT_CIRCLE_CURVE_SEGMENTS, sizeof(TEXT_CIRCLE_CURVE_SEGMENTS) / sizeof(unsigned short),
  TEXT_CIRCLE_CURVE_CALC, sizeof(TEXT_CIRCLE_CURVE_CALC) / sizeof(Calculation),
  TEXT_CIRCLE_CURVE_DEFAULT_ADJUST, sizeof(TEXT_CIRCLE_CURVE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CIRCLE_CURVE_TRS, sizeof(TEXT_CIRCLE_CURVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_BUTTON_CURVE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 10800), Vertex(21600, 10800), Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 5 CALCULATED_VALUE)
};

const unsigned short TEXT_BUTTON_CURVE_SEGMENTS[] =
{
  0xA504, 0x8000, 0x4000, 0x0001, 0x8000, 0xA304, 0x8000
};

const Calculation TEXT_BUTTON_CURVE_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x8000, 21600, 0, 0x403)
};

const TextRectangle TEXT_BUTTON_CURVE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_BUTTON_CURVE_DEFAULT_ADJUST[] =
{
  180
};

const CustomShape CS_TEXT_BUTTON_CURVE(
  TEXT_BUTTON_CURVE_VERTICES, sizeof(TEXT_BUTTON_CURVE_VERTICES) / sizeof(Vertex),
  TEXT_BUTTON_CURVE_SEGMENTS, sizeof(TEXT_BUTTON_CURVE_SEGMENTS) / sizeof(unsigned short),
  TEXT_BUTTON_CURVE_CALC, sizeof(TEXT_BUTTON_CURVE_CALC) / sizeof(Calculation),
  TEXT_BUTTON_CURVE_DEFAULT_ADJUST, sizeof(TEXT_BUTTON_CURVE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_BUTTON_CURVE_TRS, sizeof(TEXT_BUTTON_CURVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_ARCH_DOWN_POUR_VERTICES[] =
{
  Vertex(5 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0, 0), Vertex(21600, 21600), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE)
};

const unsigned short TEXT_ARCH_DOWN_POUR_SEGMENTS[] =
{
  0xA304, 0x8000, 0xA304, 0x8000
};

const Calculation TEXT_ARCH_DOWN_POUR_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x600a, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6009, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x406, 10800, 0), Calculation(0x2000, 0x407, 10800, 0), Calculation(0x8000, 21600, 0, 0x408), Calculation(0x8000, 21600, 0, 0x405)
};

const TextRectangle TEXT_ARCH_DOWN_POUR_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_ARCH_DOWN_POUR_DEFAULT_ADJUST[] =
{
  0, 5400
};

const CustomShape CS_TEXT_ARCH_DOWN_POUR(
  TEXT_ARCH_DOWN_POUR_VERTICES, sizeof(TEXT_ARCH_DOWN_POUR_VERTICES) / sizeof(Vertex),
  TEXT_ARCH_DOWN_POUR_SEGMENTS, sizeof(TEXT_ARCH_DOWN_POUR_SEGMENTS) / sizeof(unsigned short),
  TEXT_ARCH_DOWN_POUR_CALC, sizeof(TEXT_ARCH_DOWN_POUR_CALC) / sizeof(Calculation),
  TEXT_ARCH_DOWN_POUR_DEFAULT_ADJUST, sizeof(TEXT_ARCH_DOWN_POUR_DEFAULT_ADJUST) / sizeof(int),
  TEXT_ARCH_DOWN_POUR_TRS, sizeof(TEXT_ARCH_DOWN_POUR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CIRCLE_POUR_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 0xa CALCULATED_VALUE)
};

const unsigned short TEXT_CIRCLE_POUR_SEGMENTS[] =
{
  0xA504, 0x8000, 0xA504, 0x8000
};

const Calculation TEXT_CIRCLE_POUR_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x403), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x600a, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6009, 0x405, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x406, 10800, 0), Calculation(0x2000, 0x407, 10800, 0), Calculation(0x8000, 21600, 0, 0x409), Calculation(0x8000, 21600, 0, 0x405), Calculation(0x000, 21600, 0, 0)
};

const TextRectangle TEXT_CIRCLE_POUR_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CIRCLE_POUR_DEFAULT_ADJUST[] =
{
  -179, 5400
};

const CustomShape CS_TEXT_CIRCLE_POUR(
  TEXT_CIRCLE_POUR_VERTICES, sizeof(TEXT_CIRCLE_POUR_VERTICES) / sizeof(Vertex),
  TEXT_CIRCLE_POUR_SEGMENTS, sizeof(TEXT_CIRCLE_POUR_SEGMENTS) / sizeof(unsigned short),
  TEXT_CIRCLE_POUR_CALC, sizeof(TEXT_CIRCLE_POUR_CALC) / sizeof(Calculation),
  TEXT_CIRCLE_POUR_DEFAULT_ADJUST, sizeof(TEXT_CIRCLE_POUR_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CIRCLE_POUR_TRS, sizeof(TEXT_CIRCLE_POUR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_BUTTON_POUR_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 11 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 16 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 15 CALCULATED_VALUE), Vertex(6 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 13 CALCULATED_VALUE), Vertex(0, 0), Vertex(21600, 21600), Vertex(2 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 5 CALCULATED_VALUE)
};

const unsigned short TEXT_BUTTON_POUR_SEGMENTS[] =
{
  0xA504, 0x8000, 0xA504, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000, 0xA304, 0x8000, 0xA304, 0x8000
};

const Calculation TEXT_BUTTON_POUR_CALC[] =
{
  Calculation(0x400a, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x4009, 10800, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x400, 10800, 0), Calculation(0x2000, 0x401, 10800, 0), Calculation(0x8000, 21600, 0, 0x402), Calculation(0x8000, 21600, 0, 0x403), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x406), Calculation(0x600a, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6009, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2000, 0x408, 10800, 0), Calculation(0x2000, 0x409, 10800, 0), Calculation(0x8000, 21600, 0, 0x40a), Calculation(0x8000, 21600, 0, 0x40b), Calculation(0x2001, 0x406, 1, 2), Calculation(0x4000, 10800, 0x40e, 0), Calculation(0x8000, 10800, 0, 0x40e), Calculation(0x6001, 0x40e, 0x40e, 1), Calculation(0x6001, PROP_ADJUST_VAL_FIRST + 1, PROP_ADJUST_VAL_FIRST + 1, 1), Calculation(0xA000, 0x412, 0, 0x411), Calculation(0x200d, 0x413, 0, 0), Calculation(0x4000, 10800, 0x414, 0), Calculation(0x8000, 10800, 0, 0x414)
};

const TextRectangle TEXT_BUTTON_POUR_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_BUTTON_POUR_DEFAULT_ADJUST[] =
{
  180, 5400
};

const CustomShape CS_TEXT_BUTTON_POUR(
  TEXT_BUTTON_POUR_VERTICES, sizeof(TEXT_BUTTON_POUR_VERTICES) / sizeof(Vertex),
  TEXT_BUTTON_POUR_SEGMENTS, sizeof(TEXT_BUTTON_POUR_SEGMENTS) / sizeof(unsigned short),
  TEXT_BUTTON_POUR_CALC, sizeof(TEXT_BUTTON_POUR_CALC) / sizeof(Calculation),
  TEXT_BUTTON_POUR_DEFAULT_ADJUST, sizeof(TEXT_BUTTON_POUR_DEFAULT_ADJUST) / sizeof(int),
  TEXT_BUTTON_POUR_TRS, sizeof(TEXT_BUTTON_POUR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CURVE_UP_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(4900, 1 CALCULATED_VALUE /*12170->0 14250 ->0*/), Vertex(11640, 2 CALCULATED_VALUE /*12170->0 12800 ->0*/), Vertex(21600, 0), Vertex(0, 4 CALCULATED_VALUE /*12170->0 17220 ->21600*/), Vertex(3700, 21600), Vertex(8500, 21600), Vertex(10100, 21600), Vertex(14110, 21600), Vertex(15910, 21600), Vertex(21600, 4 CALCULATED_VALUE /*12170->0 17220 ->21600*/)
};

const unsigned short TEXT_CURVE_UP_SEGMENTS[] =
{
  0x4000, 0x2001, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_CURVE_UP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x4001, 14250, 0x400, 12170), Calculation(0x4001, 12800, 0x400, 12170), Calculation(0x4001, 6380, 0x400, 12170), Calculation(0x8000, 21600, 0, 0x403)
};

const TextRectangle TEXT_CURVE_UP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CURVE_UP_DEFAULT_ADJUST[] =
{
  9900
};

const CustomShape CS_TEXT_CURVE_UP(
  TEXT_CURVE_UP_VERTICES, sizeof(TEXT_CURVE_UP_VERTICES) / sizeof(Vertex),
  TEXT_CURVE_UP_SEGMENTS, sizeof(TEXT_CURVE_UP_SEGMENTS) / sizeof(unsigned short),
  TEXT_CURVE_UP_CALC, sizeof(TEXT_CURVE_UP_CALC) / sizeof(Calculation),
  TEXT_CURVE_UP_DEFAULT_ADJUST, sizeof(TEXT_CURVE_UP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CURVE_UP_TRS, sizeof(TEXT_CURVE_UP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CURVE_DOWN_VERTICES[] =
{
  Vertex(0, 0), Vertex(9960, 2 CALCULATED_VALUE), Vertex(16700, 1 CALCULATED_VALUE), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 4 CALCULATED_VALUE), Vertex(5690, 21600), Vertex(7490, 21600), Vertex(11500, 21600), Vertex(13100, 21600), Vertex(17900, 21600), Vertex(21600, 4 CALCULATED_VALUE)
};

const unsigned short TEXT_CURVE_DOWN_SEGMENTS[] =
{
  0x4000, 0x2001, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_CURVE_DOWN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x4001, 14250, 0x400, 12170), Calculation(0x4001, 12800, 0x400, 12170), Calculation(0x4001, 6380, 0x400, 12170), Calculation(0x8000, 21600, 0, 0x403)
};

const TextRectangle TEXT_CURVE_DOWN_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CURVE_DOWN_DEFAULT_ADJUST[] =
{
  9900
};

const CustomShape CS_TEXT_CURVE_DOWN(
  TEXT_CURVE_DOWN_VERTICES, sizeof(TEXT_CURVE_DOWN_VERTICES) / sizeof(Vertex),
  TEXT_CURVE_DOWN_SEGMENTS, sizeof(TEXT_CURVE_DOWN_SEGMENTS) / sizeof(unsigned short),
  TEXT_CURVE_DOWN_CALC, sizeof(TEXT_CURVE_DOWN_CALC) / sizeof(Calculation),
  TEXT_CURVE_DOWN_DEFAULT_ADJUST, sizeof(TEXT_CURVE_DOWN_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CURVE_DOWN_TRS, sizeof(TEXT_CURVE_DOWN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CASCADE_UP_VERTICES[] =
{
  Vertex(0, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 21600), Vertex(21600, 0 CALCULATED_VALUE)
};

const unsigned short TEXT_CASCADE_UP_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_CASCADE_UP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x401, 1, 4)
};

const TextRectangle TEXT_CASCADE_UP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CASCADE_UP_DEFAULT_ADJUST[] =
{
  9600
};

const CustomShape CS_TEXT_CASCADE_UP(
  TEXT_CASCADE_UP_VERTICES, sizeof(TEXT_CASCADE_UP_VERTICES) / sizeof(Vertex),
  TEXT_CASCADE_UP_SEGMENTS, sizeof(TEXT_CASCADE_UP_SEGMENTS) / sizeof(unsigned short),
  TEXT_CASCADE_UP_CALC, sizeof(TEXT_CASCADE_UP_CALC) / sizeof(Calculation),
  TEXT_CASCADE_UP_DEFAULT_ADJUST, sizeof(TEXT_CASCADE_UP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CASCADE_UP_TRS, sizeof(TEXT_CASCADE_UP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CASCADE_DOWN_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 2 CALCULATED_VALUE), Vertex(0, 0 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_CASCADE_DOWN_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_CASCADE_DOWN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x401, 1, 4)
};

const TextRectangle TEXT_CASCADE_DOWN_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CASCADE_DOWN_DEFAULT_ADJUST[] =
{
  9600
};

const CustomShape CS_TEXT_CASCADE_DOWN(
  TEXT_CASCADE_DOWN_VERTICES, sizeof(TEXT_CASCADE_DOWN_VERTICES) / sizeof(Vertex),
  TEXT_CASCADE_DOWN_SEGMENTS, sizeof(TEXT_CASCADE_DOWN_SEGMENTS) / sizeof(unsigned short),
  TEXT_CASCADE_DOWN_CALC, sizeof(TEXT_CASCADE_DOWN_CALC) / sizeof(Calculation),
  TEXT_CASCADE_DOWN_DEFAULT_ADJUST, sizeof(TEXT_CASCADE_DOWN_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CASCADE_DOWN_TRS, sizeof(TEXT_CASCADE_DOWN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_WAVE_1_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_WAVE_1_SEGMENTS[] =
{
  0x4000, 0x2001, 0x8000, 0x4000, 0x2001, 0x8000
};

const Calculation TEXT_WAVE_1_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 15800, 0x400, 4460), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 7200, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 7200), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c)
};

const TextRectangle TEXT_WAVE_1_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_WAVE_1_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const CustomShape CS_TEXT_WAVE_1(
  TEXT_WAVE_1_VERTICES, sizeof(TEXT_WAVE_1_VERTICES) / sizeof(Vertex),
  TEXT_WAVE_1_SEGMENTS, sizeof(TEXT_WAVE_1_SEGMENTS) / sizeof(unsigned short),
  TEXT_WAVE_1_CALC, sizeof(TEXT_WAVE_1_CALC) / sizeof(Calculation),
  TEXT_WAVE_1_DEFAULT_ADJUST, sizeof(TEXT_WAVE_1_DEFAULT_ADJUST) / sizeof(int),
  TEXT_WAVE_1_TRS, sizeof(TEXT_WAVE_1_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_WAVE_2_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_WAVE_2_SEGMENTS[] =
{
  0x4000, 0x2001, 0x8000, 0x4000, 0x2001, 0x8000
};

const Calculation TEXT_WAVE_2_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 15800, 0x400, 4460), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 7200, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 7200), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c)
};

const TextRectangle TEXT_WAVE_2_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_WAVE_2_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const CustomShape CS_TEXT_WAVE_2(
  TEXT_WAVE_2_VERTICES, sizeof(TEXT_WAVE_2_VERTICES) / sizeof(Vertex),
  TEXT_WAVE_2_SEGMENTS, sizeof(TEXT_WAVE_2_SEGMENTS) / sizeof(unsigned short),
  TEXT_WAVE_2_CALC, sizeof(TEXT_WAVE_2_CALC) / sizeof(Calculation),
  TEXT_WAVE_2_DEFAULT_ADJUST, sizeof(TEXT_WAVE_2_DEFAULT_ADJUST) / sizeof(int),
  TEXT_WAVE_2_TRS, sizeof(TEXT_WAVE_2_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_WAVE_3_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_WAVE_3_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_WAVE_3_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 7900, 0x400, 2230), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 3600, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 3600), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c), Calculation(0xa000, 0x412, 0, 0x40e), Calculation(0x6000, 0x412, 0x40e, 0), Calculation(0xa000, 0x413, 0, 0x40e), Calculation(0x6000, 0x413, 0x40e, 0)
};

const TextRectangle TEXT_WAVE_3_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_WAVE_3_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const CustomShape CS_TEXT_WAVE_3(
  TEXT_WAVE_3_VERTICES, sizeof(TEXT_WAVE_3_VERTICES) / sizeof(Vertex),
  TEXT_WAVE_3_SEGMENTS, sizeof(TEXT_WAVE_3_SEGMENTS) / sizeof(unsigned short),
  TEXT_WAVE_3_CALC, sizeof(TEXT_WAVE_3_CALC) / sizeof(Calculation),
  TEXT_WAVE_3_DEFAULT_ADJUST, sizeof(TEXT_WAVE_3_DEFAULT_ADJUST) / sizeof(int),
  TEXT_WAVE_3_TRS, sizeof(TEXT_WAVE_3_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_WAVE_4_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_WAVE_4_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_WAVE_4_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 7900, 0x400, 2230), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 3600, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 3600), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c), Calculation(0xa000, 0x412, 0, 0x40e), Calculation(0x6000, 0x412, 0x40e, 0), Calculation(0xa000, 0x413, 0, 0x40e), Calculation(0x6000, 0x413, 0x40e, 0)
};

const TextRectangle TEXT_WAVE_4_TRS[] =
{
  TextRectangle(Vertex(5 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 23 CALCULATED_VALUE))
};

const int TEXT_WAVE_4_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const Vertex TEXT_WAVE_4_GLUE_POINTS[] =
{
  Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 10800), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 10800)
};

const CustomShape CS_TEXT_WAVE_4(
  TEXT_WAVE_4_VERTICES, sizeof(TEXT_WAVE_4_VERTICES) / sizeof(Vertex),
  TEXT_WAVE_4_SEGMENTS, sizeof(TEXT_WAVE_4_SEGMENTS) / sizeof(unsigned short),
  TEXT_WAVE_4_CALC, sizeof(TEXT_WAVE_4_CALC) / sizeof(Calculation),
  TEXT_WAVE_4_DEFAULT_ADJUST, sizeof(TEXT_WAVE_4_DEFAULT_ADJUST) / sizeof(int),
  TEXT_WAVE_4_TRS, sizeof(TEXT_WAVE_4_TRS) / sizeof(TextRectangle),
  21600, 21600,
  TEXT_WAVE_4_GLUE_POINTS, sizeof(TEXT_WAVE_4_GLUE_POINTS) / sizeof(Vertex));

const Vertex TEXT_INFLATE_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(4100, 1 CALCULATED_VALUE), Vertex(7300, 0), Vertex(10800, 0), Vertex(14300, 0), Vertex(17500, 1 CALCULATED_VALUE), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(4100, 3 CALCULATED_VALUE), Vertex(7300, 21600), Vertex(10800, 21600), Vertex(14300, 21600), Vertex(17500, 3 CALCULATED_VALUE), Vertex(21600, 2 CALCULATED_VALUE)
};

const unsigned short TEXT_INFLATE_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_INFLATE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x4001, 1530, 0x400, 4650), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x8000, 21600, 0, 0x401)
};

const TextRectangle TEXT_INFLATE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_INFLATE_DEFAULT_ADJUST[] =
{
  2950
};

const CustomShape CS_TEXT_INFLATE(
  TEXT_INFLATE_VERTICES, sizeof(TEXT_INFLATE_VERTICES) / sizeof(Vertex),
  TEXT_INFLATE_SEGMENTS, sizeof(TEXT_INFLATE_SEGMENTS) / sizeof(unsigned short),
  TEXT_INFLATE_CALC, sizeof(TEXT_INFLATE_CALC) / sizeof(Calculation),
  TEXT_INFLATE_DEFAULT_ADJUST, sizeof(TEXT_INFLATE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_INFLATE_TRS, sizeof(TEXT_INFLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_DEFLATE_VERTICES[] =
{
  Vertex(0, 0), Vertex(3500, 1 CALCULATED_VALUE), Vertex(7100, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(14500, 0 CALCULATED_VALUE), Vertex(18100, 1 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 21600), Vertex(3500, 3 CALCULATED_VALUE), Vertex(7100, 2 CALCULATED_VALUE), Vertex(10800, 2 CALCULATED_VALUE), Vertex(14500, 2 CALCULATED_VALUE), Vertex(18100, 3 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_DEFLATE_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_DEFLATE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 5320, 7100), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x8000, 21600, 0, 0x401)
};

const TextRectangle TEXT_DEFLATE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_DEFLATE_DEFAULT_ADJUST[] =
{
  8100
};

const CustomShape CS_TEXT_DEFLATE(
  TEXT_DEFLATE_VERTICES, sizeof(TEXT_DEFLATE_VERTICES) / sizeof(Vertex),
  TEXT_DEFLATE_SEGMENTS, sizeof(TEXT_DEFLATE_SEGMENTS) / sizeof(unsigned short),
  TEXT_DEFLATE_CALC, sizeof(TEXT_DEFLATE_CALC) / sizeof(Calculation),
  TEXT_DEFLATE_DEFAULT_ADJUST, sizeof(TEXT_DEFLATE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_DEFLATE_TRS, sizeof(TEXT_DEFLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_INFLATE_BOTTOM_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0, 0 CALCULATED_VALUE), Vertex(3500, 3 CALCULATED_VALUE), Vertex(7300, 21600), Vertex(10800, 21600), Vertex(14300, 21600), Vertex(18100, 3 CALCULATED_VALUE), Vertex(21600, 0 CALCULATED_VALUE)
};

const unsigned short TEXT_INFLATE_BOTTOM_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_INFLATE_BOTTOM_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, 0x400, 0, 11150), Calculation(0x2001, 0x401, 3900, 10450), Calculation(0x2000, 0x402, 17700, 0)
};

const TextRectangle TEXT_INFLATE_BOTTOM_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_INFLATE_BOTTOM_DEFAULT_ADJUST[] =
{
  14700
};

const CustomShape CS_TEXT_INFLATE_BOTTOM(
  TEXT_INFLATE_BOTTOM_VERTICES, sizeof(TEXT_INFLATE_BOTTOM_VERTICES) / sizeof(Vertex),
  TEXT_INFLATE_BOTTOM_SEGMENTS, sizeof(TEXT_INFLATE_BOTTOM_SEGMENTS) / sizeof(unsigned short),
  TEXT_INFLATE_BOTTOM_CALC, sizeof(TEXT_INFLATE_BOTTOM_CALC) / sizeof(Calculation),
  TEXT_INFLATE_BOTTOM_DEFAULT_ADJUST, sizeof(TEXT_INFLATE_BOTTOM_DEFAULT_ADJUST) / sizeof(int),
  TEXT_INFLATE_BOTTOM_TRS, sizeof(TEXT_INFLATE_BOTTOM_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_DEFLATE_BOTTOM_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0, 21600), Vertex(2900, 3 CALCULATED_VALUE), Vertex(7200, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(14400, 0 CALCULATED_VALUE), Vertex(18700, 3 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_DEFLATE_BOTTOM_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_DEFLATE_BOTTOM_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, 0x400, 0, 1350), Calculation(0x2001, 0x401, 12070, 20250), Calculation(0x2000, 0x402, 9530, 0)
};

const TextRectangle TEXT_DEFLATE_BOTTOM_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_DEFLATE_BOTTOM_DEFAULT_ADJUST[] =
{
  11500
};

const CustomShape CS_TEXT_DEFLATE_BOTTOM(
  TEXT_DEFLATE_BOTTOM_VERTICES, sizeof(TEXT_DEFLATE_BOTTOM_VERTICES) / sizeof(Vertex),
  TEXT_DEFLATE_BOTTOM_SEGMENTS, sizeof(TEXT_DEFLATE_BOTTOM_SEGMENTS) / sizeof(unsigned short),
  TEXT_DEFLATE_BOTTOM_CALC, sizeof(TEXT_DEFLATE_BOTTOM_CALC) / sizeof(Calculation),
  TEXT_DEFLATE_BOTTOM_DEFAULT_ADJUST, sizeof(TEXT_DEFLATE_BOTTOM_DEFAULT_ADJUST) / sizeof(int),
  TEXT_DEFLATE_BOTTOM_TRS, sizeof(TEXT_DEFLATE_BOTTOM_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_INFLATE_TOP_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(3500, 1 CALCULATED_VALUE), Vertex(7300, 0), Vertex(10800, 0), Vertex(14300, 0), Vertex(18100, 1 CALCULATED_VALUE), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_INFLATE_TOP_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_INFLATE_TOP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 3900, 10450)
};

const TextRectangle TEXT_INFLATE_TOP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_INFLATE_TOP_DEFAULT_ADJUST[] =
{
  6900
};

const CustomShape CS_TEXT_INFLATE_TOP(
  TEXT_INFLATE_TOP_VERTICES, sizeof(TEXT_INFLATE_TOP_VERTICES) / sizeof(Vertex),
  TEXT_INFLATE_TOP_SEGMENTS, sizeof(TEXT_INFLATE_TOP_SEGMENTS) / sizeof(unsigned short),
  TEXT_INFLATE_TOP_CALC, sizeof(TEXT_INFLATE_TOP_CALC) / sizeof(Calculation),
  TEXT_INFLATE_TOP_DEFAULT_ADJUST, sizeof(TEXT_INFLATE_TOP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_INFLATE_TOP_TRS, sizeof(TEXT_INFLATE_TOP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_DEFLATE_TOP_VERTICES[] =
{
  Vertex(0, 0), Vertex(2900, 1 CALCULATED_VALUE), Vertex(7200, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(14400, 0 CALCULATED_VALUE), Vertex(18700, 1 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_DEFLATE_TOP_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_DEFLATE_TOP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2001, 0x400, 12070, 20250)
};

const TextRectangle TEXT_DEFLATE_TOP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_DEFLATE_TOP_DEFAULT_ADJUST[] =
{
  10100
};

const CustomShape CS_TEXT_DEFLATE_TOP(
  TEXT_DEFLATE_TOP_VERTICES, sizeof(TEXT_DEFLATE_TOP_VERTICES) / sizeof(Vertex),
  TEXT_DEFLATE_TOP_SEGMENTS, sizeof(TEXT_DEFLATE_TOP_SEGMENTS) / sizeof(unsigned short),
  TEXT_DEFLATE_TOP_CALC, sizeof(TEXT_DEFLATE_TOP_CALC) / sizeof(Calculation),
  TEXT_DEFLATE_TOP_DEFAULT_ADJUST, sizeof(TEXT_DEFLATE_TOP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_DEFLATE_TOP_TRS, sizeof(TEXT_DEFLATE_TOP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_DEFLATE_INFLATE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0, 10100), Vertex(3300, 3 CALCULATED_VALUE), Vertex(7100, 5 CALCULATED_VALUE), Vertex(10800, 5 CALCULATED_VALUE), Vertex(14500, 5 CALCULATED_VALUE), Vertex(18300, 3 CALCULATED_VALUE), Vertex(21600, 10100), Vertex(0, 11500), Vertex(3300, 4 CALCULATED_VALUE), Vertex(7100, 6 CALCULATED_VALUE), Vertex(10800, 6 CALCULATED_VALUE), Vertex(14500, 6 CALCULATED_VALUE), Vertex(18300, 4 CALCULATED_VALUE), Vertex(21600, 11500), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_DEFLATE_INFLATE_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_DEFLATE_INFLATE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 10800, 0, 0x400), Calculation(0x2001, 0x401, 5770, 9500), Calculation(0x8000, 10100, 0, 0x402), Calculation(0x8000, 11500, 0, 0x402), Calculation(0x2000, 0x400, 0, 700), Calculation(0x2000, 0x400, 700, 0)
};

const TextRectangle TEXT_DEFLATE_INFLATE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_DEFLATE_INFLATE_DEFAULT_ADJUST[] =
{
  6500
};

const CustomShape CS_TEXT_DEFLATE_INFLATE(
  TEXT_DEFLATE_INFLATE_VERTICES, sizeof(TEXT_DEFLATE_INFLATE_VERTICES) / sizeof(Vertex),
  TEXT_DEFLATE_INFLATE_SEGMENTS, sizeof(TEXT_DEFLATE_INFLATE_SEGMENTS) / sizeof(unsigned short),
  TEXT_DEFLATE_INFLATE_CALC, sizeof(TEXT_DEFLATE_INFLATE_CALC) / sizeof(Calculation),
  TEXT_DEFLATE_INFLATE_DEFAULT_ADJUST, sizeof(TEXT_DEFLATE_INFLATE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_DEFLATE_INFLATE_TRS, sizeof(TEXT_DEFLATE_INFLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_DEFLATE_INFLATE_DEFLATE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0, 6600), Vertex(3600, 3 CALCULATED_VALUE), Vertex(7250, 4 CALCULATED_VALUE), Vertex(10800, 4 CALCULATED_VALUE), Vertex(14350, 4 CALCULATED_VALUE), Vertex(18000, 3 CALCULATED_VALUE), Vertex(21600, 6600), Vertex(0, 7500), Vertex(3600, 5 CALCULATED_VALUE), Vertex(7250, 6 CALCULATED_VALUE), Vertex(10800, 6 CALCULATED_VALUE), Vertex(14350, 6 CALCULATED_VALUE), Vertex(18000, 5 CALCULATED_VALUE), Vertex(21600, 7500), Vertex(0, 14100), Vertex(3600, 9 CALCULATED_VALUE), Vertex(7250, 10 CALCULATED_VALUE), Vertex(10800, 10 CALCULATED_VALUE), Vertex(14350, 10 CALCULATED_VALUE), Vertex(18000, 9 CALCULATED_VALUE), Vertex(21600, 14100), Vertex(0, 15000), Vertex(3600, 7 CALCULATED_VALUE), Vertex(7250, 8 CALCULATED_VALUE), Vertex(10800, 8 CALCULATED_VALUE), Vertex(14350, 8 CALCULATED_VALUE), Vertex(18000, 7 CALCULATED_VALUE), Vertex(21600, 15000), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_DEFLATE_INFLATE_DEFLATE_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_DEFLATE_INFLATE_DEFLATE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 850), Calculation(0x2001, 0x400, 6120, 8700), Calculation(0x2000, 0x401, 0, 4280), Calculation(0x4000, 6600, 0x402, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 450), Calculation(0x2000, 0x403, 900, 0), Calculation(0x2000, 0x404, 900, 0), Calculation(0x8000, 21600, 0, 0x403), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x8000, 21600, 0, 0x405), Calculation(0x8000, 21600, 0, 0x406)
};

const TextRectangle TEXT_DEFLATE_INFLATE_DEFLATE_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_DEFLATE_INFLATE_DEFLATE_DEFAULT_ADJUST[] =
{
  6050
};

const CustomShape CS_TEXT_DEFLATE_INFLATE_DEFLATE(
  TEXT_DEFLATE_INFLATE_DEFLATE_VERTICES, sizeof(TEXT_DEFLATE_INFLATE_DEFLATE_VERTICES) / sizeof(Vertex),
  TEXT_DEFLATE_INFLATE_DEFLATE_SEGMENTS, sizeof(TEXT_DEFLATE_INFLATE_DEFLATE_SEGMENTS) / sizeof(unsigned short),
  TEXT_DEFLATE_INFLATE_DEFLATE_CALC, sizeof(TEXT_DEFLATE_INFLATE_DEFLATE_CALC) / sizeof(Calculation),
  TEXT_DEFLATE_INFLATE_DEFLATE_DEFAULT_ADJUST, sizeof(TEXT_DEFLATE_INFLATE_DEFLATE_DEFAULT_ADJUST) / sizeof(int),
  TEXT_DEFLATE_INFLATE_DEFLATE_TRS, sizeof(TEXT_DEFLATE_INFLATE_DEFLATE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_FADE_RIGHT_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0 CALCULATED_VALUE), Vertex(0, 21600), Vertex(21600, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_FADE_RIGHT_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_FADE_RIGHT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_FADE_RIGHT_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_FADE_RIGHT_DEFAULT_ADJUST[] =
{
  7200
};

const CustomShape CS_TEXT_FADE_RIGHT(
  TEXT_FADE_RIGHT_VERTICES, sizeof(TEXT_FADE_RIGHT_VERTICES) / sizeof(Vertex),
  TEXT_FADE_RIGHT_SEGMENTS, sizeof(TEXT_FADE_RIGHT_SEGMENTS) / sizeof(unsigned short),
  TEXT_FADE_RIGHT_CALC, sizeof(TEXT_FADE_RIGHT_CALC) / sizeof(Calculation),
  TEXT_FADE_RIGHT_DEFAULT_ADJUST, sizeof(TEXT_FADE_RIGHT_DEFAULT_ADJUST) / sizeof(int),
  TEXT_FADE_RIGHT_TRS, sizeof(TEXT_FADE_RIGHT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_FADE_LEFT_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 1 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_FADE_LEFT_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_FADE_LEFT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_FADE_LEFT_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_FADE_LEFT_DEFAULT_ADJUST[] =
{
  7200
};

const CustomShape CS_TEXT_FADE_LEFT(
  TEXT_FADE_LEFT_VERTICES, sizeof(TEXT_FADE_LEFT_VERTICES) / sizeof(Vertex),
  TEXT_FADE_LEFT_SEGMENTS, sizeof(TEXT_FADE_LEFT_SEGMENTS) / sizeof(unsigned short),
  TEXT_FADE_LEFT_CALC, sizeof(TEXT_FADE_LEFT_CALC) / sizeof(Calculation),
  TEXT_FADE_LEFT_DEFAULT_ADJUST, sizeof(TEXT_FADE_LEFT_DEFAULT_ADJUST) / sizeof(int),
  TEXT_FADE_LEFT_TRS, sizeof(TEXT_FADE_LEFT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_FADE_UP_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 0), Vertex(0, 21600), Vertex(21600, 21600)
};

const unsigned short TEXT_FADE_UP_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_FADE_UP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_FADE_UP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_FADE_UP_DEFAULT_ADJUST[] =
{
  7200
};

const CustomShape CS_TEXT_FADE_UP(
  TEXT_FADE_UP_VERTICES, sizeof(TEXT_FADE_UP_VERTICES) / sizeof(Vertex),
  TEXT_FADE_UP_SEGMENTS, sizeof(TEXT_FADE_UP_SEGMENTS) / sizeof(unsigned short),
  TEXT_FADE_UP_CALC, sizeof(TEXT_FADE_UP_CALC) / sizeof(Calculation),
  TEXT_FADE_UP_DEFAULT_ADJUST, sizeof(TEXT_FADE_UP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_FADE_UP_TRS, sizeof(TEXT_FADE_UP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_FADE_DOWN_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(0 CALCULATED_VALUE, 21600), Vertex(1 CALCULATED_VALUE, 21600)
};

const unsigned short TEXT_FADE_DOWN_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_FADE_DOWN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_FADE_DOWN_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_FADE_DOWN_DEFAULT_ADJUST[] =
{
  7200
};

const CustomShape CS_TEXT_FADE_DOWN(
  TEXT_FADE_DOWN_VERTICES, sizeof(TEXT_FADE_DOWN_VERTICES) / sizeof(Vertex),
  TEXT_FADE_DOWN_SEGMENTS, sizeof(TEXT_FADE_DOWN_SEGMENTS) / sizeof(unsigned short),
  TEXT_FADE_DOWN_CALC, sizeof(TEXT_FADE_DOWN_CALC) / sizeof(Calculation),
  TEXT_FADE_DOWN_DEFAULT_ADJUST, sizeof(TEXT_FADE_DOWN_DEFAULT_ADJUST) / sizeof(int),
  TEXT_FADE_DOWN_TRS, sizeof(TEXT_FADE_DOWN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_SLANT_UP_VERTICES[] =
{
  Vertex(0, 0 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 21600), Vertex(21600, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_SLANT_UP_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_SLANT_UP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_SLANT_UP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_SLANT_UP_DEFAULT_ADJUST[] =
{
  12000
};

const CustomShape CS_TEXT_SLANT_UP(
  TEXT_SLANT_UP_VERTICES, sizeof(TEXT_SLANT_UP_VERTICES) / sizeof(Vertex),
  TEXT_SLANT_UP_SEGMENTS, sizeof(TEXT_SLANT_UP_SEGMENTS) / sizeof(unsigned short),
  TEXT_SLANT_UP_CALC, sizeof(TEXT_SLANT_UP_CALC) / sizeof(Calculation),
  TEXT_SLANT_UP_DEFAULT_ADJUST, sizeof(TEXT_SLANT_UP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_SLANT_UP_TRS, sizeof(TEXT_SLANT_UP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_SLANT_DOWN_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(0, 0 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_SLANT_DOWN_SEGMENTS[] =
{
  0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation TEXT_SLANT_DOWN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle TEXT_SLANT_DOWN_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_SLANT_DOWN_DEFAULT_ADJUST[] =
{
  12000
};

const CustomShape CS_TEXT_SLANT_DOWN(
  TEXT_SLANT_DOWN_VERTICES, sizeof(TEXT_SLANT_DOWN_VERTICES) / sizeof(Vertex),
  TEXT_SLANT_DOWN_SEGMENTS, sizeof(TEXT_SLANT_DOWN_SEGMENTS) / sizeof(unsigned short),
  TEXT_SLANT_DOWN_CALC, sizeof(TEXT_SLANT_DOWN_CALC) / sizeof(Calculation),
  TEXT_SLANT_DOWN_DEFAULT_ADJUST, sizeof(TEXT_SLANT_DOWN_DEFAULT_ADJUST) / sizeof(int),
  TEXT_SLANT_DOWN_TRS, sizeof(TEXT_SLANT_DOWN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CAN_UP_VERTICES[] =
{
  Vertex(0, 1 CALCULATED_VALUE), Vertex(900, 0), Vertex(7100, 0), Vertex(10800, 0), Vertex(14500, 0), Vertex(20700, 0), Vertex(21600, 1 CALCULATED_VALUE), Vertex(0, 21600), Vertex(900, 4 CALCULATED_VALUE), Vertex(7100, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(14500, 0 CALCULATED_VALUE), Vertex(20700, 4 CALCULATED_VALUE), Vertex(21600, 21600)
};

const unsigned short TEXT_CAN_UP_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_CAN_UP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 14400), Calculation(0x4001, 5470, 0x402, 7200), Calculation(0x4000, 16130, 0x403, 0)
};

const TextRectangle TEXT_CAN_UP_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CAN_UP_DEFAULT_ADJUST[] =
{
  18500
};

const CustomShape CS_TEXT_CAN_UP(
  TEXT_CAN_UP_VERTICES, sizeof(TEXT_CAN_UP_VERTICES) / sizeof(Vertex),
  TEXT_CAN_UP_SEGMENTS, sizeof(TEXT_CAN_UP_SEGMENTS) / sizeof(unsigned short),
  TEXT_CAN_UP_CALC, sizeof(TEXT_CAN_UP_CALC) / sizeof(Calculation),
  TEXT_CAN_UP_DEFAULT_ADJUST, sizeof(TEXT_CAN_UP_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CAN_UP_TRS, sizeof(TEXT_CAN_UP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex TEXT_CAN_DOWN_VERTICES[] =
{
  Vertex(0, 0), Vertex(900, 2 CALCULATED_VALUE), Vertex(7100, 0 CALCULATED_VALUE), Vertex(10800, 0 CALCULATED_VALUE), Vertex(14500, 0 CALCULATED_VALUE), Vertex(20700, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(0, 1 CALCULATED_VALUE), Vertex(900, 21600), Vertex(7100, 21600), Vertex(10800, 21600), Vertex(14500, 21600), Vertex(20700, 21600), Vertex(21600, 1 CALCULATED_VALUE)
};

const unsigned short TEXT_CAN_DOWN_SEGMENTS[] =
{
  0x4000, 0x2002, 0x8000, 0x4000, 0x2002, 0x8000
};

const Calculation TEXT_CAN_DOWN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x4001, 5470, 0x400, 7200)
};

const TextRectangle TEXT_CAN_DOWN_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int TEXT_CAN_DOWN_DEFAULT_ADJUST[] =
{
  3100
};

const CustomShape CS_TEXT_CAN_DOWN(
  TEXT_CAN_DOWN_VERTICES, sizeof(TEXT_CAN_DOWN_VERTICES) / sizeof(Vertex),
  TEXT_CAN_DOWN_SEGMENTS, sizeof(TEXT_CAN_DOWN_SEGMENTS) / sizeof(unsigned short),
  TEXT_CAN_DOWN_CALC, sizeof(TEXT_CAN_DOWN_CALC) / sizeof(Calculation),
  TEXT_CAN_DOWN_DEFAULT_ADJUST, sizeof(TEXT_CAN_DOWN_DEFAULT_ADJUST) / sizeof(int),
  TEXT_CAN_DOWN_TRS, sizeof(TEXT_CAN_DOWN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex FLOW_CHART_ALTERNATE_PROCESS_VERTICES[] =
{
  Vertex(0, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0), Vertex(1 CALCULATED_VALUE, 0), Vertex(21600, 2 CALCULATED_VALUE), Vertex(21600, 3 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 21600), Vertex(0 CALCULATED_VALUE, 21600), Vertex(0, 3 CALCULATED_VALUE)
};

const unsigned short FLOW_CHART_ALTERNATE_PROCESS_SEGMENTS[] =
{
  0x4000, 0xa801, 0x0001, 0xa701, 0x0001, 0xa801, 0x0001, 0xa701, 0x6000, 0x8000
};

const Calculation FLOW_CHART_ALTERNATE_PROCESS_CALC[] =
{
  Calculation(0x2000, PROP_GEO_LEFT, 2540, 0), Calculation(0x2000, PROP_GEO_RIGHT, 0, 2540), Calculation(0x2000, PROP_GEO_TOP, 2540, 0), Calculation(0x2000, PROP_GEO_BOTTOM, 0, 2540), Calculation(0x2000, PROP_GEO_LEFT, 800, 0), Calculation(0x2000, PROP_GEO_RIGHT, 0, 800), Calculation(0x2000, PROP_GEO_TOP, 800, 0), Calculation(0x2000, PROP_GEO_BOTTOM, 0, 800)
};

const TextRectangle FLOW_CHART_ALTERNATE_PROCESS_TRS[] =
{
  TextRectangle(Vertex(4 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 7 CALCULATED_VALUE))
};

const Vertex FLOW_CHART_ALTERNATE_PROCESS_GLUE_POINTS[] =
{
  Vertex(0, 10800), Vertex(10800, 0), Vertex(21600, 10800), Vertex(10800, 21600)
};

const CustomShape CS_FLOW_CHART_ALTERNATE_PROCESS(
  FLOW_CHART_ALTERNATE_PROCESS_VERTICES, sizeof(FLOW_CHART_ALTERNATE_PROCESS_VERTICES) / sizeof(Vertex),
  FLOW_CHART_ALTERNATE_PROCESS_SEGMENTS, sizeof(FLOW_CHART_ALTERNATE_PROCESS_SEGMENTS) / sizeof(unsigned short),
  FLOW_CHART_ALTERNATE_PROCESS_CALC, sizeof(FLOW_CHART_ALTERNATE_PROCESS_CALC) / sizeof(Calculation),
  NULL, 0,
  FLOW_CHART_ALTERNATE_PROCESS_TRS, sizeof(FLOW_CHART_ALTERNATE_PROCESS_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_ALTERNATE_PROCESS_GLUE_POINTS, sizeof(FLOW_CHART_ALTERNATE_PROCESS_GLUE_POINTS) / sizeof(Vertex));

const Vertex FLOW_CHART_OFFPAGE_CONNECTOR_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 17150), Vertex(10800, 21600), Vertex(0, 17150), Vertex(0, 0)
};

const TextRectangle FLOW_CHART_OFFPAGE_CONNECTOR_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 17150))
};

const Vertex FLOW_CHART_OFFPAGE_CONNECTOR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_FLOW_CHART_OFFPAGE_CONNECTOR(
  FLOW_CHART_OFFPAGE_CONNECTOR_VERTICES, sizeof(FLOW_CHART_OFFPAGE_CONNECTOR_VERTICES) / sizeof(Vertex),
  NULL, 0,
  NULL, 0,
  NULL, 0,
  FLOW_CHART_OFFPAGE_CONNECTOR_TRS, sizeof(FLOW_CHART_OFFPAGE_CONNECTOR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  FLOW_CHART_OFFPAGE_CONNECTOR_GLUE_POINTS, sizeof(FLOW_CHART_OFFPAGE_CONNECTOR_GLUE_POINTS) / sizeof(Vertex));

const Vertex LEFT_RIGHT_UP_ARROW_VERTICES[] =
{
  Vertex(10800, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 10800), Vertex(5 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(5 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 3 CALCULATED_VALUE), Vertex(0, 10800), Vertex(2 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(2 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const unsigned short LEFT_RIGHT_UP_ARROW_SEGMENTS[] =
{
  0x4000, 0x0010, 0x6001, 0x8000
};

const Calculation LEFT_RIGHT_UP_ARROW_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x6001, 0x0403, PROP_ADJUST_VAL_FIRST + 2, 21600), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST + 1), Calculation(0x8000, 21600, 0, 0x0402)
};

const TextRectangle LEFT_RIGHT_UP_ARROW_TRS[] =
{
  TextRectangle(Vertex(0, 0), Vertex(21600, 21600))
};

const int LEFT_RIGHT_UP_ARROW_DEFAULT_ADJUST[] =
{
  6500, 8600, 6200
};

const CustomShape CS_LEFT_RIGHT_UP_ARROW(
  LEFT_RIGHT_UP_ARROW_VERTICES, sizeof(LEFT_RIGHT_UP_ARROW_VERTICES) / sizeof(Vertex),
  LEFT_RIGHT_UP_ARROW_SEGMENTS, sizeof(LEFT_RIGHT_UP_ARROW_SEGMENTS) / sizeof(unsigned short),
  LEFT_RIGHT_UP_ARROW_CALC, sizeof(LEFT_RIGHT_UP_ARROW_CALC) / sizeof(Calculation),
  LEFT_RIGHT_UP_ARROW_DEFAULT_ADJUST, sizeof(LEFT_RIGHT_UP_ARROW_DEFAULT_ADJUST) / sizeof(int),
  LEFT_RIGHT_UP_ARROW_TRS, sizeof(LEFT_RIGHT_UP_ARROW_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex SUN_VERTICES[] =
{
  Vertex(0, 10800), Vertex(4 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0x0a CALCULATED_VALUE, 0x0b CALCULATED_VALUE), Vertex(0x0c CALCULATED_VALUE, 0x0d CALCULATED_VALUE), Vertex(0x0e CALCULATED_VALUE, 0x0f CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x11 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x13 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0x15 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x17 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x19 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x1b CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x1d CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x1f CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x21 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0x23 CALCULATED_VALUE), Vertex(0x24 CALCULATED_VALUE, 0x25 CALCULATED_VALUE), Vertex(0x26 CALCULATED_VALUE, 0x27 CALCULATED_VALUE), Vertex(0x28 CALCULATED_VALUE, 0x29 CALCULATED_VALUE), Vertex(0x2a CALCULATED_VALUE, 0x2b CALCULATED_VALUE), Vertex(0x2c CALCULATED_VALUE, 0x2d CALCULATED_VALUE), Vertex(0x2e CALCULATED_VALUE, 0x2f CALCULATED_VALUE), Vertex(0x30 CALCULATED_VALUE, 0x31 CALCULATED_VALUE), Vertex(0x32 CALCULATED_VALUE, 0x33 CALCULATED_VALUE), Vertex(10800, 10800), Vertex(0x36 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0, 360)
};

const unsigned short SUN_SEGMENTS[] =
{
  0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0xa203, 0x6000, 0x8000
};

const Calculation SUN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 2700), Calculation(0x2001, 0x402, 5080, 7425), Calculation(0x2000, 0x403, 2540, 0), Calculation(0x8000, 10125, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x405, 2120, 7425), Calculation(0x2000, 0x406, 210, 0), Calculation(0x4000, 10800, 0x407, 0), Calculation(0x8000, 10800, 0, 0x407), Calculation(0x0081, 0, 10800, 45), Calculation(0x0082, 0, 10800, 45), Calculation(0x6081, 0x404, 0x408, 45), Calculation(0x6082, 0x404, 0x408, 45), Calculation(0x6081, 0x404, 0x409, 45), Calculation(0x6082, 0x404, 0x409, 45), Calculation(0x0081, 0, 10800, 90), Calculation(0x0082, 0, 10800, 90), Calculation(0x6081, 0x404, 0x408, 90), Calculation(0x6082, 0x404, 0x408, 90), Calculation(0x6081, 0x404, 0x409, 90), Calculation(0x6082, 0x404, 0x409, 90), Calculation(0x0081, 0, 10800, 135), Calculation(0x0082, 0, 10800, 135), Calculation(0x6081, 0x404, 0x408, 135), Calculation(0x6082, 0x404, 0x408, 135), Calculation(0x6081, 0x404, 0x409, 135), Calculation(0x6082, 0x404, 0x409, 135), Calculation(0x0081, 0, 10800, 180), Calculation(0x0082, 0, 10800, 180), Calculation(0x6081, 0x404, 0x408, 180), Calculation(0x6082, 0x404, 0x408, 180), Calculation(0x6081, 0x404, 0x409, 180), Calculation(0x6082, 0x404, 0x409, 180), Calculation(0x0081, 0, 10800, 225), Calculation(0x0082, 0, 10800, 225), Calculation(0x6081, 0x404, 0x408, 225), Calculation(0x6082, 0x404, 0x408, 225), Calculation(0x6081, 0x404, 0x409, 225), Calculation(0x6082, 0x404, 0x409, 225), Calculation(0x0081, 0, 10800, 270), Calculation(0x0082, 0, 10800, 270), Calculation(0x6081, 0x404, 0x408, 270), Calculation(0x6082, 0x404, 0x408, 270), Calculation(0x6081, 0x404, 0x409, 270), Calculation(0x6082, 0x404, 0x409, 270), Calculation(0x0081, 0, 10800, 315), Calculation(0x0082, 0, 10800, 315), Calculation(0x6081, 0x404, 0x408, 315), Calculation(0x6082, 0x404, 0x408, 315), Calculation(0x6081, 0x404, 0x409, 315), Calculation(0x6082, 0x404, 0x409, 315), Calculation(0x2081, PROP_ADJUST_VAL_FIRST, 10800, 45), Calculation(0x2081, PROP_ADJUST_VAL_FIRST, 10800, 225), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST)
};

const TextRectangle SUN_TRS[] =
{
  TextRectangle(Vertex(0x34 CALCULATED_VALUE, 0x34 CALCULATED_VALUE), Vertex(0x35 CALCULATED_VALUE, 0x35 CALCULATED_VALUE))
};

const int SUN_DEFAULT_ADJUST[] =
{
  5400
};

const Vertex SUN_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_SUN(
  SUN_VERTICES, sizeof(SUN_VERTICES) / sizeof(Vertex),
  SUN_SEGMENTS, sizeof(SUN_SEGMENTS) / sizeof(unsigned short),
  SUN_CALC, sizeof(SUN_CALC) / sizeof(Calculation),
  SUN_DEFAULT_ADJUST, sizeof(SUN_DEFAULT_ADJUST) / sizeof(int),
  SUN_TRS, sizeof(SUN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  SUN_GLUE_POINTS, sizeof(SUN_GLUE_POINTS) / sizeof(Vertex));

const Vertex MOON_VERTICES[] =
{
  Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 5080), Vertex(0 CALCULATED_VALUE, 10800), Vertex(0 CALCULATED_VALUE, 16520), Vertex(3 CALCULATED_VALUE, 5 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(9740, 21600), Vertex(0, 16730), Vertex(0, 10800), Vertex(0, 4870), Vertex(9740, 0), Vertex(21600, 0)
};

const unsigned short MOON_SEGMENTS[] =
{
  0x4000, 0x2004, 0x6000, 0x8000
};

const Calculation MOON_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x401, 1, 2), Calculation(0x6000, 0x402, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1794, 10000), Calculation(0x8000, 21600, 0, 0x0404), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 400, 18900), Calculation(0x8081, 0, 10800, 0x406), Calculation(0x8082, 0, 10800, 0x406), Calculation(0x6000, 0x407, 0x407, 0), Calculation(0x8000, 21600, 0, 0x408)
};

const TextRectangle MOON_TRS[] =
{
  TextRectangle(Vertex(9 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 10 CALCULATED_VALUE))
};

const int MOON_DEFAULT_ADJUST[] =
{
  10800
};

const Vertex MOON_GLUE_POINTS[] =
{
  Vertex(21600, 0), Vertex(0, 10800), Vertex(21600, 21600), Vertex(0 CALCULATED_VALUE, 10800)
};

const CustomShape CS_MOON(
  MOON_VERTICES, sizeof(MOON_VERTICES) / sizeof(Vertex),
  MOON_SEGMENTS, sizeof(MOON_SEGMENTS) / sizeof(unsigned short),
  MOON_CALC, sizeof(MOON_CALC) / sizeof(Calculation),
  MOON_DEFAULT_ADJUST, sizeof(MOON_DEFAULT_ADJUST) / sizeof(int),
  MOON_TRS, sizeof(MOON_TRS) / sizeof(TextRectangle),
  21600, 21600,
  MOON_GLUE_POINTS, sizeof(MOON_GLUE_POINTS) / sizeof(Vertex));

const Vertex BRACKET_PAIR_VERTICES[] =
{
  Vertex(0 CALCULATED_VALUE, 0), Vertex(0, 1 CALCULATED_VALUE), Vertex(0, 2 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 21600), Vertex(3 CALCULATED_VALUE, 21600), Vertex(21600, 2 CALCULATED_VALUE), Vertex(21600, 1 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 0)
};

const unsigned short BRACKET_PAIR_SEGMENTS[] =
{
  0x4000, 0xa701, 0x0001, 0xa801, 0x8000, 0x4000, 0xa701, 0x0001, 0xa801, 0x8000
};

const Calculation BRACKET_PAIR_CALC[] =
{
  Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2082, PROP_ADJUST_VAL_FIRST, 0, 45), Calculation(0x2000, 0x404, 0, 10800), Calculation(0x8000, 0, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, 0x406, 0, 0x405), Calculation(0xa000, PROP_GEO_LEFT, 0, 0x407), Calculation(0xa000, PROP_GEO_TOP, 0, 0x407), Calculation(0x6000, PROP_GEO_RIGHT, 0x407, 0), Calculation(0x6000, PROP_GEO_BOTTOM, 0x407, 0), Calculation(0xa000, PROP_GEO_LEFT, 0, 0x405), Calculation(0xa000, PROP_GEO_TOP, 0, 0x405), Calculation(0x6000, PROP_GEO_RIGHT, 0x405, 0), Calculation(0x6000, PROP_GEO_BOTTOM, 0x405, 0)
};

const TextRectangle BRACKET_PAIR_TRS[] =
{
  TextRectangle(Vertex(8 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(10 CALCULATED_VALUE, 11 CALCULATED_VALUE))
};

const int BRACKET_PAIR_DEFAULT_ADJUST[] =
{
  3700
};

const Vertex BRACKET_PAIR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_BRACKET_PAIR(
  BRACKET_PAIR_VERTICES, sizeof(BRACKET_PAIR_VERTICES) / sizeof(Vertex),
  BRACKET_PAIR_SEGMENTS, sizeof(BRACKET_PAIR_SEGMENTS) / sizeof(unsigned short),
  BRACKET_PAIR_CALC, sizeof(BRACKET_PAIR_CALC) / sizeof(Calculation),
  BRACKET_PAIR_DEFAULT_ADJUST, sizeof(BRACKET_PAIR_DEFAULT_ADJUST) / sizeof(int),
  BRACKET_PAIR_TRS, sizeof(BRACKET_PAIR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  BRACKET_PAIR_GLUE_POINTS, sizeof(BRACKET_PAIR_GLUE_POINTS) / sizeof(Vertex));

const Vertex BRACE_PAIR_VERTICES[] =
{
  Vertex(4 CALCULATED_VALUE, 0), Vertex(0 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(0, 10800), Vertex(0 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(4 CALCULATED_VALUE, 21600), Vertex(8 CALCULATED_VALUE, 21600), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 7 CALCULATED_VALUE), Vertex(21600, 10800), Vertex(3 CALCULATED_VALUE, 6 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(8 CALCULATED_VALUE, 0)
};

const unsigned short BRACE_PAIR_SEGMENTS[] =
{
  0x4000, 0xa701, 0x0001, 0xa801, 0xa701, 0x0001, 0xa801, 0x8000, 0x4000, 0xa701, 0x0001, 0xa801, 0xa701, 0x0001, 0xa801, 0x8000
};

const Calculation BRACE_PAIR_CALC[] =
{
  Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x400, 2, 1), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 2, 1), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 21600, 0, 0x406), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x405), Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 1, 3), Calculation(0x6000, 0x409, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_LEFT, 0x40a, 0), Calculation(0x6000, PROP_GEO_TOP, 0x409, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x40a), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x409)
};

const TextRectangle BRACE_PAIR_TRS[] =
{
  TextRectangle(Vertex(11 CALCULATED_VALUE, 12 CALCULATED_VALUE), Vertex(13 CALCULATED_VALUE, 14 CALCULATED_VALUE))
};

const int BRACE_PAIR_DEFAULT_ADJUST[] =
{
  1800
};

const Vertex BRACE_PAIR_GLUE_POINTS[] =
{
  Vertex(10800, 0), Vertex(0, 10800), Vertex(10800, 21600), Vertex(21600, 10800)
};

const CustomShape CS_BRACE_PAIR(
  BRACE_PAIR_VERTICES, sizeof(BRACE_PAIR_VERTICES) / sizeof(Vertex),
  BRACE_PAIR_SEGMENTS, sizeof(BRACE_PAIR_SEGMENTS) / sizeof(unsigned short),
  BRACE_PAIR_CALC, sizeof(BRACE_PAIR_CALC) / sizeof(Calculation),
  BRACE_PAIR_DEFAULT_ADJUST, sizeof(BRACE_PAIR_DEFAULT_ADJUST) / sizeof(int),
  BRACE_PAIR_TRS, sizeof(BRACE_PAIR_TRS) / sizeof(TextRectangle),
  21600, 21600,
  BRACE_PAIR_GLUE_POINTS, sizeof(BRACE_PAIR_GLUE_POINTS) / sizeof(Vertex));

const Vertex DOUBLE_WAVE_VERTICES[] =
{
  Vertex(7 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(15 CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x1f CALCULATED_VALUE, 9 CALCULATED_VALUE), Vertex(16 CALCULATED_VALUE, 10 CALCULATED_VALUE), Vertex(12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(24 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(25 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(0x21 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 26 CALCULATED_VALUE), Vertex(27 CALCULATED_VALUE, 28 CALCULATED_VALUE), Vertex(29 CALCULATED_VALUE, 1 CALCULATED_VALUE)
};

const unsigned short DOUBLE_WAVE_SEGMENTS[] =
{
  0x4000, 0x2002, 0x0001, 0x2002, 0x6000, 0x8000
};

const Calculation DOUBLE_WAVE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x8000, 21600, 0, 0x400), Calculation(0x2000, PROP_ADJUST_VAL_FIRST + 1, 0, 0), Calculation(0x2000, 0x402, 0, 10800), Calculation(0x2001, 0x403, 2, 1), Calculation(0x2003, 0x404, 0, 0), Calculation(0x8000, 4320, 0, 0x405), Calculation(0xa006, 0x403, 0, 0x405), Calculation(0x4001, 7900, 0x400, 2230), Calculation(0xa000, 0x400, 0, 0x408), Calculation(0x6000, 0x400, 0x408, 0), Calculation(0x8000, 21600, 0, 0x404), Calculation(0x6006, 0x403, 0x40b, 21600), Calculation(0xa000, 0x40c, 0, 0x407), Calculation(0x2001, 0x405, 1, 2), Calculation(0xa000, 0x407, 3600, 0x40e), Calculation(0x6000, 0x40c, 0x40e, 3600), Calculation(0x2001, 0x40d, 1, 2), Calculation(0x6000, 0x407, 0x411, 0), Calculation(0x8000, 21600, 0, 0x412), Calculation(0x2001, 0x405, 1, 2), Calculation(0x8000, 21600, 0, 0x414), Calculation(0x2001, 0x400, 2, 1), Calculation(0x8000, 21600, 0, 0x416), Calculation(0x8000, 21600, 0, 0x407), Calculation(0x8000, 21600, 0, 0x40f), Calculation(0x6000, 0x401, 0x408, 0), Calculation(0x8000, 21600, 0, 0x410), Calculation(0xa000, 0x401, 0, 0x408), Calculation(0x8000, 21600, 0, 0x40c), Calculation(0xa000, 0x412, 0, 0x40e), Calculation(0x6000, 0x412, 0x40e, 0), Calculation(0xa000, 0x413, 0, 0x40e), Calculation(0x6000, 0x413, 0x40e, 0)
};

const TextRectangle DOUBLE_WAVE_TRS[] =
{
  TextRectangle(Vertex(5 CALCULATED_VALUE, 22 CALCULATED_VALUE), Vertex(11 CALCULATED_VALUE, 23 CALCULATED_VALUE))
};

const int DOUBLE_WAVE_DEFAULT_ADJUST[] =
{
  1400, 10800
};

const Vertex DOUBLE_WAVE_GLUE_POINTS[] =
{
  Vertex(0x12 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 10800), Vertex(0x13 CALCULATED_VALUE, 1 CALCULATED_VALUE), Vertex(0x15 CALCULATED_VALUE, 10800)
};

const CustomShape CS_DOUBLE_WAVE(
  DOUBLE_WAVE_VERTICES, sizeof(DOUBLE_WAVE_VERTICES) / sizeof(Vertex),
  DOUBLE_WAVE_SEGMENTS, sizeof(DOUBLE_WAVE_SEGMENTS) / sizeof(unsigned short),
  DOUBLE_WAVE_CALC, sizeof(DOUBLE_WAVE_CALC) / sizeof(Calculation),
  DOUBLE_WAVE_DEFAULT_ADJUST, sizeof(DOUBLE_WAVE_DEFAULT_ADJUST) / sizeof(int),
  DOUBLE_WAVE_TRS, sizeof(DOUBLE_WAVE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  DOUBLE_WAVE_GLUE_POINTS, sizeof(DOUBLE_WAVE_GLUE_POINTS) / sizeof(Vertex));

const Vertex ACTION_BUTTON_BLANK_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(0 CALCULATED_VALUE, 2 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_BLANK_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_BLANK_CALC[] =
{
  Calculation(0x2001, PROP_ADJUST_VAL_FIRST, 21599, 21600), Calculation(0xa000, PROP_GEO_RIGHT, 0, 0x400), Calculation(0xa000, PROP_GEO_BOTTOM, 0, 0x400)
};

const TextRectangle ACTION_BUTTON_BLANK_TRS[] =
{
  TextRectangle(Vertex(0 CALCULATED_VALUE, 0 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE))
};

const int ACTION_BUTTON_BLANK_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_BLANK(
  ACTION_BUTTON_BLANK_VERTICES, sizeof(ACTION_BUTTON_BLANK_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_BLANK_SEGMENTS, sizeof(ACTION_BUTTON_BLANK_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_BLANK_CALC, sizeof(ACTION_BUTTON_BLANK_CALC) / sizeof(Calculation),
  ACTION_BUTTON_BLANK_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_BLANK_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_BLANK_TRS, sizeof(ACTION_BUTTON_BLANK_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_HOME_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0xa CALCULATED_VALUE), Vertex(0xc CALCULATED_VALUE, 0xe CALCULATED_VALUE), Vertex(0xc CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0xc CALCULATED_VALUE, 0xe CALCULATED_VALUE), Vertex(0xc CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x1a CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_HOME_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x000a, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0007, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_HOME_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8000, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0408, 0), Calculation(0x4001, 2960, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0407, 0), Calculation(0x4001, -5000, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0408, 0), Calculation(0x4001, -7000, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, 5000, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0407, 0), Calculation(0x4001, -2960, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0408, 0), Calculation(0x4001, 8000, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0407, 0), Calculation(0x4001, 6100, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0407, 0), Calculation(0x4001, 8260, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0408, 0), Calculation(0x4001, -6100, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0407, 0), Calculation(0x4001, -8000, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0407, 0), Calculation(0x4001, -1060, 0x0406, 1), Calculation(0x6000, 0x041f, 0x0407, 0), Calculation(0x4001, 1060, 0x0406, 1), Calculation(0x6000, 0x0421, 0x0407, 0), Calculation(0x4001, 4020, 0x0406, 1), Calculation(0x6000, 0x0423, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_HOME_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_HOME_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_HOME(
  ACTION_BUTTON_HOME_VERTICES, sizeof(ACTION_BUTTON_HOME_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_HOME_SEGMENTS, sizeof(ACTION_BUTTON_HOME_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_HOME_CALC, sizeof(ACTION_BUTTON_HOME_CALC) / sizeof(Calculation),
  ACTION_BUTTON_HOME_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_HOME_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_HOME_TRS, sizeof(ACTION_BUTTON_HOME_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_HELP_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x3e CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x3e CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x28 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2c CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2e CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x2e CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x30 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x30 CALCULATED_VALUE, 0x32 CALCULATED_VALUE), Vertex(0x34 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x36 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x32 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x38 CALCULATED_VALUE), Vertex(0x3a CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x3c CALCULATED_VALUE), Vertex(0x34 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x34 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x34 CALCULATED_VALUE, 0x14 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_HELP_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0xa704, 0x6000, 0x8000, 0x4000, 0x0001, 0x2004, 0x0001, 0x2004, 0x0001, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_HELP_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -1690, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, 4600, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 1690, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 7980, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, 1270, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0407, 0), Calculation(0x4001, 4000, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0408, 0), Calculation(0x4001, 1750, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0408, 0), Calculation(0x4001, 800, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0408, 0), Calculation(0x4001, 1650, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0407, 0), Calculation(0x4001, 2340, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0407, 0), Calculation(0x4001, 3640, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0407, 0), Calculation(0x4001, 4670, 0x0406, 1), Calculation(0x6000, 0x041f, 0x0407, 0), Calculation(0x4001, -1570, 0x0406, 1), Calculation(0x6000, 0x0421, 0x0408, 0), Calculation(0x4001, -3390, 0x0406, 1), Calculation(0x6000, 0x0423, 0x0408, 0), Calculation(0x4001, -6050, 0x0406, 1), Calculation(0x6000, 0x0425, 0x0408, 0), Calculation(0x4001, 2540, 0x0406, 1), Calculation(0x6000, 0x0427, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0429, 0x0408, 0), Calculation(0x4001, -2540, 0x0406, 1), Calculation(0x6000, 0x042b, 0x0407, 0), Calculation(0x4001, -4460, 0x0406, 1), Calculation(0x6000, 0x042d, 0x0407, 0), Calculation(0x4001, -2330, 0x0406, 1), Calculation(0x6000, 0x042f, 0x0407, 0), Calculation(0x4001, -4700, 0x0406, 1), Calculation(0x6000, 0x0431, 0x0408, 0), Calculation(0x4001, -1270, 0x0406, 1), Calculation(0x6000, 0x0433, 0x0407, 0), Calculation(0x4001, -5720, 0x0406, 1), Calculation(0x6000, 0x0435, 0x0408, 0), Calculation(0x4001, -2540, 0x0406, 1), Calculation(0x6000, 0x0437, 0x0408, 0), Calculation(0x4001, 1800, 0x0406, 1), Calculation(0x6000, 0x0439, 0x0407, 0), Calculation(0x4001, -1700, 0x0406, 1), Calculation(0x6000, 0x043b, 0x0408, 0), Calculation(0x4001, 6290, 0x0406, 1), Calculation(0x6000, 0x043d, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_HELP_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_HELP_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_HELP(
  ACTION_BUTTON_HELP_VERTICES, sizeof(ACTION_BUTTON_HELP_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_HELP_SEGMENTS, sizeof(ACTION_BUTTON_HELP_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_HELP_CALC, sizeof(ACTION_BUTTON_HELP_CALC) / sizeof(Calculation),
  ACTION_BUTTON_HELP_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_HELP_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_HELP_TRS, sizeof(ACTION_BUTTON_HELP_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_INFORMATION_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x8 CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x7 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x26 CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x26 CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x28 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_INFORMATION_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0xa704, 0x6000, 0x8000, 0x4000, 0xa704, 0x6000, 0x8000, 0x4000, 0x0009, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_INFORMATION_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, -2060, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0407, 0), Calculation(0x4001, -7620, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0408, 0), Calculation(0x4001, 2060, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0407, 0), Calculation(0x4001, -3500, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0408, 0), Calculation(0x4001, -2960, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0407, 0), Calculation(0x4001, -2960, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0408, 0), Calculation(0x4001, 1480, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0407, 0), Calculation(0x4001, 5080, 0x0406, 1), Calculation(0x6000, 0x041f, 0x0408, 0), Calculation(0x4001, 2960, 0x0406, 1), Calculation(0x6000, 0x0421, 0x0407, 0), Calculation(0x4001, 6140, 0x0406, 1), Calculation(0x6000, 0x0423, 0x0408, 0), Calculation(0x4001, -1480, 0x0406, 1), Calculation(0x6000, 0x0425, 0x0407, 0), Calculation(0x4001, -1920, 0x0406, 1), Calculation(0x6000, 0x0427, 0x0408, 0), Calculation(0x4001, -5560, 0x0406, 1), Calculation(0x6000, 0x0429, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_INFORMATION_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_INFORMATION_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_INFORMATION(
  ACTION_BUTTON_INFORMATION_VERTICES, sizeof(ACTION_BUTTON_INFORMATION_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_INFORMATION_SEGMENTS, sizeof(ACTION_BUTTON_INFORMATION_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_INFORMATION_CALC, sizeof(ACTION_BUTTON_INFORMATION_CALC) / sizeof(Calculation),
  ACTION_BUTTON_INFORMATION_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_INFORMATION_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_INFORMATION_TRS, sizeof(ACTION_BUTTON_INFORMATION_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_FORWARD_NEXT_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x10 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_FORWARD_NEXT_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_FORWARD_NEXT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_FORWARD_NEXT_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_FORWARD_NEXT_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_FORWARD_NEXT(
  ACTION_BUTTON_FORWARD_NEXT_VERTICES, sizeof(ACTION_BUTTON_FORWARD_NEXT_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_FORWARD_NEXT_SEGMENTS, sizeof(ACTION_BUTTON_FORWARD_NEXT_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_FORWARD_NEXT_CALC, sizeof(ACTION_BUTTON_FORWARD_NEXT_CALC) / sizeof(Calculation),
  ACTION_BUTTON_FORWARD_NEXT_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_FORWARD_NEXT_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_FORWARD_NEXT_TRS, sizeof(ACTION_BUTTON_FORWARD_NEXT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_BACK_PREVIOUS_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x10 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_BACK_PREVIOUS_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_BACK_PREVIOUS_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_BACK_PREVIOUS_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_BACK_PREVIOUS_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_BACK_PREVIOUS(
  ACTION_BUTTON_BACK_PREVIOUS_VERTICES, sizeof(ACTION_BUTTON_BACK_PREVIOUS_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_BACK_PREVIOUS_SEGMENTS, sizeof(ACTION_BUTTON_BACK_PREVIOUS_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_BACK_PREVIOUS_CALC, sizeof(ACTION_BUTTON_BACK_PREVIOUS_CALC) / sizeof(Calculation),
  ACTION_BUTTON_BACK_PREVIOUS_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_BACK_PREVIOUS_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_BACK_PREVIOUS_TRS, sizeof(ACTION_BUTTON_BACK_PREVIOUS_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_END_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_END_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_END_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -4020, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0407, 0), Calculation(0x4001, -6140, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0407, 0), Calculation(0x4001, 4020, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0407, 0), Calculation(0x4001, 6140, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0407, 0)
};

const TextRectangle ACTION_BUTTON_END_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_END_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_END(
  ACTION_BUTTON_END_VERTICES, sizeof(ACTION_BUTTON_END_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_END_SEGMENTS, sizeof(ACTION_BUTTON_END_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_END_CALC, sizeof(ACTION_BUTTON_END_CALC) / sizeof(Calculation),
  ACTION_BUTTON_END_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_END_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_END_TRS, sizeof(ACTION_BUTTON_END_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_BEGINNING_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x12 CALCULATED_VALUE, 0x10 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_BEGINNING_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_BEGINNING_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -4020, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0407, 0), Calculation(0x4001, -6140, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0407, 0), Calculation(0x4001, 4020, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0407, 0), Calculation(0x4001, 6140, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0407, 0)
};

const TextRectangle ACTION_BUTTON_BEGINNING_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_BEGINNING_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_BEGINNING(
  ACTION_BUTTON_BEGINNING_VERTICES, sizeof(ACTION_BUTTON_BEGINNING_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_BEGINNING_SEGMENTS, sizeof(ACTION_BUTTON_BEGINNING_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_BEGINNING_CALC, sizeof(ACTION_BUTTON_BEGINNING_CALC) / sizeof(Calculation),
  ACTION_BUTTON_BEGINNING_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_BEGINNING_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_BEGINNING_TRS, sizeof(ACTION_BUTTON_BEGINNING_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_RETURN_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x20 CALCULATED_VALUE), Vertex(0x22 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x24 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x24 CALCULATED_VALUE, 0x10 CALCULATED_VALUE), Vertex(0x24 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x28 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(7 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0x2c CALCULATED_VALUE, 0x2a CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x10 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_RETURN_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0002, 0x2001, 0x0001, 0x2001, 0x0006, 0x2001, 0x0001, 0x2001, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_RETURN_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -3800, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, -4020, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 2330, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0408, 0), Calculation(0x4001, 3390, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0408, 0), Calculation(0x4001, -3100, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0407, 0), Calculation(0x4001, 4230, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0408, 0), Calculation(0x4001, -1910, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0407, 0), Calculation(0x4001, 1190, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0407, 0), Calculation(0x4001, 2110, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0407, 0), Calculation(0x4001, 4030, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0407, 0), Calculation(0x4001, -7830, 0x0406, 1), Calculation(0x6000, 0x041f, 0x0408, 0), Calculation(0x4001, 8250, 0x0406, 1), Calculation(0x6000, 0x0421, 0x0407, 0), Calculation(0x4001, 6140, 0x0406, 1), Calculation(0x6000, 0x0423, 0x0407, 0), Calculation(0x4001, 5510, 0x0406, 1), Calculation(0x6000, 0x0425, 0x0408, 0), Calculation(0x4001, 3180, 0x0406, 1), Calculation(0x6000, 0x0427, 0x0407, 0), Calculation(0x4001, 8450, 0x0406, 1), Calculation(0x6000, 0x0429, 0x0408, 0), Calculation(0x4001, -5090, 0x0406, 1), Calculation(0x6000, 0x042b, 0x0407, 0)
};

const TextRectangle ACTION_BUTTON_RETURN_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_RETURN_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_RETURN(
  ACTION_BUTTON_RETURN_VERTICES, sizeof(ACTION_BUTTON_RETURN_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_RETURN_SEGMENTS, sizeof(ACTION_BUTTON_RETURN_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_RETURN_CALC, sizeof(ACTION_BUTTON_RETURN_CALC) / sizeof(Calculation),
  ACTION_BUTTON_RETURN_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_RETURN_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_RETURN_TRS, sizeof(ACTION_BUTTON_RETURN_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_DOCUMENT_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x12 CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_DOCUMENT_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0004, 0x6001, 0x8000, 0x4000, 0x0002, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_DOCUMENT_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -6350, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -7830, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, 1690, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 6350, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0407, 0), Calculation(0x4001, -3810, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0408, 0), Calculation(0x4001, 7830, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_DOCUMENT_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_DOCUMENT_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_DOCUMENT(
  ACTION_BUTTON_DOCUMENT_VERTICES, sizeof(ACTION_BUTTON_DOCUMENT_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_DOCUMENT_SEGMENTS, sizeof(ACTION_BUTTON_DOCUMENT_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_DOCUMENT_CALC, sizeof(ACTION_BUTTON_DOCUMENT_CALC) / sizeof(Calculation),
  ACTION_BUTTON_DOCUMENT_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_DOCUMENT_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_DOCUMENT_TRS, sizeof(ACTION_BUTTON_DOCUMENT_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_SOUND_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x14 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 8 CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x1c CALCULATED_VALUE), Vertex(0x18 CALCULATED_VALUE, 0x16 CALCULATED_VALUE), Vertex(0x1a CALCULATED_VALUE, 0x1e CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_SOUND_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0005, 0x6001, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000, 0x4000, 0x0001, 0x8000
};

const Calculation ACTION_BUTTON_SOUND_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -2750, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, -2960, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, 2120, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0407, 0), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0408, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0408, 0), Calculation(0x4001, 2750, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0408, 0), Calculation(0x4001, 4020, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0407, 0), Calculation(0x4001, -5930, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0408, 0), Calculation(0x4001, 5930, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_SOUND_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_SOUND_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_SOUND(
  ACTION_BUTTON_SOUND_VERTICES, sizeof(ACTION_BUTTON_SOUND_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_SOUND_SEGMENTS, sizeof(ACTION_BUTTON_SOUND_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_SOUND_CALC, sizeof(ACTION_BUTTON_SOUND_CALC) / sizeof(Calculation),
  ACTION_BUTTON_SOUND_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_SOUND_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_SOUND_TRS, sizeof(ACTION_BUTTON_SOUND_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

const Vertex ACTION_BUTTON_MOVIE_VERTICES[] =
{
  Vertex(0, 0), Vertex(21600, 0), Vertex(21600, 21600), Vertex(0, 21600), Vertex(0, 0), Vertex(21600, 0), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 0), Vertex(21600, 21600), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(21600, 21600), Vertex(0, 21600), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0, 21600), Vertex(0, 0), Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(1 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0xc CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x14 CALCULATED_VALUE, 0x12 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x1a CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x18 CALCULATED_VALUE), Vertex(0x20 CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x1e CALCULATED_VALUE, 0x22 CALCULATED_VALUE), Vertex(0x1c CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x24 CALCULATED_VALUE), Vertex(0x16 CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x2a CALCULATED_VALUE, 0x26 CALCULATED_VALUE), Vertex(0x2a CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0x10 CALCULATED_VALUE, 0x28 CALCULATED_VALUE), Vertex(0xe CALCULATED_VALUE, 0x2c CALCULATED_VALUE), Vertex(0xa CALCULATED_VALUE, 0x2c CALCULATED_VALUE)
};

const unsigned short ACTION_BUTTON_MOVIE_SEGMENTS[] =
{
  0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0003, 0x6001, 0x8000, 0x4000, 0x0012, 0x6001, 0x8000
};

const Calculation ACTION_BUTTON_MOVIE_CALC[] =
{
  Calculation(0x2000, PROP_ADJUST_VAL_FIRST, 0, 0), Calculation(0x6000, PROP_GEO_LEFT, PROP_ADJUST_VAL_FIRST, 0), Calculation(0x6000, PROP_GEO_TOP, PROP_ADJUST_VAL_FIRST, 0), Calculation(0xa000, PROP_GEO_RIGHT, 0, PROP_ADJUST_VAL_FIRST), Calculation(0xa000, PROP_GEO_BOTTOM, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x8000, 10800, 0, PROP_ADJUST_VAL_FIRST), Calculation(0x2001, 0x0405, 1, 10800), Calculation(0x2001, PROP_GEO_RIGHT, 1, 2), Calculation(0x2001, PROP_GEO_BOTTOM, 1, 2), Calculation(0x4001, -8050, 0x0406, 1), Calculation(0x6000, 0x0409, 0x0407, 0), Calculation(0x4001, -4020, 0x0406, 1), Calculation(0x6000, 0x040b, 0x0408, 0), Calculation(0x4001, -7000, 0x0406, 1), Calculation(0x6000, 0x040d, 0x0407, 0), Calculation(0x4001, -6560, 0x0406, 1), Calculation(0x6000, 0x040f, 0x0407, 0), Calculation(0x4001, -3600, 0x0406, 1), Calculation(0x6000, 0x0411, 0x0408, 0), Calculation(0x4001, 4020, 0x0406, 1), Calculation(0x6000, 0x0413, 0x0407, 0), Calculation(0x4001, 4660, 0x0406, 1), Calculation(0x6000, 0x0415, 0x0407, 0), Calculation(0x4001, -2960, 0x0406, 1), Calculation(0x6000, 0x0417, 0x0408, 0), Calculation(0x4001, -2330, 0x0406, 1), Calculation(0x6000, 0x0419, 0x0408, 0), Calculation(0x4001, 6780, 0x0406, 1), Calculation(0x6000, 0x041b, 0x0407, 0), Calculation(0x4001, 7200, 0x0406, 1), Calculation(0x6000, 0x041d, 0x0407, 0), Calculation(0x4001, 8050, 0x0406, 1), Calculation(0x6000, 0x041f, 0x0407, 0), Calculation(0x4001, 2960, 0x0406, 1), Calculation(0x6000, 0x0421, 0x0408, 0), Calculation(0x4001, 2330, 0x0406, 1), Calculation(0x6000, 0x0423, 0x0408, 0), Calculation(0x4001, 3800, 0x0406, 1), Calculation(0x6000, 0x0425, 0x0408, 0), Calculation(0x4001, -1060, 0x0406, 1), Calculation(0x6000, 0x0427, 0x0408, 0), Calculation(0x4001, -6350, 0x0406, 1), Calculation(0x6000, 0x0429, 0x0407, 0), Calculation(0x4001, -640, 0x0406, 1), Calculation(0x6000, 0x042b, 0x0408, 0)
};

const TextRectangle ACTION_BUTTON_MOVIE_TRS[] =
{
  TextRectangle(Vertex(1 CALCULATED_VALUE, 2 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 4 CALCULATED_VALUE))
};

const int ACTION_BUTTON_MOVIE_DEFAULT_ADJUST[] =
{
  1400
};

const CustomShape CS_ACTION_BUTTON_MOVIE(
  ACTION_BUTTON_MOVIE_VERTICES, sizeof(ACTION_BUTTON_MOVIE_VERTICES) / sizeof(Vertex),
  ACTION_BUTTON_MOVIE_SEGMENTS, sizeof(ACTION_BUTTON_MOVIE_SEGMENTS) / sizeof(unsigned short),
  ACTION_BUTTON_MOVIE_CALC, sizeof(ACTION_BUTTON_MOVIE_CALC) / sizeof(Calculation),
  ACTION_BUTTON_MOVIE_DEFAULT_ADJUST, sizeof(ACTION_BUTTON_MOVIE_DEFAULT_ADJUST) / sizeof(int),
  ACTION_BUTTON_MOVIE_TRS, sizeof(ACTION_BUTTON_MOVIE_TRS) / sizeof(TextRectangle),
  21600, 21600,
  NULL, 0);

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
  case ARC_SHAPE:
    return &CS_ARC;
  case PLAQUE:
    return &CS_PLAQUE;
  case CAN:
    return &CS_CAN;
  case DONUT:
    return &CS_DONUT;
  case TEXT_BOX:
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
  case LINE:
    return &CS_LINE;
  case FOLDED_CORNER:
    return &CS_FOLDED_CORNER;
  case LEFT_ARROW:
    return &CS_LEFT_ARROW;
  case DOWN_ARROW:
    return &CS_DOWN_ARROW;
  case UP_ARROW:
    return &CS_UP_ARROW;
  case LEFT_RIGHT_ARROW:
    return &CS_LEFT_RIGHT_ARROW;
  case UP_DOWN_ARROW:
    return &CS_UP_DOWN_ARROW;
  case IRREGULAR_SEAL_1:
    return &CS_IRREGULAR_SEAL_1;
  case IRREGULAR_SEAL_2:
    return &CS_IRREGULAR_SEAL_2;
  case LIGHTNING_BOLT:
    return &CS_LIGHTNING_BOLT;
  case HEART:
    return &CS_HEART;
  case PICTURE_FRAME:
    return NULL; //FIXME
  case QUAD_ARROW:
    return &CS_QUAD_ARROW;
  case BEVEL:
    return &CS_BEVEL;
  case LEFT_BRACKET:
    return &CS_LEFT_BRACKET;
  case RIGHT_BRACKET:
    return &CS_RIGHT_BRACKET;
  case LEFT_BRACE:
    return &CS_LEFT_BRACE;
  case RIGHT_BRACE:
    return &CS_RIGHT_BRACE;
  case LEFT_UP_ARROW:
    return &CS_LEFT_UP_ARROW;
  case BENT_UP_ARROW:
    return &CS_BENT_UP_ARROW;
  case BENT_ARROW:
    return &CS_BENT_ARROW;
  case SEAL_24:
    return &CS_SEAL_24;
  case STRIPED_RIGHT_ARROW:
    return &CS_STRIPED_RIGHT_ARROW;
  case NOTCHED_RIGHT_ARROW:
    return &CS_NOTCHED_RIGHT_ARROW;
  case BLOCK_ARC:
    return &CS_BLOCK_ARC;
  case SMILEY_FACE:
    return &CS_SMILEY_FACE;
  case VERTICAL_SCROLL:
    return &CS_VERTICAL_SCROLL;
  case HORIZONTAL_SCROLL:
    return &CS_HORIZONTAL_SCROLL;
  case CIRCULAR_ARROW:
    return &CS_CIRCULAR_ARROW;
  case U_TURN_ARROW:
    return &CS_U_TURN_ARROW;
  case CURVED_RIGHT_ARROW:
    return &CS_CURVED_RIGHT_ARROW;
  case CURVED_LEFT_ARROW:
    return &CS_CURVED_LEFT_ARROW;
  case CURVED_UP_ARROW:
    return &CS_CURVED_UP_ARROW;
  case CURVED_DOWN_ARROW:
    return &CS_CURVED_DOWN_ARROW;
  case FLOW_CHART_PROCESS:
    return &CS_FLOW_CHART_PROCESS;
  case FLOW_CHART_DECISION:
    return &CS_FLOW_CHART_DECISION;
  case FLOW_CHART_IO:
    return &CS_FLOW_CHART_IO;
  case FLOW_CHART_PREDEFINED_PROCESS:
    return &CS_FLOW_CHART_PREDEFINED_PROCESS;
  case FLOW_CHART_INTERNAL_STORAGE:
    return &CS_FLOW_CHART_INTERNAL_STORAGE;
  case FLOW_CHART_DOCUMENT:
    return &CS_FLOW_CHART_DOCUMENT;
  case FLOW_CHART_MULTI_DOCUMENT:
    return &CS_FLOW_CHART_MULTI_DOCUMENT;
  case FLOW_CHART_TERMINATOR:
    return &CS_FLOW_CHART_TERMINATOR;
  case FLOW_CHART_PREPARATION:
    return &CS_FLOW_CHART_PREPARATION;
  case FLOW_CHART_MANUAL_INPUT:
    return &CS_FLOW_CHART_MANUAL_INPUT;
  case FLOW_CHART_MANUAL_OPERATION:
    return &CS_FLOW_CHART_MANUAL_OPERATION;
  case FLOW_CHART_CONNECTOR:
    return &CS_FLOW_CHART_CONNECTOR;
  case FLOW_CHART_PUNCHED_CARD:
    return &CS_FLOW_CHART_PUNCHED_CARD;
  case FLOW_CHART_PUNCHED_TAPE:
    return &CS_FLOW_CHART_PUNCHED_TAPE;
  case FLOW_CHART_SUMMING_JUNCTION:
    return &CS_FLOW_CHART_SUMMING_JUNCTION;
  case FLOW_CHART_OR:
    return &CS_FLOW_CHART_OR;
  case FLOW_CHART_COLLATE:
    return &CS_FLOW_CHART_COLLATE;
  case FLOW_CHART_SORT:
    return &CS_FLOW_CHART_SORT;
  case FLOW_CHART_EXTRACT:
    return &CS_FLOW_CHART_EXTRACT;
  case FLOW_CHART_MERGE:
    return &CS_FLOW_CHART_MERGE;
  case FLOW_CHART_ONLINE_STORAGE:
    return &CS_FLOW_CHART_ONLINE_STORAGE;
  case FLOW_CHART_MAGNETIC_TAPE:
    return &CS_FLOW_CHART_MAGNETIC_TAPE;
  case FLOW_CHART_MAGNETIC_DISK:
    return &CS_FLOW_CHART_MAGNETIC_DISK;
  case FLOW_CHART_MAGNETIC_DRUM:
    return &CS_FLOW_CHART_MAGNETIC_DRUM;
  case FLOW_CHART_DISPLAY:
    return &CS_FLOW_CHART_DISPLAY;
  case FLOW_CHART_DELAY:
    return &CS_FLOW_CHART_DELAY;
  case TEXT_PLAIN_TEXT:
    return &CS_TEXT_PLAIN_TEXT;
  case TEXT_STOP:
    return &CS_TEXT_STOP;
  case TEXT_TRIANGLE:
    return &CS_TEXT_TRIANGLE;
  case TEXT_TRIANGLE_INVERTED:
    return &CS_TEXT_TRIANGLE_INVERTED;
  case TEXT_CHEVRON:
    return &CS_TEXT_CHEVRON;
  case TEXT_CHEVRON_INVERTED:
    return &CS_TEXT_CHEVRON_INVERTED;
  case TEXT_RING_OUTSIDE:
    return &CS_TEXT_RING_OUTSIDE;
  case TEXT_ARCH_UP_CURVE:
    return &CS_TEXT_ARCH_UP_CURVE;
  case TEXT_ARCH_DOWN_CURVE:
    return &CS_TEXT_ARCH_DOWN_CURVE;
  case TEXT_CIRCLE_CURVE:
    return &CS_TEXT_CIRCLE_CURVE;
  case TEXT_BUTTON_CURVE:
    return &CS_TEXT_BUTTON_CURVE;
  case TEXT_ARCH_UP_POUR:
    return &CS_TEXT_ARCH_UP_POUR;
  case TEXT_ARCH_DOWN_POUR:
    return &CS_TEXT_ARCH_DOWN_POUR;
  case TEXT_CIRCLE_POUR:
    return &CS_TEXT_CIRCLE_POUR;
  case TEXT_BUTTON_POUR:
    return &CS_TEXT_BUTTON_POUR;
  case TEXT_CURVE_UP:
    return &CS_TEXT_CURVE_UP;
  case TEXT_CURVE_DOWN:
    return &CS_TEXT_CURVE_DOWN;
  case TEXT_CASCADE_UP:
    return &CS_TEXT_CASCADE_UP;
  case TEXT_CASCADE_DOWN:
    return &CS_TEXT_CASCADE_DOWN;
  case TEXT_WAVE_1:
    return &CS_TEXT_WAVE_1;
  case TEXT_WAVE_2:
    return &CS_TEXT_WAVE_2;
  case TEXT_WAVE_3:
    return &CS_TEXT_WAVE_3;
  case TEXT_WAVE_4:
    return &CS_TEXT_WAVE_4;
  case TEXT_INFLATE:
    return &CS_TEXT_INFLATE;
  case TEXT_DEFLATE:
    return &CS_TEXT_DEFLATE;
  case TEXT_INFLATE_BOTTOM:
    return &CS_TEXT_INFLATE_BOTTOM;
  case TEXT_DEFLATE_BOTTOM:
    return &CS_TEXT_DEFLATE_BOTTOM;
  case TEXT_INFLATE_TOP:
    return &CS_TEXT_INFLATE_TOP;
  case TEXT_DEFLATE_TOP:
    return &CS_TEXT_DEFLATE_TOP;
  case TEXT_DEFLATE_INFLATE:
    return &CS_TEXT_DEFLATE_INFLATE;
  case TEXT_DEFLATE_INFLATE_DEFLATE: // great name
    return &CS_TEXT_DEFLATE_INFLATE_DEFLATE;
  case TEXT_FADE_RIGHT:
    return &CS_TEXT_FADE_RIGHT;
  case TEXT_FADE_LEFT:
    return &CS_TEXT_FADE_LEFT;
  case TEXT_FADE_UP:
    return &CS_TEXT_FADE_UP;
  case TEXT_FADE_DOWN:
    return &CS_TEXT_FADE_DOWN;
  case TEXT_SLANT_UP:
    return &CS_TEXT_SLANT_UP;
  case TEXT_SLANT_DOWN:
    return &CS_TEXT_SLANT_DOWN;
  case TEXT_CAN_UP:
    return &CS_TEXT_CAN_UP;
  case TEXT_CAN_DOWN:
    return &CS_TEXT_CAN_DOWN;
  case FLOW_CHART_ALTERNATE_PROCESS:
    return &CS_FLOW_CHART_ALTERNATE_PROCESS;
  case FLOW_CHART_OFFPAGE_CONNECTOR:
    return &CS_FLOW_CHART_OFFPAGE_CONNECTOR;
  case LEFT_RIGHT_UP_ARROW:
    return &CS_LEFT_RIGHT_UP_ARROW;
  case SUN:
    return &CS_SUN;
  case MOON:
    return &CS_MOON;
  case BRACKET_PAIR:
    return &CS_BRACKET_PAIR;
  case BRACE_PAIR:
    return &CS_BRACE_PAIR;
  case DOUBLE_WAVE:
    return &CS_DOUBLE_WAVE;
  case ACTION_BUTTON_BLANK:
    return &CS_ACTION_BUTTON_BLANK;
  case ACTION_BUTTON_HOME:
    return &CS_ACTION_BUTTON_HOME;
  case ACTION_BUTTON_HELP:
    return &CS_ACTION_BUTTON_HELP;
  case ACTION_BUTTON_INFORMATION:
    return &CS_ACTION_BUTTON_INFORMATION;
  case ACTION_BUTTON_FORWARD_NEXT:
    return &CS_ACTION_BUTTON_FORWARD_NEXT;
  case ACTION_BUTTON_BACK_PREVIOUS:
    return &CS_ACTION_BUTTON_BACK_PREVIOUS;
  case ACTION_BUTTON_END:
    return &CS_ACTION_BUTTON_END;
  case ACTION_BUTTON_BEGINNING:
    return &CS_ACTION_BUTTON_BEGINNING;
  case ACTION_BUTTON_RETURN:
    return &CS_ACTION_BUTTON_RETURN;
  case ACTION_BUTTON_DOCUMENT:
    return &CS_ACTION_BUTTON_DOCUMENT;
  case ACTION_BUTTON_SOUND:
    return &CS_ACTION_BUTTON_SOUND;
  case ACTION_BUTTON_MOVIE:
    return &CS_ACTION_BUTTON_MOVIE;
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
  ARCTO,
  ARC,
  CLOCKWISEARCTO,
  CLOCKWISEARC,
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
  case 0xAA:
    cmd = NOFILL;
    break;
  case 0xAB:
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
  case 0xA3:
    cmd = ARCTO;
    count = (binary & 0xFF) / 4;
    break;
  case 0xA4:
    cmd = ARC;
    count = (binary & 0xFF) / 4;
    break;
  case 0xA5:
    cmd = CLOCKWISEARCTO;
    count = (binary & 0xFF) / 4;
    break;
  case 0xA6:
    cmd = CLOCKWISEARC;
    count = (binary & 0xFF) / 4;
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
  case 0x60:
    cmd = CLOSESUBPATH;
    break;
  case 0x80:
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
  double scaleX = width * m_coordWidth;
  double scaleY = height * m_coordHeight;
  if (m_numTextRectangles == 0)
  {
    return Coordinate(x, y, x + width, y + height);
  }
  const Vertex &start = mp_textRectangles[0].first;
  const Vertex &end = mp_textRectangles[0].second;
  double startX = x + scaleX * getSpecialIfNecessary(caller, start.m_x);
  double startY = y + scaleY * getSpecialIfNecessary(caller, start.m_y);
  double endX = x + scaleX * getSpecialIfNecessary(caller, end.m_x);
  double endY = y + scaleY * getSpecialIfNecessary(caller, end.m_y);
  return Coordinate(startX, startY, endX, endY);
}

struct LineInfo
{
  WPXPropertyListVector m_vertices;
  double m_width;
  WPXString m_color;
  bool m_lineExists;
  LineInfo(WPXPropertyListVector vertices, Line current, std::vector<Color> palette) : m_vertices(vertices),
    m_width((double)(current.m_widthInEmu) / EMUS_IN_INCH),
    m_color(libmspub::MSPUBCollector::getColorString(current.m_color.getFinalColor(palette))),
    m_lineExists(current.m_lineExists) { }
  void output(libwpg::WPGPaintInterface *painter, WPXPropertyList &graphicsProps)
  {
    graphicsProps.insert("draw:stroke", m_lineExists ? "solid" : "none");
    graphicsProps.insert("svg:stroke-width", m_width);
    graphicsProps.insert("svg:stroke-color", m_color);
    painter->setStyle(graphicsProps, WPXPropertyListVector());
    painter->drawPolyline(m_vertices);
  }
private:
};

void libmspub::writeCustomShape(ShapeType shapeType, WPXPropertyList &graphicsProps, libwpg::WPGPaintInterface *painter, double x, double y, double height, double width, const libmspub::GeometricShape *caller, bool closeEverything, short clockwiseRotation, bool flipVertical, bool flipHorizontal, std::vector<Line> lines)
{
  const CustomShape *shape = getCustomShape(shapeType);
  if (!shape)
  {
    return;
  }
  bool drawStroke = !lines.empty();
  bool horizontal = height == 0;
  bool vertical = width == 0;
  if (horizontal && vertical)
  {
    return;
  }
  double centerX = x + width / 2;
  double centerY = y + height / 2;
  clockwiseRotation = correctModulo(clockwiseRotation, 360);
  if ( (clockwiseRotation >= 45 && clockwiseRotation < 135) || (clockwiseRotation >= 225 && clockwiseRotation < 315) )
  {
    //MS PUB format gives start and end vertices for the bounding rectangle rotated by 90 degrees in this case.
    rotateCounter(x, y, centerX, centerY, 90);
    double temp = height;
    height = width;
    width = temp;
    y -= height;
  }
  double scaleX = width / shape->m_coordWidth;
  double scaleY = height / shape->m_coordHeight;;
  if (shape->mp_elements == NULL)
  {
    if ((!graphicsProps["draw:fill"]) || (graphicsProps["draw:fill"]->getStr() == "none"))
    {
      std::vector<LineInfo> lineInfos;
      std::vector<Line>::const_iterator iter_line = lines.begin();
      bool rectangle = isShapeTypeRectangle(shapeType) && !lines.empty(); // ugly HACK: special handling for rectangle outlines.
      double vertexX, vertexY;
      double oldX, oldY; //before transformations like rotation and flip
      for (unsigned i = 0; i < shape->m_numVertices; ++i)
      {
        WPXPropertyListVector vertices;
        WPXPropertyList vertex;
        if (i > 0)
        {
          WPXPropertyList vertexStart;
          double lineWidth = (double)(iter_line->m_widthInEmu) / EMUS_IN_INCH;
          switch (i - 1) // fudge the lines inward by half their width so they are fully inside the shape and hence proper borders
          {
          case 0:
            oldY += lineWidth / 2;
            break;
          case 1:
            oldX -= lineWidth / 2;
            break;
          case 2:
            oldY -= lineWidth / 2;
            break;
          case 3:
            oldX += lineWidth / 2;
            break;
          }
          rotateCounter(oldX, oldY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(oldX, oldY, centerX, centerY, flipVertical, flipHorizontal);
          vertexStart.insert("svg:x", oldX);
          vertexStart.insert("svg:y", oldY);
          vertices.append(vertexStart);
        }
        vertexX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[i].m_x);
        vertexY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[i].m_y);
        oldX = vertexX;
        oldY = vertexY;
        if (rectangle)
        {
          double lineWidth = (double)(iter_line->m_widthInEmu) / EMUS_IN_INCH;
          switch (i) // fudge the lines inward by half their width so they are fully inside the shape and hence proper borders
          {
          case 1:
            vertexY += lineWidth / 2;
            break;
          case 2:
            vertexX -= lineWidth / 2;
            break;
          case 3:
            vertexY -= lineWidth / 2;
            break;
          case 4:
            vertexX += lineWidth / 2;
            break;
          }
        }
        rotateCounter(vertexX, vertexY, centerX, centerY, -clockwiseRotation);
        flipIfNecessary(vertexX, vertexY, centerX, centerY, flipVertical, flipHorizontal);
        vertex.insert("svg:x", vertexX);
        vertex.insert("svg:y", vertexY);
        vertices.append(vertex);
        if (i > 0)
        {
          lineInfos.push_back(LineInfo(vertices, *iter_line, caller->getPaletteColors()));
          if (drawStroke)
          {
            if (iter_line + 1 < lines.end()) // continue using the last element if we run out of lines.
            {
              ++iter_line;
            }
          }
        }
      }
      if (rectangle)
      {
        LineInfo *top = &lineInfos[0];
        LineInfo *right = (lineInfos.size() > 1) ? &lineInfos[1] : NULL;
        LineInfo *bottom = (lineInfos.size() > 2) ? &lineInfos[2] : NULL;
        LineInfo *left = (lineInfos.size() > 3) ? &lineInfos[3] : NULL;
        if(top)
        {
          top->output(painter, graphicsProps);
        }
        if (bottom)
        {
          bottom->output(painter, graphicsProps);
        }
        if (left)
        {
          left->output(painter, graphicsProps);
        }
        if (right)
        {
          right->output(painter, graphicsProps);
        }
      }
      else
      {
        for (unsigned i = 0; i < lineInfos.size(); ++i)
        {
          lineInfos[i].output(painter, graphicsProps);
        }
      }
    }
    else
    {
      WPXPropertyListVector vertices;
      for (unsigned i = 0; i < shape->m_numVertices; ++i)
      {
        WPXPropertyList vertex;
        double vertexX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[i].m_x);
        double vertexY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[i].m_y);
        rotateCounter(vertexX, vertexY, centerX, centerY, -clockwiseRotation);
        flipIfNecessary(vertexX, vertexY, centerX, centerY, flipVertical, flipHorizontal);
        vertex.insert("svg:x", vertexX);
        vertex.insert("svg:y", vertexY);
        vertices.append(vertex);
      }
      painter->drawPolygon(vertices);
    }
  }
  else
  {
    WPXPropertyListVector vertices;
    if (drawStroke)
    {
      // don't bother with different strokes for things defined by segments
      Line &first = lines[0];
      graphicsProps.insert("draw:stroke", first.m_lineExists ? "solid": "none");
      graphicsProps.insert("svg:stroke-width", (double)(first.m_widthInEmu) / EMUS_IN_INCH);
      graphicsProps.insert("svg:stroke-color", libmspub::MSPUBCollector::getColorString(first.m_color.getFinalColor(caller->getPaletteColors())));
      painter->setStyle(graphicsProps, WPXPropertyListVector());
    }
    unsigned vertexIndex = 0;
    bool hasUnclosedElements = false;
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
          double currX = x + scaleX * getSpecialIfNecessary(caller, curr.m_x);
          double currY = y + scaleY * getSpecialIfNecessary(caller, curr.m_y);
          if (vertexIndex)
          {
            hasUnclosedElements = true;
            const Vertex &prev = shape->mp_vertices[vertexIndex - 1];
            double prevX = x + scaleX * getSpecialIfNecessary(caller, prev.m_x);
            double prevY = y + scaleY * getSpecialIfNecessary(caller, prev.m_y);
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
            rotateCounter(x1, y1, centerX, centerY, -clockwiseRotation);
            flipIfNecessary(x1, y1, centerX, centerY, flipVertical, flipHorizontal);
            rotateCounter(x2, y2, centerX, centerY, -clockwiseRotation);
            flipIfNecessary(x2, y2, centerX, centerY, flipVertical, flipHorizontal);
            rotateCounter(currX, currY, centerX, centerY, -clockwiseRotation);
            flipIfNecessary(currX, currY, centerX, centerY, flipVertical, flipHorizontal);
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
              if (hasUnclosedElements && closeEverything)
              {
                WPXPropertyList closeVertex;
                closeVertex.insert("libwpg:path-action", "Z");
                vertices.append(closeVertex);
              }
              hasUnclosedElements = false;
              double newX = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
              double newY = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
              rotateCounter(newX, newY, centerX, centerY, -clockwiseRotation);
              flipIfNecessary(newX, newY, centerX, centerY, flipVertical, flipHorizontal);
              moveVertex.insert("svg:x", scaleX * newX + x);
              moveVertex.insert("svg:y", scaleY * newY + y);
              moveVertex.insert("libwpg:path-action", "M");
              vertices.append(moveVertex);
              ++vertexIndex;
            }
          }
        }
      }
      break;
      case CLOCKWISEARCTO:
      case CLOCKWISEARC:
      case ARCTO:
      case ARC:
      {
        bool move = (cmd.m_command == CLOCKWISEARCTO || cmd.m_command == ARCTO);
        bool clockwise = (cmd.m_command == CLOCKWISEARCTO || cmd.m_command == CLOCKWISEARC);
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex + 3 < shape->m_numVertices); ++j, vertexIndex += 4)
        {
          hasUnclosedElements = true;
          unsigned startIndex = vertexIndex + 2;//(clockwise ? 3 : 2);
          unsigned endIndex = vertexIndex + 3;//(clockwise ? 2 : 3);
          double left = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double top = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          double right = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_x);
          double bottom = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_y);
          double startX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[startIndex].m_x);
          double startY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[startIndex].m_y);
          double endX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[endIndex].m_x);
          double endY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[endIndex].m_y);
          WPXPropertyList moveVertex;
          rotateCounter(startX, startY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(startX, startY, centerX, centerY, flipVertical, flipHorizontal);
          rotateCounter(endX, endY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(endX, endY, centerX, centerY, flipVertical, flipHorizontal);
          moveVertex.insert("libwpg:path-action", move ? "L" : "M");
          moveVertex.insert("svg:x", startX);
          moveVertex.insert("svg:y", startY);
          vertices.append(moveVertex);
          WPXPropertyList endVertex;
          endVertex.insert("libwpg:path-action", "A");
          double startVecX = startX - (right + left) / 2;
          double startVecY = (top + bottom) / 2 - startY;
          double endVecX = endX - (right + left) / 2;
          double endVecY = (top + bottom ) / 2 - endY;
          double startAngle = atan2(startVecY, startVecX);
          bool large = (endVecX * sin(startAngle) <= endVecY * cos(startAngle)) ? clockwise : !clockwise; //let $ be the angle of Start. Then this checks the sign of the y component of End rotated by -$. If the sign is negative and we are going counterclockwise, this is a short arc. Do the opposite for clockwise.
          bool flipped = flipVertical ^ flipHorizontal;
          if (flipped)
          {
            clockwiseRotation = -clockwiseRotation;
            clockwise = !clockwise;
          }
          endVertex.insert("libwpg:large-arc", large ? 1 : 0);
          endVertex.insert("libwpg:sweep", clockwise ? 1 : 0);
          endVertex.insert("svg:x", endX);
          endVertex.insert("svg:y", endY);
          endVertex.insert("libwpg:rotate", clockwiseRotation);
          endVertex.insert("svg:rx", (right - left) / 2);
          endVertex.insert("svg:ry", (bottom - top) / 2);
          vertices.append(endVertex);
        }
      }
      break;

      case ANGLEELLIPSE:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex + 2 < shape->m_numVertices); ++j, vertexIndex += 3)
        {
          hasUnclosedElements = true;
          WPXPropertyList vertex;
          double startAngle = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_x);
          double endAngle = getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_y);
          double cx = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double cy = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          double rx = scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_x);
          double ry = scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_y);

          // FIXME: Are angles supposed to be the actual angle of the point with the x-axis,
          // or the eccentric anomaly, or something else?
          //
          // assuming eccentric anomaly for now
          WPXPropertyList moveVertex;
          double startX = cx + rx * cos(startAngle * PI / 180);
          double startY = cy + ry * sin(startAngle * PI / 180);
          rotateCounter(startX, startY, centerX, centerY, -clockwiseRotation);
          moveVertex.insert("libwpg:path-action", "M");
          moveVertex.insert("svg:x", startX);
          moveVertex.insert("svg:y", startY);
          vertices.append(moveVertex);
          double halfX = cx + rx * cos(endAngle * PI / 360);
          double halfY = cy + ry * sin(endAngle * PI / 360);
          rotateCounter(halfX, halfY, centerX, centerY, -clockwiseRotation);
          WPXPropertyList halfVertex;
          halfVertex.insert("libwpg:path-action", "A");
          halfVertex.insert("svg:x", halfX);
          halfVertex.insert("svg:y", halfY);
          halfVertex.insert("svg:rx", rx);
          halfVertex.insert("svg:ry", ry);
          halfVertex.insert("libwpg:rotate", clockwiseRotation);
          vertices.append(halfVertex);
          double endX = cx + rx * cos(endAngle * PI / 180);
          double endY = cy + ry * sin(endAngle * PI / 180);
          rotateCounter(endX, endY, centerX, centerY, -clockwiseRotation);
          vertex.insert("svg:x", endX);
          vertex.insert("svg:y", endY);
          vertex.insert("svg:rx", rx);
          vertex.insert("svg:ry", ry);
          vertex.insert("libwpg:rotate", clockwiseRotation);
          vertex.insert("libwpg:path-action", "A");
          vertices.append(vertex);
        }
        break;
      case MOVETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex < shape->m_numVertices); ++j, ++vertexIndex)
        {
          if (hasUnclosedElements && closeEverything)
          {
            WPXPropertyList closeVertex;
            closeVertex.insert("libwpg:path-action", "Z");
            vertices.append(closeVertex);
          }
          hasUnclosedElements = false;
          WPXPropertyList moveVertex;
          double newX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double newY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          rotateCounter(newX, newY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(newX, newY, centerX, centerY, flipVertical, flipHorizontal);
          moveVertex.insert("svg:x", newX);
          moveVertex.insert("svg:y", newY);
          moveVertex.insert("libwpg:path-action", "M");
          vertices.append(moveVertex);
        }
        break;
      case LINETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex < shape->m_numVertices); ++j, ++vertexIndex)
        {
          hasUnclosedElements = true;
          WPXPropertyList vertex;
          double vertexX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double vertexY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          rotateCounter(vertexX, vertexY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(vertexX, vertexY, centerX, centerY, flipVertical, flipHorizontal);
          vertex.insert("svg:x", vertexX);
          vertex.insert("svg:y", vertexY);
          vertex.insert("libwpg:path-action", "L");
          vertices.append(vertex);
        }
        break;
      case CURVETO:
        for (unsigned j = 0; (j < cmd.m_count) && (vertexIndex + 2 < shape->m_numVertices); ++j, vertexIndex += 3)
        {
          hasUnclosedElements = true;
          double firstCtrlX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_x);
          double firstCtrlY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex].m_y);
          rotateCounter(firstCtrlX, firstCtrlY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(firstCtrlX, firstCtrlY, centerX, centerY, flipVertical, flipHorizontal);
          double secondCtrlX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_x);
          double secondCtrlY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 1].m_y);
          rotateCounter(secondCtrlX, secondCtrlY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(secondCtrlX, secondCtrlY, centerX, centerY, flipVertical, flipHorizontal);
          double endX = x + scaleX * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_x);
          double endY = y + scaleY * getSpecialIfNecessary(caller, shape->mp_vertices[vertexIndex + 2].m_y);
          rotateCounter(endX, endY, centerX, centerY, -clockwiseRotation);
          flipIfNecessary(endX, endY, centerX, centerY, flipVertical, flipHorizontal);
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
        break;
      case CLOSESUBPATH:
        // case ENDSUBPATH:
      {
        WPXPropertyList end;
        end.insert("libwpg:path-action", "Z");
        vertices.append(end);
        hasUnclosedElements = false;
      }
      break;
      default:
        break;
      }
    }
    if (hasUnclosedElements && closeEverything)
    {
      WPXPropertyList closeVertex;
      closeVertex.insert("libwpg:path-action", "Z");
      vertices.append(closeVertex);
    }
    painter->drawPath(vertices);
  }
}

bool libmspub::isShapeTypeRectangle(ShapeType type)
{
  return type == RECTANGLE || type == TEXT_BOX;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
