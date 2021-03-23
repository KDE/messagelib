/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <qglobal.h> // defines uint, at least.

namespace MessageList
{
namespace Core
{
class ModelInvariantRowMapper;
class ModelInvariantRowMapperPrivate;
class RowShift;

/**
 * An invariant index that can be ALWAYS used to reference
 * an item inside a QAbstractItemModel.
 *
 * This class is meant to be used together with ModelInvariantRowMapper.
 */
class ModelInvariantIndex
{
    friend class ModelInvariantRowMapper;
    friend class ModelInvariantRowMapperPrivate;
    friend class RowShift;

public:
    explicit ModelInvariantIndex();
    virtual ~ModelInvariantIndex();

public:
    /**
     * Returns true if this ModelInvariantIndex is valid, that is, it has been attached
     * to a ModelInvariantRowMapper. Returns false otherwise.
     * An invalid index will always map to the current row -1 (which is invalid as QModelIndex row).
     */
    Q_REQUIRED_RESULT bool isValid() const;

    /**
     * Returns the current model index row for this invariant index. This function
     * calls the mapper and asks it to perform the persistent mapping.
     * If this index isn't valid then the returned value is -1.
     *
     * If you actually own the row mapper then you may save some clock cycles
     * by calling the modelInvariantIndexToModelIndexRow() by your own. If you don't
     * own the mapper then this function is the only way to go.
     */
    Q_REQUIRED_RESULT int currentModelIndexRow();

private:
    class Private;
    Private *const d;
};
} // namespace Core
} // namespace MessageList

