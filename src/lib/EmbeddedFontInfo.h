/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __EMBEDDEDFONTINFO_H__
#define __EMBEDDEDFONTINFO_H__

#include <librevenge/librevenge.h>

namespace libmspub
{
struct EmbeddedFontInfo
{
  librevenge::RVNGString m_name;
  librevenge::RVNGBinaryData m_blob;
  EmbeddedFontInfo(const librevenge::RVNGString &name) :
    m_name(name), m_blob()
  {
  }
};
} // namespace libmspub

#endif /* __EMBEDDEDFONTINFO_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
