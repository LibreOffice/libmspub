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
#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <vector>
#include <map>

#include "Coordinate.h"
#include "MSPUBConstants.h"
#include "ShapeType.h"
#include "VectorTransformation2D.h"

namespace libmspub
{
class MSPUBCollector;
struct CustomShape;
struct Shape
{
  Shape(MSPUBCollector *o) : props(), graphicsProps(), owner(o) { }
  virtual void output(libwpg::WPGPaintInterface *painter, Coordinate coord);
  virtual ~Shape()
  {
  }
  WPXPropertyList props;
  WPXPropertyList graphicsProps;
protected:
  virtual void setCoordProps(Coordinate coord);
  virtual void write(libwpg::WPGPaintInterface *painter) = 0;
  MSPUBCollector *owner;

  virtual WPXPropertyListVector updateGraphicsProps();

  Shape();
private:
  Shape(const Shape &);
  Shape &operator=(const Shape &);
};
struct FillableShape : public Shape
{
  FillableShape(MSPUBCollector *o) : Shape(o), m_fill(NULL) { }
  Fill *m_fill;
  void setFill(Fill *fill);
protected:
  virtual WPXPropertyListVector updateGraphicsProps();
private:
  FillableShape(const FillableShape &);
  FillableShape &operator=(const FillableShape &);
};
struct GeometricShape : public FillableShape
{
  void addLine(ColorReference color, unsigned widthInEmu, bool lineExists);
  void fillDefaultAdjustValues();
  void setAdjustValue(unsigned index, int adjustValue);
  void setText(std::vector<TextParagraph> str);
  double getCalculationValue(unsigned index, bool recursiveEntry = false) const;
  double getSpecialValue(const CustomShape &shape, int arg) const;
  void writeText(libwpg::WPGPaintInterface *painter);
  void setTransformation(VectorTransformation2D transform);

  std::vector<TextParagraph> m_str;
  bool m_hasText;
  unsigned m_pageSeqNum;
  unsigned m_imgIndex;
  ShapeType m_type;
  double m_x, m_y, m_width, m_height;
  VectorTransformation2D m_transform;
  std::vector<int> m_adjustValues;
  unsigned m_left, m_top, m_right, m_bottom; //emu
  GeometricShape(MSPUBCollector *o)
    : FillableShape(o), m_str(), m_hasText(false), m_pageSeqNum(0), m_imgIndex(0), m_type(RECTANGLE),
      m_x(0), m_y(0), m_width(0), m_height(0), m_transform(VectorTransformation2D()),
      m_adjustValues(),
      m_left(DEFAULT_MARGIN), m_top(DEFAULT_MARGIN), m_right(DEFAULT_MARGIN), m_bottom(DEFAULT_MARGIN),
      m_valuesSeen(), m_filledDefaultAdjustValues(false), m_textCoord(), m_closeEverything(false),
      m_lines(), m_drawStroke(false),
      m_borderPosition(HALF_INSIDE_SHAPE),
      m_coordinatesRotated90(false), m_foldedTransform(VectorTransformation2D()) { }
  GeometricShape(unsigned pageSeqNum, MSPUBCollector *o)
    : FillableShape(o), m_str(), m_hasText(false), m_pageSeqNum(pageSeqNum), m_imgIndex(0), m_type(RECTANGLE),
      m_x(0), m_y(0), m_width(0), m_height(0), m_transform(VectorTransformation2D()), m_adjustValues(),
      m_left(DEFAULT_MARGIN), m_top(DEFAULT_MARGIN), m_right(DEFAULT_MARGIN), m_bottom(DEFAULT_MARGIN),
      m_valuesSeen(), m_filledDefaultAdjustValues(false), m_textCoord(), m_closeEverything(false),
      m_lines(), m_drawStroke(false),
      m_borderPosition(HALF_INSIDE_SHAPE),
      m_coordinatesRotated90(false), m_foldedTransform(VectorTransformation2D()) { }
  std::vector<Color> getPaletteColors() const;
  void output(libwpg::WPGPaintInterface *painter, Coordinate coord);
protected:
  virtual bool hasFill();
  void setCoordProps(Coordinate coord);
  virtual void write(libwpg::WPGPaintInterface *painter);
  WPXPropertyListVector updateGraphicsProps();
  GeometricShape();
private:
  GeometricShape(const GeometricShape &);
  GeometricShape &operator=(const GeometricShape &);
  mutable std::vector<bool> m_valuesSeen;
  bool m_filledDefaultAdjustValues;
  Coordinate m_textCoord;
  bool m_closeEverything;
public:
  std::vector<Line> m_lines;
  bool m_drawStroke;
  BorderPosition m_borderPosition;
  bool m_coordinatesRotated90;
  VectorTransformation2D m_foldedTransform;
};
} // namespace libmspub
#endif // __SHAPES_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
