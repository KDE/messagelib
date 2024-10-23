/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommand.h"
#include <KLocalizedString>
using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
QMap<QString, SearchLineCommand::SearchLineType> SearchLineCommand::mKeyList = {
    {"subject"_L1, SearchLineCommand::SearchLineType::Subject},
    {"to"_L1, SearchLineCommand::SearchLineType::To},
    {"cc"_L1, SearchLineCommand::SearchLineType::Cc},
    {"bcc"_L1, SearchLineCommand::SearchLineType::Bcc},
    {"from"_L1, SearchLineCommand::SearchLineType::From},
    {"has:attachment"_L1, SearchLineCommand::SearchLineType::HasAttachment},
    {"has:hasinvitation"_L1, SearchLineCommand::SearchLineType::HasInvitation},
    {"is:unread"_L1, SearchLineCommand::SearchLineType::IsUnRead},
    {"is:read"_L1, SearchLineCommand::SearchLineType::IsRead},
    {"is:important"_L1, SearchLineCommand::SearchLineType::IsImportant},
    {"is:ignored"_L1, SearchLineCommand::SearchLineType::IsIgnored},
    {"is:ham"_L1, SearchLineCommand::SearchLineType::IsHam},
    {"is:spam"_L1, SearchLineCommand::SearchLineType::IsSpam},
    {"is:watched"_L1, SearchLineCommand::SearchLineType::IsWatched},
    {"is:replied"_L1, SearchLineCommand::SearchLineType::IsReplied},
    {"is:forwarded"_L1, SearchLineCommand::SearchLineType::IsForwarded},
    {"size"_L1, SearchLineCommand::SearchLineType::Size},
    {"smaller"_L1, SearchLineCommand::SearchLineType::Larger},
    {"larger"_L1, SearchLineCommand::SearchLineType::Smaller},
    {"older_than"_L1, SearchLineCommand::SearchLineType::OlderThan},
    {"newer_than"_L1, SearchLineCommand::SearchLineType::NewerThan},
    // after:before:older:newer:
    // TODO add support for OR
};
SearchLineCommand::SearchLineCommand() = default;

SearchLineCommand::~SearchLineCommand() = default;

bool SearchLineCommand::hasSubType(const QString &v)
{
    return v == QLatin1StringView("is") || v == QLatin1StringView("has");
}

bool SearchLineCommand::hasSubType(SearchLineCommand::SearchLineType type)
{
    return type == Date || type == Size || type == To || type == Bcc || type == Cc || type == From || type == Subject || type == Smaller || type == Larger
        || type == OlderThan || type == NewerThan;
}

QString SearchLineCommand::convertSearchLinetypeToTranslatedString(SearchLineCommand::SearchLineType type) const
{
    switch (type) {
    case Unknown:
    case HasStateOrAttachment:
    case Literal:
        // TODO implement Date/Size
    case Date:
    case Smaller:
    case Larger:
    case OlderThan:
    case NewerThan:
        return {};
    case Size:
        return i18n("Size is");
    case To:
        return i18n("To contains");
    case Bcc:
        return i18n("BCC contains");
    case Cc:
        return i18n("CC contains");
    case From:
        return i18n("From contains");
    case Subject:
        return i18n("Subject contains");
    case HasAttachment:
        return i18n("Mail has attachment");
    case HasInvitation:
        return i18n("Mail has invitation");
    case IsImportant:
        return i18n("Mail is important");
    case IsRead:
        return i18n("Mail is read");
    case IsUnRead:
        return i18n("Mail is Unread");
    case IsIgnored:
        return i18n("Mail is Ignored");
    case IsHam:
        return i18n("Mail is Ham");
    case IsSpam:
        return i18n("Mail is Spam");
    case IsWatched:
        return i18n("Mail is watched");
    case IsReplied:
        return i18n("Mail is replied");
    case IsForwarded:
        return i18n("Mail is forwarded");
    }
    return {};
}

QString SearchLineCommand::generateCommadLineStr() const
{
    QString result;
    for (const auto &info : mSearchLineInfo) {
        if (!result.isEmpty()) {
            result += QLatin1Char(' ') + i18n("AND") + QLatin1Char(' ');
        }
        const QString translatedType = convertSearchLinetypeToTranslatedString(info.type);
        if (!translatedType.isEmpty()) {
            result += translatedType;
        }
        if (!info.argument.isEmpty()) {
            if (!translatedType.isEmpty()) {
                result += QLatin1Char(' ');
            }
            result += info.argument;
        }
    }
    return result;
}

#define DEBUG_COMMAND_PARSER 1

SearchLineCommand::SearchLineInfo SearchLineCommand::isAnotherInfo(QString tmp, SearchLineInfo searchLineInfo)
{
    if (!tmp.contains(QLatin1Char(' '))) {
        return {};
    }
    if (tmp.endsWith(QLatin1StringView("is")) || tmp.endsWith(QLatin1StringView("has"))) {
#ifdef DEBUG_COMMAND_PARSER
        qDebug() << " found has subtype " << tmp;
#endif
        return {};
    }
    const QStringList keys = mKeyList.keys();
    for (const QString &key : keys) {
        if (tmp.endsWith(key)) {
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " found element !!!!!! " << tmp;
#endif
            tmp.remove(key);
            tmp.removeLast(); // Remove last space
            searchLineInfo.argument = tmp;
            // qDebug() << " AAAAAAAAAAAAAAAAAAAAAAAAAAA " << searchLineInfo;
            if (searchLineInfo.isValid()) {
                mSearchLineInfo.append(searchLineInfo);
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " Add  searchLineInfo" << searchLineInfo;
#endif

            SearchLineInfo info;
            info.type = mKeyList.value(key);
            return info;
        }
    }
    return {};
}

void SearchLineCommand::parseSearchLineCommand(const QString &str)
{
    mSearchLineInfo.clear();
    if (str.isEmpty()) {
        return;
    }
    SearchLineInfo searchLineInfo;
    QString tmp;
    int parentheses = 0;
    for (int i = 0, total = str.length(); i < total; ++i) {
        const QChar ch = str.at(i);
        if (ch == QLatin1Char(':')) {
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " tmp ! " << tmp;
#endif
            const SearchLineCommand::SearchLineInfo newInfo = isAnotherInfo(tmp, searchLineInfo);
            if (newInfo.type != Unknown) {
                tmp.clear();
                searchLineInfo = newInfo;
                qDebug() << " vxvxcvxcvxcv " << tmp;
            } else if (mKeyList.contains(tmp.trimmed())) {
#ifdef DEBUG_COMMAND_PARSER
                qDebug() << " contains " << tmp;
#endif
                searchLineInfo.type = mKeyList.value(tmp.trimmed());
                tmp.clear();
            } else if (hasSubType(tmp)) {
                searchLineInfo.type = HasStateOrAttachment;
                tmp += ch;
                // continue
            } else {
                tmp += ch;
            }
        } else if (ch.isSpace()) {
            // We can use is:... or has:...
            if (mKeyList.contains(tmp)) {
                searchLineInfo.type = mKeyList.value(tmp);
                tmp.clear();
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " is space " << "pare" << parentheses << " tmp " << tmp << "searchLineInfo.type " << searchLineInfo.type
                     << " searchLineInfo.argument.isEmpty() " << searchLineInfo.argument.isEmpty();
#endif
            if (tmp.isEmpty() && hasSubType(searchLineInfo.type) && parentheses == 0) {
#ifdef DEBUG_COMMAND_PARSER
                qDebug() << "clear invalid type" << searchLineInfo;
#endif
                searchLineInfo.type = Unknown;
                tmp.clear();
            } else if (hasSubType(searchLineInfo.type)) {
                tmp += ch;
            } else if (searchLineInfo.type != Unknown && parentheses == 0) {
                searchLineInfo.argument = tmp;
                tmp.clear();
#ifdef DEBUG_COMMAND_PARSER
                qDebug() << "clear tmp argument " << searchLineInfo;
#endif
            } else { // Literal
                tmp += ch;
            }
            if (searchLineInfo.isValid() && parentheses == 0) {
                mSearchLineInfo.append(std::move(searchLineInfo));
                searchLineInfo.clear();
                tmp.clear();
            }
        } else if (ch == QLatin1Char('(')) {
            parentheses++;
            if (parentheses > 1) {
                tmp += ch;
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " parenthese ( equal " << parentheses;
#endif
        } else if (ch == QLatin1Char(')')) {
            parentheses--;
            if (parentheses > 0) {
                tmp += ch;
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " parenthese ) equal " << parentheses;
#endif
            if (parentheses == 0) {
                searchLineInfo.argument = tmp;
                tmp.clear();
#ifdef DEBUG_COMMAND_PARSER
                qDebug() << " new values " << searchLineInfo;
#endif
                mSearchLineInfo.append(std::move(searchLineInfo));
                searchLineInfo.clear();
            }
        } else {
            tmp += ch;
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " tmp " << tmp << " ch " << ch << "end";
#endif
        }
    }
    if (searchLineInfo.type != Unknown) {
        if (searchLineInfo.type == HasStateOrAttachment) {
            if (mKeyList.contains(tmp)) {
                searchLineInfo.type = mKeyList.value(tmp);
                mSearchLineInfo.append(std::move(searchLineInfo));
            }
        } else {
            if (!tmp.isEmpty()) {
                searchLineInfo.argument = tmp;
                mSearchLineInfo.append(std::move(searchLineInfo));
            }
        }
    } else {
        if (!tmp.isEmpty()) {
            searchLineInfo.type = Literal;
            searchLineInfo.argument = tmp;
            mSearchLineInfo.append(std::move(searchLineInfo));
        }
    }
#ifdef DEBUG_COMMAND_PARSER
    qDebug() << " END " << mSearchLineInfo;
#endif
    // TODO add date ?
    // TODO add size: ?
    // TODO add support for double quote
    // We need to extend emailquery or creating query by hand
}

QList<SearchLineCommand::SearchLineInfo> SearchLineCommand::searchLineInfo() const
{
    return mSearchLineInfo;
}

void SearchLineCommand::setSearchLineInfo(const QList<SearchLineInfo> &newSearchLineInfo)
{
    mSearchLineInfo = newSearchLineInfo;
}

void SearchLineCommand::SearchLineInfo::clear()
{
    type = SearchLineCommand::SearchLineType::Unknown;
    argument.clear();
}

bool SearchLineCommand::SearchLineInfo::isValid() const
{
    if (type == SearchLineType::Unknown) {
        return false;
    }
    if (hasSubType(type) && !argument.isEmpty()) {
        return true;
    }
    if (!hasSubType(type) && argument.isEmpty()) {
        return true;
    }
    return false;
}

bool SearchLineCommand::SearchLineInfo::operator==(const SearchLineInfo &other) const
{
    return type == other.type && argument == other.argument;
}

QDebug operator<<(QDebug d, const MessageList::Core::SearchLineCommand::SearchLineInfo &info)
{
    d << " type " << info.type;
    d << " argument " << info.argument;
    return d;
}

#include "moc_searchlinecommand.cpp"
