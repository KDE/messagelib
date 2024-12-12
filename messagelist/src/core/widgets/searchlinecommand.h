/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_export.h"
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QStringList>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_EXPORT SearchLineCommand
{
    Q_GADGET
public:
    enum SearchLineType : uint8_t {
        Unknown = 0,
        HasStateOrAttachment,
        Literal,
        To,
        Bcc,
        Cc,
        From,
        Subject,
        Body,
        Date,
        Size,
        HasAttachment,
        HasInvitation,
        IsImportant,
        IsRead,
        IsUnRead,
        IsIgnored,
        IsHam,
        IsSpam,
        IsWatched,
        IsReplied,
        IsForwarded,
        Larger,
        Smaller,
        OlderThan,
        NewerThan,
        Category,
    };
    Q_ENUM(SearchLineType)
    struct MESSAGELIST_EXPORT SearchLineInfo {
        SearchLineType type = SearchLineType::Unknown;
        QString argument;
        void clear();
        [[nodiscard]] bool isValid() const;
        [[nodiscard]] bool operator==(const SearchLineInfo &other) const;
        [[nodiscard]] bool isValidDate() const;
        [[nodiscard]] bool mustBeUnique() const;
        [[nodiscard]] qint64 convertArgumentAsSize() const;
    };
    SearchLineCommand();
    ~SearchLineCommand();

    void parseSearchLineCommand(const QString &str);

    [[nodiscard]] QString generateCommadLineStr() const;

    [[nodiscard]] QList<SearchLineInfo> searchLineInfo() const;
    void setSearchLineInfo(const QList<SearchLineInfo> &newSearchLineInfo);

    [[nodiscard]] static bool hasSubType(const QString &v);
    [[nodiscard]] static bool hasSubType(SearchLineCommand::SearchLineType type);

    [[nodiscard]] bool isEmpty() const;

private:
    [[nodiscard]] MESSAGELIST_NO_EXPORT QString convertSearchLinetypeToTranslatedString(SearchLineCommand::SearchLineType type) const;
    [[nodiscard]] MESSAGELIST_NO_EXPORT SearchLineInfo isAnotherInfo(QString tmp, SearchLineInfo searchLineInfo);
    MESSAGELIST_NO_EXPORT void appendSearchLineInfo(SearchLineInfo searchLineInfo);
    static QMap<QString, SearchLineCommand::SearchLineType> mKeyList;
    QList<SearchLineInfo> mSearchLineInfo;
};
}
}
Q_DECLARE_TYPEINFO(MessageList::Core::SearchLineCommand::SearchLineInfo, Q_RELOCATABLE_TYPE);
MESSAGELIST_EXPORT QDebug operator<<(QDebug debug, const MessageList::Core::SearchLineCommand::SearchLineInfo &info);
