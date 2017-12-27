/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LISTINFO_H
#define INCLUDED_LISTINFO_H

#include <boost/optional.hpp>

#include "NumberingDelimiter.h"
#include "NumberingType.h"

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

#endif /* INCLUDED_LISTINFO_H */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
