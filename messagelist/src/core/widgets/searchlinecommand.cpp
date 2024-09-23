/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommand.h"
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

void SearchLineCommand::parseSearchLineCommand(const QString &str)
{
    mSearchLineInfo.clear();
    if (str.isEmpty()) {
        return;
    }
    SearchLineInfo searchLineInfo;
    QString tmp;
    for (int i = 0, total = str.length(); i < total; ++i) {
        const QChar ch = str.at(i);
        if (ch == QLatin1Char(':')) {
            if (mKeyList.contains(tmp)) {
                // qDebug() << " contains " << tmp;
                searchLineInfo.type = mKeyList.value(tmp);
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

            // qDebug() << " is space ";
            if (searchLineInfo.type != Unknown) {
                searchLineInfo.argument = tmp;
                tmp.clear();
            } else { // Literal
                tmp += ch;
            }
            if (searchLineInfo.isValid()) {
                mSearchLineInfo.append(std::move(searchLineInfo));
                searchLineInfo.clear();
                tmp.clear();
            }
        } else if (ch == QLatin1Char('"')) {
            // TODO
        } else if (ch == QLatin1Char('(')) {
            // TODO
        } else if (ch == QLatin1Char(')')) {
            // TODO
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
        searchLineInfo.type = Literal;
        searchLineInfo.argument = tmp;
        mSearchLineInfo.append(std::move(searchLineInfo));
    }
    // TODO add date ?
    // TODO add size: ?
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
