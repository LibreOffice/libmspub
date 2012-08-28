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
 * Copyright (C) 2012 Brennan Vincent <brennanv@email.arizona.edu>
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

#ifndef __LISTINFO_H__
#define __LISTINFO_H__

#include <boost/optional.hpp>
#include <boost/cstdint.hpp>

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
