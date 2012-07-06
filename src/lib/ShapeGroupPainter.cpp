#include "MSPUBCollector.h"
#include "ShapeGroup.h"
#include "ShapeGroupPainter.h"

void libmspub::ShapeGroupPainter::group(ShapeGroup * /*group*/)
{
  m_owner->m_painter->startLayer(WPXPropertyList());
}
void libmspub::ShapeGroupPainter::shape(ShapeGroupElementLeaf *leaf)
{
  unsigned seqNum = leaf->getFirstShapeSeqNum();
  Shape *myShape = ptr_getIfExists(m_owner->m_shapesBySeqNum, seqNum);
  if (myShape)
  {
    Coordinate *coord = getIfExists(m_owner->m_shapeCoordinatesBySeqNum, seqNum);
    if (coord)
    {
      GeometricShape *casted = dynamic_cast<GeometricShape *>(myShape);
      leaf->m_coordinates = *coord;
      if (casted)
      {
        casted->m_foldedTransform = leaf->getFoldedTransform(casted->m_transform);
      }
      myShape->output(m_owner->m_painter, *coord);
    }
    else
    {
      MSPUB_DEBUG_MSG(("Could not output shape of seqnum 0x%x: no coordinates provided\n", seqNum));
    }
  }
  else
  {
    MSPUB_DEBUG_MSG(("No shape with seqnum 0x%x found in collector\n", seqNum));
  }
}
void libmspub::ShapeGroupPainter::endGroup()
{
  m_owner->m_painter->endLayer();
}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
