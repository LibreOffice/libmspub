#ifndef __SHAPEGROUPPAINTER_H__
#define __SHAPEGROUPPAINTER_H__

#include <libwpd/libwpd.h>

namespace libmspub
{
class MSPUBCollector;
class ShapeGroup;
class ShapeGroupElementLeaf;
class ShapeGroupVisitor
{
public:
  virtual void group(ShapeGroup *group) = 0;
  virtual void shape(ShapeGroupElementLeaf *leaf) = 0;
  virtual void endGroup() = 0;
  virtual ~ShapeGroupVisitor() { }
};
class ShapeGroupPainter : public ShapeGroupVisitor
{
  MSPUBCollector *m_owner;
  ShapeGroupPainter &operator=(const ShapeGroupPainter &);
  ShapeGroupPainter(const ShapeGroupPainter &);
public:
  ShapeGroupPainter(MSPUBCollector *owner) : m_owner(owner)
  {
  }
  void group(ShapeGroup *group);
  void shape(ShapeGroupElementLeaf *leaf);
  void endGroup();
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
