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
  for (unsigned i = 0; i < m_chidren.size(); ++i)
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
