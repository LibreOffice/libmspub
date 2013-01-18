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
 * Copyright (C) 2011 Fridrich Strba <fridrich.strba@bluewin.ch>
 * Copyright (C) 2011 Eilidh McAdam <tibbylickle@gmail.com>
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

#include <unicode/ucnv.h>
#include <unicode/utypes.h>

#include <string.h> // for memcpy
#include <math.h>
#include <zlib.h>
#include <cstring>

#include "libmspub_utils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ZLIB_CHUNK 16384

using std::strcmp;
const char *libmspub::windowsCharsetNameByOriginalCharset(const char *name)
{
  if (strcmp(name, "Shift_JIS") == 0)
  {
    return "windows-932";
  }
  if (strcmp(name, "GB18030") == 0)
  {
    return "windows-936";
  }
  if (strcmp(name, "Big5") == 0)
  {
    return "windows-950";
  }
  if (strcmp(name, "ISO-8859-1") == 0)
  {
    return "windows-1252";
  }
  if (strcmp(name, "ISO-8859-2") == 0)
  {
    return "windows-1250";
  }
  if (strcmp(name, "windows-1251") == 0)
  {
    return "windows-1251";
  }
  if (strcmp(name, "windows-1256") == 0)
  {
    return "windows-1256";
  }
  return NULL;
}

const char *libmspub::mimeByImgType(ImgType type)
{
  switch (type)
  {
  case PNG:
    return "image/png";
  case JPEG:
    return "image/jpeg";
  case DIB:
    return "image/bmp";
  case PICT:
    return "image/pict";
  case WMF:
    return "image/wmf";
  case EMF:
    return "image/emf";
  case TIFF:
    return "image/tiff";
  default:
    MSPUB_DEBUG_MSG(("Unknown image type %d passed to mimeByImgType!\n", type));
    return 0;
  }
}

void libmspub::rotateCounter(double &x, double &y, double centerX, double centerY, short rotation)
{
  double vecX = x - centerX;
  double vecY = centerY - y;
  double sinTheta = sin(rotation * M_PI / 180.);
  double cosTheta = cos(rotation * M_PI / 180.);
  double newVecX = cosTheta * vecX - sinTheta * vecY;
  double newVecY = sinTheta * vecX + cosTheta * vecY;
  x = centerX + newVecX;
  y = centerY - newVecY;
}

double libmspub::doubleModulo(double x, double y)
{
  if (y <= 0) // y <= 0 doesn't make sense
  {
    return x;
  }
  while (x < 0)
  {
    x += y;
  }
  while (x >= y)
  {
    x -= y;
  }
  return x;
}

double libmspub::toFixedPoint(int fp)
{
  unsigned short fractionalPart = ((unsigned short) fp) & 0xFFFF;
  short integralPart = fp >> 16;
  return integralPart + fractionalPart / 65536.;
}

double libmspub::readFixedPoint(WPXInputStream *input)
{
  return toFixedPoint(readS32(input));
}

void libmspub::flipIfNecessary(double &x, double &y, double centerX, double centerY, bool flipVertical, bool flipHorizontal)
{
  double vecX = x - centerX;
  double vecY = centerY - y;
  if (flipVertical)
  {
    y = centerY + vecY;
  }
  if (flipHorizontal)
  {
    x = centerX - vecX;
  }
}

unsigned libmspub::correctModulo(int x, unsigned n) // returns the canonical representation of x in Z/nZ
//difference with C++ % operator is that this never returns negative values.
{
  if (x < 0)
  {
    int result = x % (int)n;
    //sign of result is implementation defined
    if (result < 0)
    {
      return n + result;
    }
    return result;
  }
  return x % n;
}

WPXBinaryData libmspub::inflateData(WPXBinaryData deflated)
{
  WPXBinaryData inflated;
  unsigned char out[ZLIB_CHUNK];
  const unsigned char *data = deflated.getDataBuffer();
  z_stream strm;
  int ret;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  if (inflateInit2(&strm,-MAX_WBITS) != Z_OK)
  {
    return WPXBinaryData();
  }
  int have;
  unsigned left = deflated.size();
  do
  {
    strm.avail_in = ZLIB_CHUNK > left ? left : ZLIB_CHUNK;
    strm.next_in = (unsigned char *)data;
    do
    {
      strm.avail_out = ZLIB_CHUNK;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      if (ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
      {
        inflateEnd(&strm);
        return WPXBinaryData();
      }
      have = ZLIB_CHUNK - strm.avail_out;
      inflated.append(out, have);
    }
    while (strm.avail_out == 0);
    data += ZLIB_CHUNK > left ? left : ZLIB_CHUNK;
    left -= ZLIB_CHUNK > left ? left : ZLIB_CHUNK;
  }
  while (ret != Z_STREAM_END);
  inflateEnd(&strm);
  return inflated;
}

namespace
{

static void _appendUCS4(WPXString &text, unsigned ucs4Character)
{
  unsigned char first;
  int len;
  if (ucs4Character < 0x80)
  {
    first = 0;
    len = 1;
  }
  else if (ucs4Character < 0x800)
  {
    first = 0xc0;
    len = 2;
  }
  else if (ucs4Character < 0x10000)
  {
    first = 0xe0;
    len = 3;
  }
  else if (ucs4Character < 0x200000)
  {
    first = 0xf0;
    len = 4;
  }
  else if (ucs4Character < 0x4000000)
  {
    first = 0xf8;
    len = 5;
  }
  else
  {
    first = 0xfc;
    len = 6;
  }

  unsigned char outbuf[6] = { 0, 0, 0, 0, 0, 0 };
  int i;
  for (i = len - 1; i > 0; --i)
  {
    outbuf[i] = (ucs4Character & 0x3f) | 0x80;
    ucs4Character >>= 6;
  }
  outbuf[0] = (ucs4Character & 0xff) | first;

  for (i = 0; i < len; i++)
    text.append(outbuf[i]);
}

} // anonymous namespace

#define MSPUB_NUM_ELEMENTS(array) sizeof(array)/sizeof(array[0])

uint8_t libmspub::readU8(WPXInputStream *input)
{
  if (!input || input->atEOS())
  {
    MSPUB_DEBUG_MSG(("Something bad happened here!Tell: %ld\n", input->tell()));
    throw EndOfStreamException();
  }
  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint8_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint8_t))
    return *(uint8_t const *)(p);
  throw EndOfStreamException();
}

uint16_t libmspub::readU16(const unsigned char *input, unsigned offset)
{
  uint16_t p0 = (uint16_t)(*(input + offset));
  uint16_t p1 = (uint16_t)(*(input + offset + 1));
  return (uint16_t)(p0|(p1<<8));
}

uint32_t libmspub::readU32(const unsigned char *input, unsigned offset)
{
  uint32_t p0 = (uint32_t)(*(input + offset));
  uint32_t p1 = (uint32_t)(*(input + offset + 1));
  uint32_t p2 = (uint32_t)(*(input + offset + 2));
  uint32_t p3 = (uint32_t)(*(input + offset + 3));
  return (uint32_t)(p0|(p1<<8)|(p2<<16)|(p3<<24));
}

uint16_t libmspub::readU16(WPXInputStream *input)
{
  uint16_t p0 = (uint16_t)readU8(input);
  uint16_t p1 = (uint16_t)readU8(input);
  return (uint16_t)(p0|(p1<<8));
}

uint32_t libmspub::readU32(WPXInputStream *input)
{
  uint32_t p0 = (uint32_t)readU8(input);
  uint32_t p1 = (uint32_t)readU8(input);
  uint32_t p2 = (uint32_t)readU8(input);
  uint32_t p3 = (uint32_t)readU8(input);
  return (uint32_t)(p0|(p1<<8)|(p2<<16)|(p3<<24));
}

int8_t libmspub::readS8(WPXInputStream *input)
{
  return (int8_t)readU8(input);
}

int16_t libmspub::readS16(WPXInputStream *input)
{
  return (int16_t)readU16(input);
}

int32_t libmspub::readS32(WPXInputStream *input)
{
  return (int32_t)readU32(input);
}

uint64_t libmspub::readU64(WPXInputStream *input)
{
  uint64_t p0 = (uint64_t)readU8(input);
  uint64_t p1 = (uint64_t)readU8(input);
  uint64_t p2 = (uint64_t)readU8(input);
  uint64_t p3 = (uint64_t)readU8(input);
  uint64_t p4 = (uint64_t)readU8(input);
  uint64_t p5 = (uint64_t)readU8(input);
  uint64_t p6 = (uint64_t)readU8(input);
  uint64_t p7 = (uint64_t)readU8(input);
  return (uint64_t)(p0|(p1<<8)|(p2<<16)|(p3<<24)|(p4<<32)|(p5<<40)|(p6<<48)|(p7<<56));
}

void libmspub::readNBytes(WPXInputStream *input, unsigned long length, std::vector<unsigned char> &out)
{
  unsigned long numBytesRead = 0;
  const unsigned char *tmpBuffer = input->read(length, numBytesRead);
  if (numBytesRead != length)
  {
    out.clear();
    return;
  }
  out = std::vector<unsigned char>(numBytesRead);
  memcpy(&out[0], tmpBuffer, numBytesRead);
  return;
}

#define SURROGATE_VALUE(h,l) (((h) - 0xd800) * 0x400 + (l) - 0xdc00 + 0x10000)


void libmspub::appendCharacters(WPXString &text, const std::vector<unsigned char> characters,
                                const char *encoding)
{
  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = NULL;
  conv = ucnv_open(encoding, &status);
  if (U_SUCCESS(status))
  {
    // ICU documentation claims that character-by-character processing is faster "for small amounts of data" and "'normal' charsets"
    // (in any case, it is more convenient :) )
    const char *src = (const char *)&characters[0];
    const char *srcLimit = (const char *)src + characters.size();
    while (src < srcLimit)
    {
      uint32_t ucs4Character = (uint32_t)ucnv_getNextUChar(conv, &src, srcLimit, &status);
      if (U_SUCCESS(status))
      {
        _appendUCS4(text, ucs4Character);
      }
    }
  }
  if (conv)
  {
    ucnv_close(conv);
  }
}

bool libmspub::stillReading(WPXInputStream *input, unsigned long until)
{
  if (input->atEOS())
    return false;
  if (input->tell() < 0)
    return false;
  if ((unsigned long)input->tell() >= until)
    return false;
  return true;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
