/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>
#include <libmspub/libmspub.h>

namespace libmspub
{
class MSPUBStringVectorImpl
{
public:
  MSPUBStringVectorImpl() : m_strings() {}
  ~MSPUBStringVectorImpl() {}
  std::vector<librevenge::RVNGString> m_strings;
};

MSPUBStringVector::MSPUBStringVector()
  : m_pImpl(new MSPUBStringVectorImpl())
{
}

MSPUBStringVector::MSPUBStringVector(const MSPUBStringVector &vec)
  : m_pImpl(new MSPUBStringVectorImpl(*(vec.m_pImpl)))
{
}

MSPUBStringVector::~MSPUBStringVector()
{
  delete m_pImpl;
}

MSPUBStringVector &MSPUBStringVector::operator=(const MSPUBStringVector &vec)
{
  // Check for self-assignment
  if (this == &vec)
    return *this;
  if (m_pImpl)
    delete m_pImpl;
  m_pImpl = new MSPUBStringVectorImpl(*(vec.m_pImpl));
  return *this;
}

unsigned MSPUBStringVector::size() const
{
  return (unsigned)(m_pImpl->m_strings.size());
}

bool MSPUBStringVector::empty() const
{
  return m_pImpl->m_strings.empty();
}

const librevenge::RVNGString &MSPUBStringVector::operator[](unsigned idx) const
{
  return m_pImpl->m_strings[idx];
}

void MSPUBStringVector::append(const librevenge::RVNGString &str)
{
  m_pImpl->m_strings.push_back(str);
}

void MSPUBStringVector::clear()
{
  m_pImpl->m_strings.clear();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
