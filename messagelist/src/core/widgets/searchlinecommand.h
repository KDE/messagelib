/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_private_export.h"
#include <QObject>
#include <QStringList>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineCommand
{
    Q_GADGET
public:
    enum SearchLineType {
        Unknown = 0,
        To,
        Bcc,
        From,
        Subject,
        Date,
        Size,
    };
    Q_ENUM(SearchLineType)
    struct SearchLineInfo {
        SearchLineType type = SearchLineType::Unknown;
        QString argument;
        [[nodiscard]] bool isValid() const;
    };
    SearchLineCommand();
    ~SearchLineCommand();
    void parseSearchLineCommand(const QString &str);
    [[nodiscard]] QString dump() const;

private:
    static QStringList mKeyList;
};
}
}
Q_DECLARE_TYPEINFO(MessageList::Core::SearchLineCommand::SearchLineInfo, Q_RELOCATABLE_TYPE);
