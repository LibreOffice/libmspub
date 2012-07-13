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

#ifndef __POLYGONUTILS_H__
#define __POLYGONUTILS_H__

#include <utility> // for std::pair

#include <libwpg/libwpg.h>

#include "MSPUBCollector.h"
#include "Shapes.h"
#include "VectorTransformation2D.h"

namespace libmspub
{
const int PROP_ADJUST_VAL_FIRST = 327;
const int PROP_ADJUST_VAL_LAST  = 336;
const int PROP_GEO_LEFT         = 320;
const int PROP_GEO_TOP          = 321;
const int PROP_GEO_RIGHT        = 322;
const int PROP_GEO_BOTTOM       = 323;

const int OTHER_CALC_VAL        = 0x400;
const int ASPECT_RATIO          = 0x600;

class MSPUBCollector;

typedef struct
{
  int m_x;
  int m_y;
}  Vertex;

typedef struct
{
  int m_flags;
  int m_argOne;
  int m_argTwo;
  int m_argThree;
} Calculation;

typedef struct
{
  Vertex first;
  Vertex second;
} TextRectangle;

struct CustomShape
{
  const Vertex *mp_vertices;
  unsigned m_numVertices;
  const unsigned short *mp_elements;
  unsigned m_numElements;
  const Calculation *mp_calculations;
  unsigned m_numCalculations;
  const int *mp_defaultAdjustValues;
  unsigned m_numDefaultAdjustValues;
  const TextRectangle *mp_textRectangles;
  unsigned m_numTextRectangles;
  unsigned m_coordWidth;
  unsigned m_coordHeight;
  const Vertex *mp_gluePoints;
  unsigned m_numGluePoints;
  unsigned char m_adjustShiftMask;

  Coordinate getTextRectangle(double x, double y, double width, double height, const libmspub::GeometricShape *caller) const;

  CustomShape(const Vertex *p_vertices, unsigned numVertices, const unsigned short *p_elements, unsigned numElements, const Calculation *p_calculations, unsigned numCalculations, const int *p_defaultAdjustValues, unsigned numDefaultAdjustValues, const TextRectangle *p_textRectangles, unsigned numTextRectangles, unsigned coordWidth, unsigned coordHeight, const Vertex *p_gluePoints, unsigned numGluePoints, unsigned char adjustShiftMask = 0) :
    mp_vertices(p_vertices), m_numVertices(numVertices),
    mp_elements(p_elements), m_numElements(numElements),
    mp_calculations(p_calculations), m_numCalculations(numCalculations),
    mp_defaultAdjustValues(p_defaultAdjustValues), m_numDefaultAdjustValues(numDefaultAdjustValues),
    mp_textRectangles(p_textRectangles), m_numTextRectangles(numTextRectangles),
    m_coordWidth(coordWidth), m_coordHeight(coordHeight),
    mp_gluePoints(p_gluePoints), m_numGluePoints(numGluePoints),
    m_adjustShiftMask(adjustShiftMask)
  {
  }
};

const CustomShape *getCustomShape(ShapeType type);
bool isShapeTypeRectangle(ShapeType type);
void writeCustomShape(ShapeType shapeType, WPXPropertyList &graphicsProps, libwpg::WPGPaintInterface *painter, double x, double y, double height, double width, const GeometricShape *caller, bool closeEverything, VectorTransformation2D transform, std::vector<Line> lines);

} // libmspub
#endif /* __POLYGONUTILS_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
