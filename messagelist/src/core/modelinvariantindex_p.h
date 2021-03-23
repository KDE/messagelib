/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/modelinvariantindex.h"

namespace MessageList
{
namespace Core
{
class ModelInvariantIndex::Private
{
public:
    int mModelIndexRow; ///< The row that this index referenced at the time it was emitted
    uint mRowMapperSerial; ///< The serial that was current in the RowMapper at the time the invariant index was emitted
    ModelInvariantRowMapper *mRowMapper = nullptr; ///< The mapper that this invariant index is attached to

    int modelIndexRow() const
    {
        return mModelIndexRow;
    }

    uint rowMapperSerial() const
    {
        return mRowMapperSerial;
    }

    void setModelIndexRowAndRowMapperSerial(int modelIndexRow, uint rowMapperSerial)
    {
        mModelIndexRow = modelIndexRow;
        mRowMapperSerial = rowMapperSerial;
    }

    ModelInvariantRowMapper *rowMapper() const
    {
        return mRowMapper;
    }

    void setRowMapper(ModelInvariantRowMapper *mapper)
    {
        mRowMapper = mapper;
    }
};
} // namespace Core
} // namespace MessageList

