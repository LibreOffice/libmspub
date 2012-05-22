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

#ifndef __MSPUBCOLLECTOR_H__
#define __MSPUBCOLLECTOR_H__

#include <list>
#include <vector>
#include <map>
#include <string>
#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

#include "MSPUBTypes.h"
#include "libmspub_utils.h"
#include "MSPUBContentChunkType.h"

namespace libmspub
{

class MSPUBCollector
{
public:
  typedef std::list<ContentChunkReference>::const_iterator ccr_iterator_t;

  MSPUBCollector(::libwpg::WPGPaintInterface *painter);
  virtual ~MSPUBCollector();

  // collector functions
  bool addPage(unsigned seqNum);
  bool addTextString(const std::vector<TextParagraph> &str, unsigned id);
  bool addTextShape(unsigned stringId, unsigned seqNum, unsigned pageSeqNum);
  bool addImage(unsigned index, ImgType type, WPXBinaryData img);
  bool addShape(unsigned seqNum, unsigned pageSeqNum);

  bool setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye);
  bool setShapeImgIndex(unsigned seqNum, unsigned index);

  void setWidthInEmu(unsigned long);
  void setHeightInEmu(unsigned long);

  void addColor(unsigned char r, unsigned char g, unsigned char b);

  void setDefaultCharacterStyle(const CharacterStyle &style);

  bool go();
private:

  struct TextShapeInfo
  {
    TextShapeInfo(std::vector<TextParagraph> str) : str(str), props() { }
    std::vector<TextParagraph> str;
    WPXPropertyList props;
  };
  struct UnknownShapeInfo
  {
    UnknownShapeInfo(unsigned pageSeqNum) : pageSeqNum(pageSeqNum), imgIndex(0), props() { }
    unsigned pageSeqNum;
    unsigned imgIndex;
    WPXPropertyList props;
  };
  struct ImgShapeInfo
  {
    ImgShapeInfo(ImgType type, WPXBinaryData img, WPXPropertyList props) : img(img), props(props)
    {
      const char *mime;
      switch (type)
      {
      case PNG:
        mime = "image/png";
        break;
      case JPEG:
        mime = "image/jpeg";
        break;
      default:
        mime = "";
        MSPUB_DEBUG_MSG(("Unknown image type %d passed to ImgShapeInfo constructor!\n", type));
      }
      this->props.insert("libwpg:mime-type", mime);
    }

    WPXBinaryData img;
    WPXPropertyList props;
  };
  struct PageInfo
  {
    PageInfo() : textShapeReferences(), imgShapeReferences() { }
    std::vector<std::map<unsigned, TextShapeInfo>::const_iterator> textShapeReferences;
    std::vector<std::map<unsigned, ImgShapeInfo>::const_iterator> imgShapeReferences;
  };

  MSPUBCollector(const MSPUBCollector &);
  MSPUBCollector &operator=(const MSPUBCollector &);

  libwpg::WPGPaintInterface *m_painter;
  std::list<ContentChunkReference> contentChunkReferences;
  unsigned long m_width, m_height;
  bool m_widthSet, m_heightSet;
  WPXPropertyList m_commonPageProperties;
  unsigned short m_numPages;
  std::map<unsigned, std::vector<TextParagraph> > textStringsById;
  std::map<unsigned, PageInfo> pagesBySeqNum;
  std::map<unsigned, TextShapeInfo> textShapesBySeqNum;
  std::map<unsigned, ImgShapeInfo> imgShapesBySeqNum;
  std::vector<std::pair<ImgType, WPXBinaryData> > images;
  std::map<unsigned, UnknownShapeInfo> possibleImageShapes;
  std::vector<Color> colors;
  CharacterStyle defaultCharStyle;

  // helper functions
  void assignImages();
  WPXPropertyList getCharStyleProps(const CharacterStyle &);
  WPXPropertyList getParaStyleProps(const ParagraphStyle &);
  static WPXString getColorString(const Color &);
};

} // namespace libmspub

#endif /* __MSPUBCOLLECTOR_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
