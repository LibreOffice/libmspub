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

} // namespace libmspub

libmspub::MSPUBStringVector::MSPUBStringVector()
  : m_pImpl(new MSPUBStringVectorImpl())
{
}

libmspub::MSPUBStringVector::MSPUBStringVector(const MSPUBStringVector &vec)
  : m_pImpl(new MSPUBStringVectorImpl(*(vec.m_pImpl)))
{
}

libmspub::MSPUBStringVector::~MSPUBStringVector()
{
  delete m_pImpl;
}

libmspub::MSPUBStringVector &libmspub::MSPUBStringVector::operator=(const MSPUBStringVector &vec)
{
  // Check for self-assignment
  if (this == &vec)
    return *this;
  if (m_pImpl)
    delete m_pImpl;
  m_pImpl = new MSPUBStringVectorImpl(*(vec.m_pImpl));
  return *this;
}

unsigned libmspub::MSPUBStringVector::size() const
{
  return (unsigned)(m_pImpl->m_strings.size());
}

bool libmspub::MSPUBStringVector::empty() const
{
  return m_pImpl->m_strings.empty();
}

const librevenge::RVNGString &libmspub::MSPUBStringVector::operator[](unsigned idx) const
{
  return m_pImpl->m_strings[idx];
}

void libmspub::MSPUBStringVector::append(const librevenge::RVNGString &str)
{
  m_pImpl->m_strings.push_back(str);
}

void libmspub::MSPUBStringVector::clear()
{
  m_pImpl->m_strings.clear();
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
