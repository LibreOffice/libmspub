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

namespace libmspub
{

struct Vertex
{
  int m_x;
  int m_y;
  Vertex(int x, int y) : m_x(x), m_y(y) { }
};

typedef std::pair<Vertex, Vertex> TextRectangle;

struct CustomShape
{
  const Vertex *mp_vertices;
  unsigned m_numVertices;
  const unsigned short *mp_elements;
  unsigned m_numElements;
  const TextRectangle *mp_textRectangles;
  unsigned m_numTextRectangles;
  unsigned m_coordWidth;
  unsigned m_coordHeight;
  const Vertex *mp_gluePoints;
  unsigned m_numGluePoints;

  CustomShape(const Vertex *p_vertices, unsigned numVertices, const unsigned short *p_elements, unsigned numElements, const TextRectangle *p_textRectangles, unsigned numTextRectangles, unsigned coordWidth, unsigned coordHeight, const Vertex *p_gluePoints, unsigned numGluePoints) :
    mp_vertices(p_vertices), m_numVertices(numVertices),
    mp_elements(p_elements), m_numElements(numElements),
    mp_textRectangles(p_textRectangles), m_numTextRectangles(numTextRectangles),
    m_coordWidth(coordWidth), m_coordHeight(coordHeight),
    mp_gluePoints(p_gluePoints), m_numGluePoints(numGluePoints)
  {
  }
};

const CustomShape *getCustomShape(ShapeType type);
void writeCustomShape(const CustomShape *shape, const WPXPropertyList &props, libwpg::WPGPaintInterface *painter, double x, double y, double height, double width);

} // libmspub
#endif /* __POLYGONUTILS_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
