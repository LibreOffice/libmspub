/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SHAPEINFO_H
#define INCLUDED_SHAPEINFO_H
#include <boost/optional.hpp>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include "ShapeType.h"
#include "Coordinate.h"
#include "Line.h"
#include "Margins.h"
#include "MSPUBTypes.h"
#include "Fill.h"
#include "PolygonUtils.h"
#include "Dash.h"
#include "TableInfo.h"
#include "Arrow.h"
#include "VerticalAlign.h"
#include "ColorReference.h"
#include "Shadow.h"

namespace libmspub
{
void noop(const CustomShape *);
struct ShapeInfo
{
  boost::optional<ShapeType> m_type;
  boost::optional<ShapeType> m_cropType;
  boost::optional<unsigned> m_imgIndex;
  boost::optional<unsigned> m_borderImgIndex;
  boost::optional<Coordinate> m_coordinates;
  std::vector<Line> m_lines;
  boost::optional<unsigned> m_pageSeqNum;
  boost::optional<unsigned> m_textId;
  std::map<unsigned, int> m_adjustValuesByIndex;
  std::vector<int> m_adjustValues;
  boost::optional<double> m_rotation;
  boost::optional<std::pair<bool, bool> > m_flips;
  boost::optional<Margins> m_margins;
  boost::optional<BorderPosition> m_borderPosition; // Irrelevant except for rectangular shapes
  std::shared_ptr<const Fill> m_fill;
  boost::optional<DynamicCustomShape> m_customShape;
  bool m_stretchBorderArt;
  boost::optional<ColorReference> m_lineBackColor;
  boost::optional<Dash> m_dash;
  boost::optional<TableInfo> m_tableInfo;
  boost::optional<unsigned> m_numColumns;
  unsigned m_columnSpacing;
  boost::optional<Arrow> m_beginArrow;
  boost::optional<Arrow> m_endArrow;
  boost::optional<VerticalAlign> m_verticalAlign;
  boost::optional<ColorReference> m_pictureRecolor;
  boost::optional<Shadow> m_shadow;
  boost::optional<int> m_innerRotation;
  std::vector<libmspub::Vertex> m_clipPath;
  boost::optional<int> m_pictureBrightness;
  boost::optional<int> m_pictureContrast;
  ShapeInfo() : m_type(), m_cropType(), m_imgIndex(), m_borderImgIndex(),
    m_coordinates(), m_lines(), m_pageSeqNum(),
    m_textId(), m_adjustValuesByIndex(), m_adjustValues(),
    m_rotation(), m_flips(), m_margins(), m_borderPosition(),
    m_fill(), m_customShape(), m_stretchBorderArt(false),
    m_lineBackColor(), m_dash(), m_tableInfo(),
    m_numColumns(),
    m_columnSpacing(0), m_beginArrow(), m_endArrow(),
    m_verticalAlign(), m_pictureRecolor(), m_shadow(), m_innerRotation(), m_clipPath(), m_pictureBrightness(), m_pictureContrast()
  {
  }
  std::shared_ptr<const CustomShape> getCustomShape() const
  {
    if (bool(m_customShape))
    {
      return getFromDynamicCustomShape(m_customShape.get());
    }
    if (bool(m_cropType))
    {
      return std::shared_ptr<const CustomShape>(
               libmspub::getCustomShape(m_cropType.get()),
               std::function<void (const CustomShape *)>(noop));
    }
    return std::shared_ptr<const CustomShape>(
             libmspub::getCustomShape(m_type.get_value_or(RECTANGLE)),
             std::function<void (const CustomShape *)>(noop));
  }
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
