/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#ifndef MESSAGELIST_CORE_GROUPHEADERITEM_H
#define MESSAGELIST_CORE_GROUPHEADERITEM_H

#include <QString>

#include "core/item.h"

namespace MessageList {
namespace Core {
class GroupHeaderItem : public Item
{
public:
    explicit GroupHeaderItem(const QString &label);
    ~GroupHeaderItem() override;

    Q_REQUIRED_RESULT const QString &label() const;
    void setLabel(const QString &label);

private:
    QString mLabel;
};
} // namespace Core
} // namespace MessageList

#endif //!__MESSAGELIST_CORE_GROUPHEADERITEM_H
