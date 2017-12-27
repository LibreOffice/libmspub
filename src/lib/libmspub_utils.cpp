/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libmspub_utils.h"

#include <cstdarg>
#include <cstring>
#include <math.h>
#include <string.h> // for memcpy

#include <unicode/ucnv.h>
#include <unicode/utypes.h>

#include <zlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ZLIB_CHUNK 16384

namespace libmspub
{

#ifdef DEBUG

void debugPrint(const char *const format, ...)
{
  va_list args;
  va_start(args, format);
  std::vfprintf(stderr, format, args);
  va_end(args);
}

#endif

using std::strcmp;
const char *windowsCharsetNameByOriginalCharset(const char *name)
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
  return nullptr;
}

const char *mimeByImgType(ImgType type)
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
    return nullptr;
  }
}

void rotateCounter(double &x, double &y, double centerX, double centerY, short rotation)
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

double doubleModulo(double x, double y)
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

double toFixedPoint(int fp)
{
  unsigned short fractionalPart = ((unsigned short) fp) & 0xFFFF;
  short integralPart = fp >> 16;
  return integralPart + fractionalPart / 65536.;
}

double readFixedPoint(librevenge::RVNGInputStream *input)
{
  return toFixedPoint(readS32(input));
}

void flipIfNecessary(double &x, double &y, double centerX, double centerY, bool flipVertical, bool flipHorizontal)
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

unsigned correctModulo(int x, unsigned n) // returns the canonical representation of x in Z/nZ
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

librevenge::RVNGBinaryData inflateData(librevenge::RVNGBinaryData deflated)
{
  librevenge::RVNGBinaryData inflated;
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
    return librevenge::RVNGBinaryData();
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
      if (ret < 0 || ret == Z_NEED_DICT)
      {
        inflateEnd(&strm);
        return librevenge::RVNGBinaryData();
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

static void _appendUCS4(librevenge::RVNGString &text, unsigned ucs4Character)
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

  char outbuf[7] = { 0 };
  int i;
  for (i = len - 1; i > 0; --i)
  {
    outbuf[i] = char((ucs4Character & 0x3f) | 0x80);
    ucs4Character >>= 6;
  }
  outbuf[0] = char((ucs4Character & 0xff) | first);
  outbuf[len] = '\0';

  text.append(outbuf);
}

} // anonymous namespace

#define MSPUB_NUM_ELEMENTS(array) sizeof(array)/sizeof(array[0])

uint8_t readU8(librevenge::RVNGInputStream *input)
{
  if (!input || input->isEnd())
  {
    MSPUB_DEBUG_MSG(("Something bad happened here!"));
    if (input)
    {
      MSPUB_DEBUG_MSG((" Tell: %ld\n", input->tell()));
    }
    throw EndOfStreamException();
  }
  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint8_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint8_t))
    return *(uint8_t const *)(p);
  throw EndOfStreamException();
}

uint16_t readU16(librevenge::RVNGInputStream *input)
{
  auto p0 = (uint16_t)readU8(input);
  auto p1 = (uint16_t)readU8(input);
  return (uint16_t)(p0|(p1<<8));
}

uint32_t readU32(librevenge::RVNGInputStream *input)
{
  auto p0 = (uint32_t)readU8(input);
  auto p1 = (uint32_t)readU8(input);
  auto p2 = (uint32_t)readU8(input);
  auto p3 = (uint32_t)readU8(input);
  return (uint32_t)(p0|(p1<<8)|(p2<<16)|(p3<<24));
}

int8_t readS8(librevenge::RVNGInputStream *input)
{
  return (int8_t)readU8(input);
}

int16_t readS16(librevenge::RVNGInputStream *input)
{
  return (int16_t)readU16(input);
}

int32_t readS32(librevenge::RVNGInputStream *input)
{
  return (int32_t)readU32(input);
}

uint64_t readU64(librevenge::RVNGInputStream *input)
{
  auto p0 = (uint64_t)readU8(input);
  auto p1 = (uint64_t)readU8(input);
  auto p2 = (uint64_t)readU8(input);
  auto p3 = (uint64_t)readU8(input);
  auto p4 = (uint64_t)readU8(input);
  auto p5 = (uint64_t)readU8(input);
  auto p6 = (uint64_t)readU8(input);
  auto p7 = (uint64_t)readU8(input);
  return (uint64_t)(p0|(p1<<8)|(p2<<16)|(p3<<24)|(p4<<32)|(p5<<40)|(p6<<48)|(p7<<56));
}

void readNBytes(librevenge::RVNGInputStream *input, unsigned long length, std::vector<unsigned char> &out)
{
  if (length == 0)
  {
    MSPUB_DEBUG_MSG(("Attempt to read 0 bytes!"));
    return;
  }

  unsigned long numBytesRead = 0;
  const unsigned char *tmpBuffer = input->read(length, numBytesRead);
  if (numBytesRead != length)
  {
    out.clear();
    return;
  }
  out = std::vector<unsigned char>(numBytesRead);
  memcpy(out.data(), tmpBuffer, numBytesRead);
  return;
}

unsigned long getLength(librevenge::RVNGInputStream *const input)
{
  if (!input)
    throw EndOfStreamException();

  const long orig = input->tell();

  unsigned long end = 0;

  if (0 == input->seek(0, librevenge::RVNG_SEEK_END))
  {
    end = static_cast<unsigned long>(input->tell());
  }
  else
  {
    // RVNG_SEEK_END does not work. Use the harder way.
    if (0 != input->seek(0, librevenge::RVNG_SEEK_SET))
      throw EndOfStreamException();
    while (!input->isEnd())
    {
      readU8(input);
      ++end;
    }
  }

  if (0 != input->seek(orig, librevenge::RVNG_SEEK_SET))
    throw EndOfStreamException();

  return end;
}

#define SURROGATE_VALUE(h,l) (((h) - 0xd800) * 0x400 + (l) - 0xdc00 + 0x10000)


void appendCharacters(librevenge::RVNGString &text, const std::vector<unsigned char> characters,
                      const char *encoding)
{
  if (characters.empty())
  {
    MSPUB_DEBUG_MSG(("Attempt to append 0 characters!"));
    return;
  }

  UErrorCode status = U_ZERO_ERROR;
  UConverter *conv = nullptr;
  conv = ucnv_open(encoding, &status);
  if (U_SUCCESS(status))
  {
    // ICU documentation claims that character-by-character processing is faster "for small amounts of data" and "'normal' charsets"
    // (in any case, it is more convenient :) )
    const auto *src = (const char *)characters.data();
    const char *srcLimit = (const char *)src + characters.size();
    while (src < srcLimit)
    {
      auto ucs4Character = (uint32_t)ucnv_getNextUChar(conv, &src, srcLimit, &status);
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

bool stillReading(librevenge::RVNGInputStream *input, unsigned long until)
{
  if (input->isEnd())
    return false;
  if (input->tell() < 0)
    return false;
  if ((unsigned long)input->tell() >= until)
    return false;
  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
