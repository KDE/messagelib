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
    {"is:unread"_L1, SearchLineCommand::SearchLineType::IsUnRead},
    {"is:read"_L1, SearchLineCommand::SearchLineType::IsRead},
    {"is:important"_L1, SearchLineCommand::SearchLineType::IsImportant},
    // TODO add more
};
SearchLineCommand::SearchLineCommand() = default;

SearchLineCommand::~SearchLineCommand() = default;

bool SearchLineCommand::hasSubType(const QString &v)
{
    return v == QLatin1StringView("is") || v == QLatin1StringView("has");
}

QString SearchLineCommand::convertSearchLinetypeToTranslatedString(SearchLineCommand::SearchLineType type) const
{
    switch (type) {
    case Unknown:
    case HasStateOrAttachment:
    case Literal:
    case Date:
    case Size:
        return {};
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
    case IsImportant:
        return i18n("Mail is important");
    case IsRead:
        return i18n("Mail is read");
    case IsUnRead:
        return i18n("Mail is Unread");
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
            // qDebug() << " tmp ! " << tmp;
            if (mKeyList.contains(tmp.trimmed())) {
                // qDebug() << " contains " << tmp;
                searchLineInfo.type = mKeyList.value(tmp.trimmed());
                tmp.clear();
            } else if (hasSubType(tmp)) {
                searchLineInfo.type = HasStateOrAttachment;
                tmp += ch;
                // continue
            }
        } else if (ch.isSpace()) {
            // We can use is:... or has:...
            if (mKeyList.contains(tmp)) {
                searchLineInfo.type = mKeyList.value(tmp);
                tmp.clear();
            }

            // qDebug() << " is space " << "pare" << parentheses << " tmp " << tmp;
            if (searchLineInfo.type != Unknown && parentheses == 0) {
                searchLineInfo.argument = tmp;
                tmp.clear();
            } else { // Literal
                tmp += ch;
            }
            if (searchLineInfo.isValid() && parentheses == 0) {
                mSearchLineInfo.append(std::move(searchLineInfo));
                searchLineInfo.clear();
                tmp.clear();
            }
        } else if (ch == QLatin1Char('"')) {
            // TODO
        } else if (ch == QLatin1Char('(')) {
            parentheses++;
            // qDebug() << " parenthese ( equal " << parentheses;
        } else if (ch == QLatin1Char(')')) {
            parentheses--;
            // qDebug() << " parenthese ) equal " << parentheses;
            if (parentheses == 0) {
                searchLineInfo.argument = tmp;
                tmp.clear();
                // qDebug() << " new values " << searchLineInfo;
                mSearchLineInfo.append(std::move(searchLineInfo));
                searchLineInfo.clear();
            }
        } else {
            tmp += ch;
            // qDebug() << " tmp " << tmp << " ch " << ch << "end";
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
    // qDebug() << " END " << mSearchLineInfo;
    // TODO add date ?
    // TODO add size: ?
    // TODO add support for double quote
    // TODO add more email state (spam/starred etc .)
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
    return true;
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
