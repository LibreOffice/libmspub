/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __LIBMSPUB_UTILS_H__
#define __LIBMSPUB_UTILS_H__

#include <stdio.h>
#include <vector>
#include <map>
#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

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

uint8_t readU8(librevenge::RVNGInputStream *input);
uint16_t readU16(librevenge::RVNGInputStream *input);
uint32_t readU32(librevenge::RVNGInputStream *input);
uint64_t readU64(librevenge::RVNGInputStream *input);
int8_t readS8(librevenge::RVNGInputStream *input);
int16_t readS16(librevenge::RVNGInputStream *input);
int32_t readS32(librevenge::RVNGInputStream *input);
double readFixedPoint(librevenge::RVNGInputStream *input);
double toFixedPoint(int fp);
void readNBytes(librevenge::RVNGInputStream *input, unsigned long length, std::vector<unsigned char> &out);

void appendCharacters(librevenge::RVNGString &text, std::vector<unsigned char> characters, const char *encoding);

bool stillReading(librevenge::RVNGInputStream *input, unsigned long until);

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

librevenge::RVNGBinaryData inflateData(librevenge::RVNGBinaryData);

} // namespace libmspub

#endif // __LIBMSPUB_UTILS_H__
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
