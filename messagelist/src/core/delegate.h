/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/themedelegate.h"

namespace MessageList
{
namespace Core
{
class View;

class Delegate : public ThemeDelegate
{
    Q_OBJECT
public:
    explicit Delegate(View *pParent);
    ~Delegate() override;

protected:
    /**
     * Returns the Item for the specified model index. Reimplemented from ThemeDelegate.
     */
    Q_REQUIRED_RESULT Item *itemFromIndex(const QModelIndex &index) const override;
};
} // namespace Core
} // namespace MessageList

