/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
  virtual void output(librevenge::RVNGDrawingInterface *painter, Coordinate coord);
  virtual ~Shape()
  {
  }
  librevenge::RVNGPropertyList props;
  librevenge::RVNGPropertyList graphicsProps;
protected:
  virtual void setCoordProps(Coordinate coord);
  virtual void write(librevenge::RVNGDrawingInterface *painter) = 0;
  MSPUBCollector *owner;

  virtual librevenge::RVNGPropertyListVector updateGraphicsProps();

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
  virtual librevenge::RVNGPropertyListVector updateGraphicsProps();
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
  void writeText(librevenge::RVNGDrawingInterface *painter);
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
  void output(librevenge::RVNGDrawingInterface *painter, Coordinate coord);
protected:
  virtual bool hasFill();
  void setCoordProps(Coordinate coord);
  virtual void write(librevenge::RVNGDrawingInterface *painter);
  librevenge::RVNGPropertyListVector updateGraphicsProps();
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
