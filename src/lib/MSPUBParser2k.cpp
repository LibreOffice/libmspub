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

#include <libwpd-stream/WPXStream.h>
#include "MSPUBParser2k.h"
#include "ShapeType.h"
#include "libmspub_utils.h"
#include "MSPUBCollector.h"

libmspub::MSPUBParser2k::MSPUBParser2k(WPXInputStream *input, MSPUBCollector *collector)
  : MSPUBParser(input, collector),
    m_imageDataChunks()
{
}

libmspub::MSPUBParser2k::~MSPUBParser2k()
{
}

// Takes a line width specifier in Pub2k format and translates it into quarter points
unsigned short translateLineWidth(unsigned char lineWidth)
{
  if (lineWidth == 0x81)
  {
    return 0;
  }
  else if (lineWidth > 0x81)
  {
    return ((lineWidth - 0x81) / 3) * 4 + ((lineWidth - 0x81) % 3) + 1;
  }
  else
  {
    return lineWidth * 4;
  }
}

libmspub::Color libmspub::MSPUBParser2k::getColorBy2kHex(unsigned hex)
{
  switch ((hex >> 24) & 0xFF)
  {
  case 0x00:
    return getColorBy2kIndex(hex & 0xFF);
  case 0x20:
    return Color(hex & 0xFF, (hex >> 8) & 0xFF, (hex >> 16) & 0xFF);
  default:
    return Color();
  }
}

libmspub::Color libmspub::MSPUBParser2k::getColorBy2kIndex(unsigned char index)
{
  switch(index)
  {
  case 0x00:
    return Color(0, 0, 0);
  case 0x01:
    return Color(0xff, 0xff, 0xff);
  case 0x02:
    return Color(0xff, 0, 0);
  case 0x03:
    return Color(0, 0xff, 0);
  case 0x04:
    return Color(0, 0, 0xff);
  case 0x05:
    return Color(0xff, 0xff, 0);
  case 0x06:
    return Color(0, 0xff, 0xff);
  case 0x07:
    return Color(0xff, 0, 0xff);
  case 0x08:
    return Color(128, 128, 128);
  case 0x09:
    return Color(192, 192, 192);
  case 0x0A:
    return Color(128, 0, 0);
  case 0x0B:
    return Color(0, 128, 0);
  case 0x0C:
    return Color(0, 0, 128);
  case 0x0D:
    return Color(128, 128, 0);
  case 0x0E:
    return Color(0, 128, 128);
  case 0x0F:
    return Color(128, 0, 128);
  case 0x10:
    return Color(255, 153, 51);
  case 0x11:
    return Color(51, 0, 51);
  case 0x12:
    return Color(0, 0, 153);
  case 0x13:
    return Color(0, 153, 0);
  case 0x14:
    return Color(153, 153, 0);
  case 0x15:
    return Color(204, 102, 0);
  case 0x16:
    return Color(153, 0, 0);
  case 0x17:
    return Color(204, 153, 204);
  case 0x18:
    return Color(102, 102, 255);
  case 0x19:
    return Color(102, 255, 102);
  case 0x1A:
    return Color(255, 255, 153);
  case 0x1B:
    return Color(255, 204, 153);
  case 0x1C:
    return Color(255, 102, 102);
  case 0x1D:
    return Color(255, 153, 0);
  case 0x1E:
    return Color(0, 102, 255);
  case 0x1F:
    return Color(255, 204, 0);
  case 0x20:
    return Color(153, 0, 51);
  case 0x21:
    return Color(102, 51, 0);
  case 0x22:
    return Color(66, 66, 66);
  case 0x23:
    return Color(255, 153, 102);
  case 0x24:
    return Color(153, 51, 0);
  case 0x25:
    return Color(255, 102, 0);
  case 0x26:
    return Color(51, 51, 0);
  case 0x27:
    return Color(153, 204, 0);
  case 0x28:
    return Color(255, 255, 153);
  case 0x29:
    return Color(0, 51, 0);
  case 0x2A:
    return Color(51, 153, 102);
  case 0x2B:
    return Color(204, 255, 204);
  case 0x2C:
    return Color(0, 51, 102);
  case 0x2D:
    return Color(51, 204, 204);
  case 0x2E:
    return Color(204, 255, 255);
  case 0x2F:
    return Color(51, 102, 255);
  case 0x30:
    return Color(0, 204, 255);
  case 0x31:
    return Color(153, 204, 255);
  case 0x32:
    return Color(51, 51, 153);
  case 0x33:
    return Color(102, 102, 153);
  case 0x34:
    return Color(153, 51, 102);
  case 0x35:
    return Color(204, 153, 255);
  case 0x36:
    return Color(51, 51, 51);
  case 0x37:
    return Color(150, 150, 150);
  default:
    return Color();
  }
}

// takes a color reference in 2k format and translates it into 2k2 format that collector understands.
unsigned libmspub::MSPUBParser2k::translate2kColorReference(unsigned ref2k)
{
  switch ((ref2k >> 24) & 0xFF)
  {
  case 0xC0: //index into user palette
    return (ref2k & 0xFF) | (0x08 << 24);
  default:
  {
    Color c = getColorBy2kHex(ref2k);
    return (c.r) | (c.g << 8) | (c.b << 16);
  }
  }
}

//FIXME: Valek found different values; what does this depend on?
libmspub::ShapeType libmspub::MSPUBParser2k::getShapeType(unsigned char shapeSpecifier)
{
  switch (shapeSpecifier)
  {
  case 0x1:
    return RIGHT_TRIANGLE;
  /*
  case 0x2:
    return GENERAL_TRIANGLE;
  */
  case 0x3:
    return UP_ARROW;
  case 0x4:
    return STAR;
  case 0x5:
    return HEART;
  case 0x6:
    return ISOCELES_TRIANGLE;
  case 0x7:
    return PARALLELOGRAM;
  /* 
  case 0x8:
    return TILTED_TRAPEZOID;
  */
  case 0x9:
    return UP_DOWN_ARROW;
  case 0xA:
    return SEAL_16;
  case 0xB:
    return WAVE;
  case 0xC:
    return DIAMOND;
  case 0xD:
    return TRAPEZOID;
  case 0xE:
    return CHEVRON;
  case 0xF:
    return BENT_ARROW;
  case 0x10:
    return SEAL_24;
  /*
  case 0x11:
    return PIE;
  */
  case 0x12:
    return PENTAGON;
  case 0x13:
    return HOME_PLATE;
  /*
  case 0x14:
    return NOTCHED_TRIANGLE;
  */
  case 0x15:
    return U_TURN_ARROW;
  case 0x16:
    return IRREGULAR_SEAL_1;
  /*
  case 0x17:
    return CHORD;
  */
  case 0x18:
    return HEXAGON;
  /*
  case 0x19:
    return NOTCHED_RECTANGLE;
  */
  /*
  case 0x1A:
    return W_SHAPE; //This is a bizarre shape; the number of vertices depends on one of the adjust values.
                    //We need to refactor our escher shape drawing routines before we can handle it.
  */
  /*
  case 0x1B:
    return ROUND_RECT_CALLOUT_2K; //This is not quite the same as the round rect. found in 2k2 and above.
  */
  case 0x1C:
    return IRREGULAR_SEAL_2;
  case 0x1D:
    return BLOCK_ARC;
  case 0x1E:
    return OCTAGON;
  case 0x1F:
    return PLUS;
  case 0x20:
    return CUBE;
  /*
  case 0x21:
    return OVAL_CALLOUT_2K; //Not sure yet if this is the same as the 2k2 one.
  */
  case 0x22:
    return LIGHTNING_BOLT;
  default:
    return UNKNOWN_SHAPE;
  }
}

bool libmspub::MSPUBParser2k::parseContents(WPXInputStream *input)
{
  input->seek(0x16, WPX_SEEK_SET);
  unsigned trailerOffset = readU32(input);
  input->seek(trailerOffset, WPX_SEEK_SET);
  unsigned numBlocks = readU16(input);
  ContentChunkReference *last = NULL;
  unsigned chunkOffset = 0;
  for (unsigned i = 0; i < numBlocks; ++i)
  {
    input->seek(input->tell() + 2, WPX_SEEK_SET);
    unsigned short id = readU16(input);
    unsigned short parent = readU16(input);
    chunkOffset = readU32(input);
    if (last)
    {
      last->end = chunkOffset;
    }
    unsigned offset = input->tell();
    input->seek(chunkOffset, WPX_SEEK_SET);
    unsigned short typeMarker = readU16(input);
    input->seek(offset, WPX_SEEK_SET);
    switch (typeMarker)
    {
    case 0x0014:
      m_pageChunks.push_back(ContentChunkReference(PAGE, chunkOffset, 0, id, parent));
      last = &(m_pageChunks.back());
      break;
    case 0x0015:
      m_documentChunk = ContentChunkReference(DOCUMENT, chunkOffset, 0, id, parent);
      last = &m_documentChunk;
      m_seenDocumentChunk = true;
      break;
    case 0x0002:
      m_shapeChunks.push_back(ContentChunkReference(IMAGE_2K, chunkOffset, 0, id, parent));
      last = &(m_shapeChunks.back());
      break;
    case 0x0021:
      m_imageDataChunks.push_back(ContentChunkReference(IMAGE_2K_DATA, chunkOffset, 0, id, parent));
      last = &(m_imageDataChunks.back());
      break;
    case 0x0005:
    case 0x0006:
    case 0x0007:
    case 0x0008:
      m_shapeChunks.push_back(ContentChunkReference(SHAPE, chunkOffset, 0, id, parent));
      last = &(m_shapeChunks.back());
      break;
    case 0x0047:
      m_paletteChunks.push_back(ContentChunkReference(PALETTE, chunkOffset, 0, id, parent));
      last = &(m_paletteChunks.back());
      break;
    default:
      m_unknownChunks.push_back(ContentChunkReference(UNKNOWN_CHUNK, chunkOffset, 0, id, parent));
      last = &(m_unknownChunks.back());
      break;
    }
  }
  if (last)
  {
    last->end = chunkOffset;
  }
  if (! m_seenDocumentChunk)
  {
    MSPUB_DEBUG_MSG(("No document chunk found.\n"));
    return false;
  }
  input->seek(m_documentChunk.offset, WPX_SEEK_SET);
  input->seek(0x14, WPX_SEEK_CUR);
  unsigned width = readU32(input);
  unsigned height = readU32(input);
  m_collector->setWidthInEmu(width);
  m_collector->setHeightInEmu(height);

  for (ccr_iterator_t iter = m_paletteChunks.begin(); iter != m_paletteChunks.end(); ++iter)
  {
    input->seek(iter->offset, WPX_SEEK_SET);
    input->seek(0xA0, WPX_SEEK_CUR);
    for (unsigned i = 0; i < 8; ++i)
    {
      unsigned hex = readU32(input);
      Color color = getColorBy2kHex(hex);
      m_collector->addPaletteColor(color);
    }
  }

  for (ccr_iterator_t iter = m_imageDataChunks.begin(); iter != m_imageDataChunks.end(); ++iter)
  {
    input->seek(iter->offset + 4, WPX_SEEK_SET);
    unsigned toRead = readU32(input);
    WPXBinaryData img;
    while (toRead > 0 && stillReading(input, (unsigned long)-1))
    {
      unsigned long howManyRead = 0;
      const unsigned char *buf = input->read(toRead, howManyRead);
      img.append(buf, howManyRead);
      toRead -= howManyRead;
    }
    m_collector->addImage(++m_lastAddedImage, WMF, img);
  }

  for (ccr_iterator_t iter = m_shapeChunks.begin(); iter != m_shapeChunks.end(); ++iter)
  {
    input->seek(iter->offset, WPX_SEEK_SET);
    ccr_iterator_t i_page = std::find_if(m_pageChunks.begin(), m_pageChunks.end(), FindBySeqNum(iter->parentSeqNum));
    if (i_page == m_pageChunks.end())
    {
      continue;
    }
    if (getPageTypeBySeqNum(i_page->seqNum) != NORMAL)
    {
      continue;
    }
    // if the parent of this is a page and hasn't yet been added, then add it.
    if (!m_collector->hasPage(iter->parentSeqNum))
    {
      m_collector->addPage(iter->parentSeqNum);
    }
    m_collector->addShape(iter->seqNum, iter->parentSeqNum);
    unsigned short typeMarker = readU16(input);
    bool isImage = false;
    bool isRectangle = false;
    switch (typeMarker)
    {
    case 0x0002:
      isImage = true;
      m_collector->setShapeType(iter->seqNum, RECTANGLE);
      isRectangle = true;
      break;
    case 0x0005:
      m_collector->setShapeType(iter->seqNum, RECTANGLE);
      isRectangle = true;
      break;
    case 0x0006:
    {
      input->seek(iter->offset + 0x31, WPX_SEEK_SET);
      ShapeType shapeType = getShapeType(readU8(input));
      if (shapeType != UNKNOWN_SHAPE)
      {
        m_collector->setShapeType(iter->seqNum, shapeType);
      }
    }
    break;
    case 0x0007:
      m_collector->setShapeType(iter->seqNum, ELLIPSE);
      break;
    case 0x0008:
    {
      m_collector->setShapeType(iter->seqNum, RECTANGLE);
      isRectangle = true;
      input->seek(iter->offset + 0x58, WPX_SEEK_SET);
      unsigned txtId = readU32(input);
      m_collector->addTextShape(txtId, iter->seqNum, iter->parentSeqNum);
    }
    break;
    default:
      break;
    }
    input->seek(iter->offset + 6, WPX_SEEK_SET);
    int xs = readS32(input);
    int ys = readS32(input);
    int xe = readS32(input);
    int ye = readS32(input);
    m_collector->setShapeCoordinatesInEmu(iter->seqNum, xs, ys, xe, ye);
    if (isImage)
    {
      std::vector<ContentChunkReference>::const_iterator i_dataIndex =
        std::find_if(m_imageDataChunks.begin(), m_imageDataChunks.end(), FindByParentSeqNum(iter->seqNum));
      if (i_dataIndex != m_imageDataChunks.end())
      {
        m_collector->setShapeImgIndex(iter->seqNum, 1 + i_dataIndex - m_imageDataChunks.begin());
      }
    }
    else
    {
      input->seek(iter->offset + 0x2A, WPX_SEEK_SET);
      unsigned char fillType = readU8(input);
      if (fillType == 2) // other types are gradients and patterns which are not implemented yet. 0 is no fill.
      {
        input->seek(iter->offset + 0x22, WPX_SEEK_SET);
        unsigned fillColorReference = readU32(input);
        unsigned translatedFillColorReference = translate2kColorReference(fillColorReference);
        m_collector->setShapeFill(iter->seqNum, new SolidFill(ColorReference(translatedFillColorReference), 1, m_collector), false);
      }
    }
    input->seek(iter->offset + 0x2C, WPX_SEEK_SET);
    unsigned short leftLineWidth = readU8(input);
    bool leftLineExists = leftLineWidth != 0;
    unsigned leftColorReference = readU32(input);
    unsigned translatedLeftColorReference = translate2kColorReference(leftColorReference);
    if (isRectangle)
    {
      input->seek(4, WPX_SEEK_CUR);
      unsigned char topLineWidth = readU8(input);
      bool topLineExists = topLineWidth != 0;
      unsigned topColorReference = readU32(input);
      unsigned translatedTopColorReference = translate2kColorReference(topColorReference);
      m_collector->addShapeLine(iter->seqNum, Line(ColorReference(translatedTopColorReference),
                                translateLineWidth(topLineWidth) * EMUS_IN_INCH / (4 * POINTS_IN_INCH), topLineExists));

      input->seek(1, WPX_SEEK_CUR);
      unsigned char rightLineWidth = readU8(input);
      bool rightLineExists = rightLineWidth != 0;
      unsigned rightColorReference = readU32(input);
      unsigned translatedRightColorReference = translate2kColorReference(rightColorReference);
      m_collector->addShapeLine(iter->seqNum, Line(ColorReference(translatedRightColorReference),
                                translateLineWidth(rightLineWidth) * EMUS_IN_INCH / (4 * POINTS_IN_INCH), rightLineExists));

      input->seek(1, WPX_SEEK_CUR);
      unsigned char bottomLineWidth = readU8(input);
      bool bottomLineExists = bottomLineWidth != 0;
      unsigned bottomColorReference = readU32(input);
      unsigned translatedBottomColorReference = translate2kColorReference(bottomColorReference);
      m_collector->addShapeLine(iter->seqNum, Line(ColorReference(translatedBottomColorReference),
                                translateLineWidth(bottomLineWidth) * EMUS_IN_INCH / (4 * POINTS_IN_INCH), bottomLineExists));
    }
    m_collector->addShapeLine(iter->seqNum, Line(ColorReference(translatedLeftColorReference),
                              translateLineWidth(leftLineWidth) * EMUS_IN_INCH / (4 * POINTS_IN_INCH), leftLineExists));
    m_collector->setShapeOrder(iter->seqNum);
  }
  return true;
}

bool libmspub::MSPUBParser2k::parse()
{
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
  {
    MSPUB_DEBUG_MSG(("Couldn't get contents stream.\n"));
    return false;
  }
  if (!parseContents(contents))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse contents stream.\n"));
    delete contents;
    return false;
  }
  WPXInputStream *quill = m_input->getDocumentOLEStream("Quill/QuillSub/CONTENTS");
  if (!quill)
  {
    MSPUB_DEBUG_MSG(("Couldn't get quill stream.\n"));
    return false;
  }
  if (!parseQuill(quill))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse quill stream.\n"));
    delete quill;
    return false;
  }
  return m_collector->go();
}

libmspub::PageType libmspub::MSPUBParser2k::getPageTypeBySeqNum(unsigned seqNum)
{
  switch(seqNum)
  {
  case 0x116:
  case 0x108:
  case 0x109:
  case 0x10B:
  case 0x10D:
  case 0x119:
    return DUMMY_PAGE;
  default:
    return NORMAL;
  }
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
