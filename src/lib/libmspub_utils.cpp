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

#include <string.h> // for memcpy
#include "libmspub_utils.h"
#include "puff.h"

#define ZLIB_CHUNK 16384

WPXBinaryData libmspub::undeflate(WPXBinaryData deflated)
{
  unsigned long destlen = 0;
  unsigned char *dest = NULL;
  const unsigned char *source = deflated.getDataBuffer();
  unsigned long sourcelen = deflated.size();
  puff(dest, &destlen, (unsigned char *)source, &sourcelen);
  dest = (unsigned char *)malloc(destlen);
  puff(dest, &destlen, (unsigned char *)source, &sourcelen);
  WPXBinaryData ret;
  ret.append(dest, destlen);
  return ret;
/*  WPXBinaryData inflated;
  unsigned char out[ZLIB_CHUNK];
  const unsigned char *data = deflated.getDataBuffer();
  z_stream strm;
  int ret;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  if (inflateInit(&strm) != Z_OK)
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
    } while (strm.avail_out == 0);
    data += ZLIB_CHUNK > left ? left : ZLIB_CHUNK;
    left -= ZLIB_CHUNK > left ? left : ZLIB_CHUNK;
  } while (ret != Z_STREAM_END);
  inflateEnd(&strm);
  return ret == Z_STREAM_END ? inflated : WPXBinaryData(); */
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

void libmspub::appendCharacters(WPXString &text, const std::vector<unsigned char> characters)
{
  for (std::vector<unsigned char>::const_iterator iter = characters.begin();
       iter != characters.end();)
  {
    uint16_t high_surrogate = 0;
    bool fail = false;
    uint32_t ucs4Character = 0;
    while (true)
    {
      if (iter == characters.end())
      {
        fail = true;
        break;
      }
      uint16_t character = *iter++;
      character |= (uint16_t)(*iter++) << 8;
      if (character >= 0xdc00 && character < 0xe000) /* low surrogate */
      {
        if (high_surrogate)
        {
          ucs4Character = SURROGATE_VALUE(high_surrogate, character);
          high_surrogate = 0;
          break;
        }
        else
        {
          fail = true;
          break;
        }
      }
      else
      {
        if (high_surrogate)
        {
          fail = true;
          break;
        }
        if (character >= 0xd800 && character < 0xdc00) /* high surrogate */
          high_surrogate = character;
        else
        {
          ucs4Character = character;
          break;
        }
      }
    }
    if (fail)
      throw libmspub::GenericException();

    _appendUCS4(text, ucs4Character);
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
