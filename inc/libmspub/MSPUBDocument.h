/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __MSPUBDOCUMENT_H__
#define __MSPUBDOCUMENT_H__

#include <librevenge/librevenge.h>

#ifdef DLL_EXPORT
#ifdef LIBMSPUB_BUILD
#define PUBAPI __declspec(dllexport)
#else
#define PUBAPI __declspec(dllimport)
#endif
#else
#define PUBAPI
#endif

namespace libmspub
{
class MSPUBDocument
{
public:

  static PUBAPI bool isSupported(librevenge::RVNGInputStream *input);

  static PUBAPI bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGDrawingInterface *painter);
};

} // namespace libmspub

#endif //  __MSPUBDOCUMENT_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
