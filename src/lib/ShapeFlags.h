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

#ifndef INCLUDED_SHAPEFLAGS_H
#define INCLUDED_SHAPEFLAGS_H

#define SF_GROUP       (1 << 0)
#define SF_CHILD       (1 << 1)
#define SF_PATRIARCH   (1 << 2)
#define SF_DELETED     (1 << 3)
#define SF_OLE_SHAPE   (1 << 4)
#define SF_HAVE_MASTER (1 << 5)
#define SF_FLIP_H      (1 << 6)
#define SF_FLIP_V      (1 << 7)
#define SF_CONNECTOR   (1 << 8)
#define SF_HAVE_ANCHOR (1 << 9)
#define SF_BACKGROUND  (1 << 10)
#define SF_HAVE_SPT    (1 << 11)

#endif /* INCLUDED_SHAPEFLAGS_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
