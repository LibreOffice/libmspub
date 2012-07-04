#ifndef __SHAPEGROUPPAINTER_H__
#define __SHAPEGROUPPAINTER_H__

#include <libwpd/WPXPropertyList.h>

namespace libmspub
{
class MSPUBCollector;
class ShapeGroupPainter
{
  MSPUBCollector *m_owner;
public:
  ShapeGroupPainter(MSPUBCollector *owner) : m_owner(owner)
  {
  }
  void group(WPXPropertyList layerProperties);
  void shape(unsigned seqNum);
  void endGroup();
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
