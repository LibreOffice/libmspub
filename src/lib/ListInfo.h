/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __LISTINFO_H__
#define __LISTINFO_H__

#include <boost/optional.hpp>

#include "NumberingType.h"
#include "NumberingDelimiter.h"

namespace libmspub
{
enum ListType
{
  ORDERED,
  UNORDERED
};
struct ListInfo
{
  ListType m_listType;

  //unordered list stuff
  boost::optional<unsigned> m_bulletChar;
  ListInfo(unsigned bulletChar) : m_listType(UNORDERED),
    m_bulletChar(bulletChar), m_numberIfRestarted(),
    m_numberingType(), m_numberingDelimiter()
  {
  }

  //ordered list stuff
  boost::optional<unsigned> m_numberIfRestarted;
  boost::optional<NumberingType> m_numberingType;
  boost::optional<NumberingDelimiter> m_numberingDelimiter;
  ListInfo(boost::optional<unsigned> numberIfRestarted, NumberingType numberingType,
           NumberingDelimiter numberingDelimiter)
    : m_listType(ORDERED), m_bulletChar(),
      m_numberIfRestarted(numberIfRestarted), m_numberingType(numberingType),
      m_numberingDelimiter(numberingDelimiter)
  {
  }
};
} // namespace libmspub

#endif /* __LISTINFO_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
