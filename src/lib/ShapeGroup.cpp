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
#include "ShapeGroupPainter.h"
#include "ShapeGroup.h"

libmspub::VectorTransformation2D libmspub::ShapeGroup::getFoldedTransform()
{
  if (!m_parent)
  {
    return m_transform;
  }
  return m_parent->getFoldedTransform() * m_transform;
}

libmspub::VectorTransformation2D libmspub::ShapeGroupElementLeaf::getFoldedTransform(VectorTransformation2D init)
{
  if (m_parent)
  {
    return m_parent->getFoldedTransform() * init;
  }
  return init;
}

void libmspub::ShapeGroup::visit(ShapeGroupVisitor *v)
{
  v->group(this);
  for (unsigned i = 0; i < m_elements.size(); ++i)
  {
    m_elements[i].visit(v);
  }
  v->endGroup();
}

void libmspub::ShapeGroupElementLeaf::visit(ShapeGroupVisitor *v)
{
  v->shape(this);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
