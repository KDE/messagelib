/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_private_export.h"
#include <QDebug>
#include <QMap>
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
        HasStateOrAttachment,
        Literal,
        To,
        Bcc,
        Cc,
        From,
        Subject,
        Date,
        Size,
        HasAttachment,
        IsImportant,
        IsRead,
        IsUnRead,
    };
    Q_ENUM(SearchLineType)
    struct SearchLineInfo {
        SearchLineType type = SearchLineType::Unknown;
        QString argument;
        void clear();
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] bool operator==(const SearchLineInfo &other) const;
    };
    SearchLineCommand();
    ~SearchLineCommand();

    void parseSearchLineCommand(const QString &str);
    [[nodiscard]] QString dump() const;

    [[nodiscard]] QList<SearchLineInfo> searchLineInfo() const;
    void setSearchLineInfo(const QList<SearchLineInfo> &newSearchLineInfo);

private:
    [[nodiscard]] SearchLineCommand::SearchLineType convertStringToSearchLineType(const QString &str) const;
    static QMap<QString, SearchLineCommand::SearchLineType> mKeyList;
    QList<SearchLineInfo> mSearchLineInfo;
};
}
}
Q_DECLARE_TYPEINFO(MessageList::Core::SearchLineCommand::SearchLineInfo, Q_RELOCATABLE_TYPE);
MESSAGELIST_TESTS_EXPORT QDebug operator<<(QDebug debug, const MessageList::Core::SearchLineCommand::SearchLineInfo &info);
