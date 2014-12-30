/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __MSPUBMETADATA_H__
#define __MSPUBMETADATA_H__

#include <vector>
#include <utility>
#include <map>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include "libmspub_utils.h"

namespace libmspub
{

class MSPUBMetaData
{
public:
  MSPUBMetaData();
  ~MSPUBMetaData();
  bool parse(librevenge::RVNGInputStream *input);
  bool parseTimes(librevenge::RVNGInputStream *input);
  const librevenge::RVNGPropertyList &getMetaData();

private:
  MSPUBMetaData(const MSPUBMetaData &);
  MSPUBMetaData &operator=(const MSPUBMetaData &);

  void readPropertySetStream(librevenge::RVNGInputStream *input);
  void readPropertySet(librevenge::RVNGInputStream *input, uint32_t offset);
  void readPropertyIdentifierAndOffset(librevenge::RVNGInputStream *input);
  void readTypedPropertyValue(librevenge::RVNGInputStream *input, uint32_t index, uint32_t offset);
  librevenge::RVNGString readCodePageString(librevenge::RVNGInputStream *input);

  uint32_t getCodePage();

  std::vector< std::pair<uint32_t, uint32_t> > m_idsAndOffsets;
  std::map<uint16_t, uint16_t> m_typedPropertyValues;
  librevenge::RVNGPropertyList m_metaData;
};

} // namespace libmspub

#endif // __MSPUBMETADATA_H__

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
