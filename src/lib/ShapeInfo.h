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
#ifndef __SHAPEINFO_H__
#define __SHAPEINFO_H__
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <boost/function.hpp>
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
  boost::shared_ptr<const Fill> m_fill;
  boost::optional<DynamicCustomShape> m_customShape;
  bool m_stretchBorderArt;
  boost::optional<ColorReference> m_lineBackColor;
  boost::optional<Dash> m_dash;
  boost::optional<TableInfo> m_tableInfo;
  boost::optional<std::vector<unsigned> > m_tableCellTextEnds;
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
    m_tableCellTextEnds(), m_numColumns(),
    m_columnSpacing(0), m_beginArrow(), m_endArrow(),
    m_verticalAlign(), m_pictureRecolor(), m_shadow(), m_innerRotation(), m_clipPath(), m_pictureBrightness(), m_pictureContrast()
  {
  }
  boost::shared_ptr<const CustomShape> getCustomShape() const
  {
    if (!!m_customShape)
    {
      return getFromDynamicCustomShape(m_customShape.get());
    }
    if (!!m_cropType)
    {
      return boost::shared_ptr<const CustomShape>(
               libmspub::getCustomShape(m_cropType.get()),
               boost::function<void (const CustomShape *)>(noop));
    }
    return boost::shared_ptr<const CustomShape>(
             libmspub::getCustomShape(m_type.get_value_or(RECTANGLE)),
             boost::function<void (const CustomShape *)>(noop));
  }
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
