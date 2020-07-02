/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef QUICKSEARCHWARNING_H
#define QUICKSEARCHWARNING_H

#include <KMessageWidget>
#include "messagelist_private_export.h"
namespace MessageList {
namespace Core {
class MESSAGELIST_TESTS_EXPORT QuickSearchWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit QuickSearchWarning(QWidget *parent = nullptr);
    ~QuickSearchWarning();
    void setSearchText(const QString &text);

private Q_SLOTS:
    void slotDoNotRememberIt();
};
}
}
#endif // QUICKSEARCHWARNING_H
