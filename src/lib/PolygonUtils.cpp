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

const unsigned TRAPEZOID_DEFAULT_ADJUST[] =
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
  TRAPEZOID_DEFAULT_ADJUST, sizeof(TRAPEZOID_DEFAULT_ADJUST) / sizeof(unsigned),
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
  Calculation(0x4000, 10800, OTHER_CALC_VAL, 0),
  Calculation(0x8000, 10800, 0, OTHER_CALC_VAL | 2)
};

const TextRectangle SEAL_4_TRS[] =
{
  TextRectangle(Vertex(4 CALCULATED_VALUE, 4 CALCULATED_VALUE), Vertex(3 CALCULATED_VALUE, 3 CALCULATED_VALUE))
};

const unsigned SEAL_4_DEFAULT_ADJUST[] =
{
  8100
};

const CustomShape CS_SEAL_4(
  SEAL_4_VERTICES, sizeof(SEAL_4_VERTICES) / sizeof(Vertex),
  NULL, 0,
  SEAL_4_CALC, sizeof(SEAL_4_CALC) / sizeof(Calculation),
  SEAL_4_DEFAULT_ADJUST, sizeof(SEAL_4_DEFAULT_ADJUST) / sizeof(unsigned),
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

const unsigned ISOCELES_TRIANGLE_DEFAULT_ADJUST[] =
{
  10800
};

const CustomShape CS_ISOCELES_TRIANGLE(
  ISOCELES_TRIANGLE_VERTICES, sizeof(ISOCELES_TRIANGLE_VERTICES) / sizeof(Vertex),
  ISOCELES_TRIANGLE_SEGMENTS, sizeof(ISOCELES_TRIANGLE_SEGMENTS) / sizeof(unsigned short),
  ISOCELES_TRIANGLE_CALC, sizeof(ISOCELES_TRIANGLE_CALC) / sizeof(Calculation),
  ISOCELES_TRIANGLE_DEFAULT_ADJUST, sizeof(ISOCELES_TRIANGLE_DEFAULT_ADJUST) / sizeof(unsigned),
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

const unsigned PARALLELOGRAM_DEFAULT_ADJUST[] =
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
  PARALLELOGRAM_DEFAULT_ADJUST, sizeof(PARALLELOGRAM_DEFAULT_ADJUST) / sizeof(unsigned),
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

const unsigned HEXAGON_DEFAULT_ADJUST[] =
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
  HEXAGON_DEFAULT_ADJUST, sizeof(HEXAGON_DEFAULT_ADJUST) / sizeof(unsigned),
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
  default:
    return NULL;
  }
}

enum Command
{
  MOVETO,
  LINETO,
  ANGLEELLIPSE,
  CLOSESUBPATH
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
  case 0xA2:
    cmd = ANGLEELLIPSE;
    count = (binary & 0xFF) / 3;
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
  default:
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
