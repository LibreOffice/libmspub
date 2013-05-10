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
 * Copyright (C) 2012-2013 Brennan Vincent <brennanv@email.arizona.edu>
 * Copyright (C) 2012 Fridrich Strba <fridrich.strba@bluewin.ch>
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

#ifndef __LIBMSPUB_UTILS_H__
#define __LIBMSPUB_UTILS_H__

#include <stdio.h>
#include <vector>
#include <map>
#include <libwpd/libwpd.h>
#include <libwpd-stream/libwpd-stream.h>

#include "MSPUBTypes.h"

#ifdef _MSC_VER

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned __int64 uint64_t;

#else

#ifdef HAVE_CONFIG_H

#include <config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#else

// assume that the headers are there inside LibreOffice build when no HAVE_CONFIG_H is defined
#include <stdint.h>
#include <inttypes.h>

#endif

#endif

// debug message includes source file and line number
//#define VERBOSE_DEBUG 1

// do nothing with debug messages in a release compile
#ifdef DEBUG
#ifdef VERBOSE_DEBUG
#define MSPUB_DEBUG_MSG(M) printf("%15s:%5d: ", __FILE__, __LINE__); printf M
#define MSPUB_DEBUG(M) M
#else
#define MSPUB_DEBUG_MSG(M) printf M
#define MSPUB_DEBUG(M) M
#endif
#else
#define MSPUB_DEBUG_MSG(M)
#define MSPUB_DEBUG(M)
#endif

namespace libmspub
{
const char *mimeByImgType(ImgType type);
const char *windowsCharsetNameByOriginalCharset(const char *name);

uint8_t readU8(WPXInputStream *input);
uint16_t readU16(WPXInputStream *input);
uint32_t readU32(WPXInputStream *input);
uint64_t readU64(WPXInputStream *input);
int8_t readS8(WPXInputStream *input);
int16_t readS16(WPXInputStream *input);
int32_t readS32(WPXInputStream *input);
double readFixedPoint(WPXInputStream *input);
double toFixedPoint(int fp);
void readNBytes(WPXInputStream *input, unsigned long length, std::vector<unsigned char> &out);

void appendCharacters(WPXString &text, std::vector<unsigned char> characters, const char *encoding);

bool stillReading(WPXInputStream *input, unsigned long until);

void rotateCounter(double &x, double &y, double centerX, double centerY, short rotation);
void flipIfNecessary(double &x, double &y, double centerX, double centerY, bool flipVertical, bool flipHorizontal);

unsigned correctModulo(int x, unsigned n);
double doubleModulo(double x, double y);

template <class MapT> typename MapT::mapped_type *getIfExists(MapT &map, const typename MapT::key_type &key)
{
  typename MapT::iterator i = map.find(key);
  return i == map.end() ? NULL : &(i->second);
}

template <class MapT> const typename MapT::mapped_type *getIfExists_const(MapT &map, const typename MapT::key_type &key)
{
  typename MapT::const_iterator i = map.find(key);
  return i == map.end() ? NULL : &(i->second);
}

template <class MapT> typename MapT::mapped_type ptr_getIfExists(MapT &map, const typename MapT::key_type &key)
{
  typename MapT::iterator i = map.find(key);
  return i == map.end() ? NULL : i->second;
}

class EndOfStreamException
{
};

class GenericException
{
};

WPXBinaryData inflateData(WPXBinaryData);

} // namespace libmspub

#endif // __LIBMSPUB_UTILS_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
