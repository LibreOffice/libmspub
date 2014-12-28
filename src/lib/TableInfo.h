/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __TABLEINFO_H__
#define __TABLEINFO_H__

#include <vector>

namespace libmspub
{

struct CellInfo
{
  CellInfo()
    : m_startRow()
    , m_endRow()
    , m_startColumn()
    , m_endColumn()
  {
  }

  unsigned m_startRow;
  unsigned m_endRow;
  unsigned m_startColumn;
  unsigned m_endColumn;
};

struct TableInfo
{
  std::vector<unsigned> m_rowOffsetsInEmu;
  std::vector<unsigned> m_columnOffsetsInEmu;
  unsigned m_numRows;
  unsigned m_numColumns;
  std::vector<CellInfo> m_cells;
  TableInfo(unsigned numRows, unsigned numColumns) : m_rowOffsetsInEmu(),
    m_columnOffsetsInEmu(), m_numRows(numRows), m_numColumns(numColumns),
    m_cells()
  {
  }
};
} // namespace libmspub

#endif /* __TABLEINFO_H__ */
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
