/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <KMessageWidget>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT QuickSearchWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit QuickSearchWarning(QWidget *parent = nullptr);
    ~QuickSearchWarning() override;
    void setSearchText(const QString &text);

private Q_SLOTS:
    void slotDoNotRememberIt();
};
}
}
