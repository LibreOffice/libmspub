/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBMSPUB_UTILS_H
#define INCLUDED_LIBMSPUB_UTILS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <vector>
#include <map>

#include <boost/cstdint.hpp>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "MSPUBTypes.h"

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

unsigned long getLength(librevenge::RVNGInputStream *input);

void appendCharacters(librevenge::RVNGString &text, std::vector<unsigned char> characters, const char *encoding);

bool stillReading(librevenge::RVNGInputStream *input, unsigned long until);

void rotateCounter(double &x, double &y, double centerX, double centerY, short rotation);
void flipIfNecessary(double &x, double &y, double centerX, double centerY, bool flipVertical, bool flipHorizontal);

unsigned correctModulo(int x, unsigned n);
double doubleModulo(double x, double y);

template <class MapT> typename MapT::mapped_type *getIfExists(MapT &map, const typename MapT::key_type &key)
{
  auto i = map.find(key);
  return i == map.end() ? nullptr : &(i->second);
}

template <class MapT> const typename MapT::mapped_type *getIfExists_const(MapT &map, const typename MapT::key_type &key)
{
  auto i = map.find(key);
  return i == map.end() ? nullptr : &(i->second);
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

#endif // INCLUDED_LIBMSPUB_UTILS_H
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
