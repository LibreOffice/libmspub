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

#include <boost/ptr_container/ptr_map.hpp>

#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

#include "MSPUBTypes.h"
#include "libmspub_utils.h"
#include "MSPUBContentChunkType.h"
#include "ShapeType.h"
#include "Fill.h"

namespace libmspub
{

class MSPUBCollector
{
  friend class Shape;
  friend class TextShape;
  friend class GeometricShape;
  friend class Fill;
  friend class ImgFill;
  friend class SolidFill;
  friend class GradientFill;
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

  bool setShapeType(unsigned seqNum, ShapeType type);
  bool setShapeCoordinatesInEmu(unsigned seqNum, int xs, int ys, int xe, int ye);
  bool setShapeImgIndex(unsigned seqNum, unsigned index);
  bool setShapeLineColor(unsigned seqNum, unsigned line);
  bool setShapeFill(unsigned seqNum, Fill *fill);

  void setWidthInEmu(unsigned long);
  void setHeightInEmu(unsigned long);

  void addTextColor(unsigned c);
  void setDefaultColor(unsigned char r, unsigned char g, unsigned char b);
  void addFont(std::vector<unsigned char> name);

  void addDefaultCharacterStyle(const CharacterStyle &style);
  void addDefaultParagraphStyle(const ParagraphStyle &style);
  void addPaletteColor(Color);

  bool go();
private:

  struct Coordinate
  {
    Coordinate(int xs, int ys, int xe, int ye) : xs(xs), ys(ys), xe(xe), ye(ye) { }
    Coordinate() : xs(0), ys(0), xe(0), ye(0) { }
    int xs, ys, xe, ye;
  };
  struct Shape
  {
    Shape(MSPUBCollector *o) : props(), graphicsProps(), owner(o) { }
    void output(libwpg::WPGPaintInterface *painter, Coordinate coord);
    virtual ~Shape()
    {
    }
    WPXPropertyList props;
    WPXPropertyList graphicsProps;
  protected:
    virtual void setCoordProps(Coordinate coord);
    virtual void write(libwpg::WPGPaintInterface *painter) = 0;
    MSPUBCollector *owner;

    virtual WPXPropertyListVector updateGraphicsProps();

    Shape() : props(), graphicsProps(), owner(NULL) { }
  private:
    Shape(const Shape &) : props(), graphicsProps(), owner(NULL) { }
    Shape &operator=(const Shape &)
    {
      return *this;
    }
  };
  struct TextShape : public Shape
  {
    TextShape(std::vector<TextParagraph> s, MSPUBCollector *o) : Shape(o), str(s) { }
    std::vector<TextParagraph> str;
  protected:
    void write(libwpg::WPGPaintInterface *painter);
  private:
    TextShape(const TextShape &) : str() { }
    TextShape &operator=(const TextShape &)
    {
      return *this;
    }
  };
  struct GeometricShape : public Shape
  {
    GeometricShape(unsigned psn, MSPUBCollector *o) : Shape(o), pageSeqNum(psn), imgIndex(0), type(RECTANGLE), line(0x08000000), fill(NULL) { }
    unsigned pageSeqNum;
    unsigned imgIndex;
    ShapeType type;
    unsigned line;
    Fill *fill;
    void setLine(unsigned line);
    void setFill(Fill *fill);
  protected:
    void setCoordProps(Coordinate coord);
    virtual void write(libwpg::WPGPaintInterface *painter);
    WPXPropertyListVector updateGraphicsProps();
    GeometricShape() : pageSeqNum(0), imgIndex(0), type(RECTANGLE), line(0x08000000), fill(NULL) { }
  private:
    GeometricShape(const GeometricShape &) : pageSeqNum(0), imgIndex(0), type(RECTANGLE), line(0x08000000), fill(NULL) { }
    GeometricShape &operator=(const GeometricShape &)
    {
      return *this;
    }
  };
  struct ImgShape : public GeometricShape
  {
    ImgShape(const GeometricShape &from, ImgType imgType, WPXBinaryData i, MSPUBCollector *o);
    ImgShape(ImgType type, WPXBinaryData i, WPXPropertyList p, unsigned psn, MSPUBCollector *o) : GeometricShape(psn, o), img(i)
    {
      setMime_(type);
    }
    WPXBinaryData img;
    static const char *mimeByImgType(ImgType type);
  protected:
    virtual void write(libwpg::WPGPaintInterface *painter);
  private:
    void setMime_(ImgType type);
    ImgShape(const ImgShape &) : img() { }
    ImgShape &operator=(const ImgShape &)
    {
      return *this;
    }
  };
  struct PageInfo
  {
    PageInfo() : shapeSeqNums() { }
    std::vector<unsigned> shapeSeqNums;
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
  boost::ptr_map<unsigned, Shape> shapesBySeqNum; // boost::ptr_map is used instead of std::map to support Shape polymorphism
  std::vector<std::pair<ImgType, WPXBinaryData> > images;
  std::vector<unsigned> textColors;
  Color defaultColor;
  std::vector<std::vector<unsigned char> > fonts;
  std::vector<CharacterStyle> defaultCharStyles;
  std::vector<ParagraphStyle> defaultParaStyles;
  std::map<unsigned, ShapeType> shapeTypesBySeqNum;
  std::vector<unsigned> possibleImageShapeSeqNums;
  std::map<unsigned, unsigned> shapeImgIndicesBySeqNum;
  std::map<unsigned, Coordinate> shapeCoordinatesBySeqNum;
  std::map<unsigned, unsigned > shapeLineColorsBySeqNum;
  boost::ptr_map<unsigned, Fill> shapeFillsBySeqNum;
  std::vector<Color> paletteColors;

  // helper functions
  void assignImages();
  void setRectCoordProps(Coordinate, WPXPropertyList *);
  void setEllipseCoordProps(Coordinate, WPXPropertyList *);
  WPXPropertyList getCharStyleProps(const CharacterStyle &, unsigned defaultCharStyleIndex);
  WPXPropertyList getParaStyleProps(const ParagraphStyle &, unsigned defaultParaStyleIndex);
  static WPXString getColorString(const Color &);
  Color getColorByReference(unsigned c) const;
};

} // namespace libmspub

#endif /* __MSPUBCOLLECTOR_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
