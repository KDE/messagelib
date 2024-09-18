/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_private_export.h"
#include <QStringList>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineCommand
{
public:
    SearchLineCommand();
    ~SearchLineCommand();
    void parseSearchLineCommand(const QString &str);
    [[nodiscard]] QString dump() const;

private:
    enum {
        To = 0,
        Bcc,
        From,
        Subject,
        Date,
        Size,
    };
    static QStringList mKeyList;
};
}
}
