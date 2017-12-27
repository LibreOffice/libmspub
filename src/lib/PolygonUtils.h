/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_POLYGONUTILS_H
#define INCLUDED_POLYGONUTILS_H

#include <functional>
#include <memory>
#include <vector>

#include <librevenge/librevenge.h>

#include "ShapeType.h"
#include "VectorTransformation2D.h"
#include "Coordinate.h"
#include "Line.h"

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

  Coordinate getTextRectangle(double x, double y, double width, double height, std::function<double(unsigned index)> calculator) const;

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

struct DynamicCustomShape
{
  std::vector<Vertex> m_vertices;
  std::vector<unsigned short> m_elements;
  std::vector<Calculation> m_calculations;
  std::vector<int> m_defaultAdjustValues;
  std::vector<TextRectangle> m_textRectangles;
  std::vector<Vertex> m_gluePoints;
  unsigned m_coordWidth;
  unsigned m_coordHeight;
  unsigned char m_adjustShiftMask;

  DynamicCustomShape(unsigned coordWidth, unsigned coordHeight)
    : m_vertices(), m_elements(),
      m_calculations(), m_defaultAdjustValues(),
      m_textRectangles(), m_gluePoints(),
      m_coordWidth(coordWidth), m_coordHeight(coordHeight),
      m_adjustShiftMask(0)
  {
  }
};

std::shared_ptr<const CustomShape> getFromDynamicCustomShape(const DynamicCustomShape &dcs);

const CustomShape *getCustomShape(ShapeType type);
bool isShapeTypeRectangle(ShapeType type);
librevenge::RVNGPropertyList calcClipPath(const std::vector<libmspub::Vertex> &verts, double x, double y, double height, double width, VectorTransformation2D transform, std::shared_ptr<const CustomShape> shape);
void writeCustomShape(ShapeType shapeType, librevenge::RVNGPropertyList &graphicsProps, librevenge::RVNGDrawingInterface *painter, double x, double y, double height, double width, bool closeEverything, VectorTransformation2D transform, std::vector<Line> lines, std::function<double(unsigned index)> calculator, const std::vector<Color> &palette, std::shared_ptr<const CustomShape> shape);

} // libmspub
#endif /* INCLUDED_POLYGONUTILS_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
