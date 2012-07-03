#ifndef __SHAPEGROUP_H__
#define __SHAPEGROUP_H__
namespace libmspub
{
class ShapeGroup;
class ShapeGroupElement
{
public:
  ShapeGroup *m_parent;
  ShapeGroupElement(ShapeGroup *parent) : m_parent(parent)
  {
  }
  virtual unsigned getPageSeqNum() const = 0;
  virtual unsigned getFirstShapeSeqNum() const = 0;
  virtual void setPage(unsigned pageSeqNum) = 0;
  virtual void paint(ShapeGroupPainter p) = 0;
  virtual ~ShapeGroupElement()
  {
  }
private:
  ShapeGroupElement(const ShapeGroupElement &);
  ShapeGroupElement &operator=(const ShapeGroupElement &);
};
class ShapeGroup : public ShapeGroupElement
{
  ShapeGroup(const ShapeGroup &);
  ShapeGroup &operator=(const ShapeGroup &);
public:
  boost::ptr_vector<ShapeGroupElement> m_elements;
  ShapeGroup(ShapeGroup *parent) : ShapeGroupElement(parent), m_elements()
  {
  }
  unsigned getFirstShapeSeqNum() const
  {
    return (!m_elements.empty()) ? m_elements[0].getFirstShapeSeqNum() : 0;
  }
  unsigned getPageSeqNum() const
  {
    if (!m_elements.empty())
    {
      return m_elements[0].getPageSeqNum();
    }
    return 0;
  }
  void setPage(unsigned pageSeqNum)
  {
    for (unsigned i = 0; i < m_elements.size(); ++i)
    {
      m_elements[i].setPage(pageSeqNum);
    }
  }
  void paint(ShapeGroupPainter p)
  {
    p.group();
    for (unsigned i = 0; i < m_elements.size(); ++i)
    {
      m_elements[i].paint(p);
    }
    p.endGroup();
  }
};
class ShapeGroupElementLeaf : public ShapeGroupElement
{
  ShapeGroupElementLeaf(const ShapeGroupElementLeaf &);
  ShapeGroupElementLeaf &operator=(const ShapeGroupElementLeaf &);
  unsigned m_pageSeqNum;
  unsigned m_seqNum;
public:
  ShapeGroupElementLeaf(ShapeGroup *parent, unsigned seqNum = 0) : ShapeGroupElement(parent), m_pageSeqNum(0), m_seqNum(seqNum)
  {
  }
  unsigned getPageSeqNum() const
  {
    return m_pageSeqNum;
  }
  unsigned getFirstShapeSeqNum() const
  {
    return m_seqNum;
  }
  void setPage(unsigned pageSeqNum)
  {
    m_pageSeqNum = pageSeqNum;
  }
  void paint(ShapeGroupPainter p)
  {
    p.shape(m_seqNum);
  }
};
}
#endif
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */

