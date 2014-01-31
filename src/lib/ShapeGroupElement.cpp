/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ShapeGroupElement.h"

libmspub::ShapeGroupElement::ShapeGroupElement(ShapeGroupElement *parent) : m_shapeInfo(), m_parent(parent), m_children(), m_seqNum(0), m_transform()
{
  if (m_parent)
  {
    m_parent->m_children.push_back(this);
  }
}

libmspub::ShapeGroupElement::~ShapeGroupElement()
{
  for (unsigned i = 0; i < m_children.size(); ++i)
  {
    delete m_children[i];
  }
}

libmspub::ShapeGroupElement::ShapeGroupElement(ShapeGroupElement *parent, unsigned seqNum) : m_shapeInfo(), m_parent(parent), m_children(), m_seqNum(seqNum), m_transform()
{
  if (m_parent)
  {
    m_parent->m_children.push_back(this);
  }
}

void libmspub::ShapeGroupElement::setShapeInfo(const ShapeInfo &shapeInfo)
{
  m_shapeInfo = shapeInfo;
}

void libmspub::ShapeGroupElement::setTransform(const VectorTransformation2D &transform)
{
  m_transform = transform;
}

void libmspub::ShapeGroupElement::setup(boost::function<void(ShapeGroupElement &self)> visitor)
{
  visitor(*this);
  for (unsigned i = 0; i < m_children.size(); ++i)
  {
    m_children[i]->setup(visitor);
  }
}

void libmspub::ShapeGroupElement::visit(boost::function<
                                        boost::function<void(void)>
                                        (const libmspub::ShapeInfo &info, const libmspub::Coordinate &relativeTo, const libmspub::VectorTransformation2D &foldedTransform, bool isGroup, const libmspub::VectorTransformation2D &thisTransform)
                                        > visitor, const Coordinate &relativeTo, const VectorTransformation2D &parentFoldedTransform) const
{
  const ShapeInfo &info = m_shapeInfo.get_value_or(ShapeInfo());
  Coordinate coord = info.m_coordinates.get_value_or(Coordinate());
  double centerX = ((double)coord.m_xs + (double)coord.m_xe) / (2 * EMUS_IN_INCH);
  double centerY = ((double)coord.m_ys + (double)coord.m_ye) / (2 * EMUS_IN_INCH);
  double relativeCenterX = ((double)relativeTo.m_xs + (double)relativeTo.m_xe) / (2 * EMUS_IN_INCH);
  double relativeCenterY = ((double)relativeTo.m_ys + (double)relativeTo.m_ye) / (2 * EMUS_IN_INCH);
  double offsetX = centerX - relativeCenterX;
  double offsetY = centerY - relativeCenterY;
  VectorTransformation2D foldedTransform = VectorTransformation2D::fromTranslate(-offsetX, -offsetY)
                                           * parentFoldedTransform * VectorTransformation2D::fromTranslate(offsetX, offsetY) * m_transform;
  boost::function<void(void)> afterOp = visitor(info, relativeTo, foldedTransform, isGroup(), m_transform);
  for (unsigned i = 0; i < m_children.size(); ++i)
  {
    m_children[i]->visit(visitor, coord, foldedTransform);
  }
  afterOp();
}

void libmspub::ShapeGroupElement::visit(boost::function<
                                        boost::function<void(void)>
                                        (const libmspub::ShapeInfo &info, const libmspub::Coordinate &relativeTo, const libmspub::VectorTransformation2D &foldedTransform, bool isGroup, const libmspub::VectorTransformation2D &thisTransform)
                                        > visitor) const
{
  Coordinate origin;
  VectorTransformation2D identity;
  visit(visitor, origin, identity);
}

bool libmspub::ShapeGroupElement::isGroup() const
{
  return !m_children.empty();
}

libmspub::ShapeGroupElement *libmspub::ShapeGroupElement::getParent()
{
  return m_parent;
}

const libmspub::ShapeGroupElement *libmspub::ShapeGroupElement::getParent() const
{
  return m_parent;
}

void libmspub::ShapeGroupElement::setSeqNum(unsigned seqNum)
{
  m_seqNum = seqNum;
}

unsigned libmspub::ShapeGroupElement::getSeqNum() const
{
  return m_seqNum;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
