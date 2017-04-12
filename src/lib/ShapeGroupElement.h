/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __SHAPEGROUPELEMENT_H__
#define __SHAPEGROUPELEMENT_H__
#include <boost/optional.hpp>
#include <functional>
#include <vector>

#include "ShapeInfo.h"
#include "VectorTransformation2D.h"

namespace libmspub
{
class ShapeGroupElement
{
  boost::optional<ShapeInfo> m_shapeInfo;
  ShapeGroupElement *m_parent;
  std::vector<ShapeGroupElement *> m_children;
  unsigned m_seqNum;
  ShapeGroupElement &operator=(const ShapeGroupElement &); //not implemented
  ShapeGroupElement(const ShapeGroupElement &); //not implemented
  VectorTransformation2D m_transform;
public:
  ShapeGroupElement(ShapeGroupElement *parent);
  ShapeGroupElement(ShapeGroupElement *parent, unsigned seqNum);
  ~ShapeGroupElement();
  void setShapeInfo(const ShapeInfo &shapeInfo);
  void setup(std::function<void(ShapeGroupElement &self)> visitor);
  void visit(std::function<
             std::function<void(void)>
             (const ShapeInfo &info, const Coordinate &relativeTo, const VectorTransformation2D &foldedTransform, bool isGroup, const VectorTransformation2D &thisTransform)> visitor,
             const Coordinate &relativeTo, const VectorTransformation2D &foldedTransform) const;
  void visit(std::function<
             std::function<void(void)>
             (const ShapeInfo &info, const Coordinate &relativeTo, const VectorTransformation2D &foldedTransform, bool isGroup, const VectorTransformation2D &thisTransform)> visitor) const;
  bool isGroup() const;
  ShapeGroupElement *getParent();
  const ShapeGroupElement *getParent() const;
  void setSeqNum(unsigned seqNum);
  void setTransform(const VectorTransformation2D &transform);
  unsigned getSeqNum() const;
};
}

#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
