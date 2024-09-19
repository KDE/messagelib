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
    {"from"_L1, SearchLineCommand::SearchLineType::From}
    // TODO add more
};
SearchLineCommand::SearchLineCommand()
{
}

SearchLineCommand::~SearchLineCommand() = default;

SearchLineCommand::SearchLineType SearchLineCommand::convertStringToSearchLineType(const QString &str) const
{
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
    for (int i = 0, total = str.length(); i < total; ++i) {
        const QChar ch = str.at(i);
        if (ch != QLatin1Char(':')) {
            tmp += ch;
        } else if (ch.isSpace()) {
            // TODO
        } else { // Compare with keys
            if (mKeyList.contains(tmp)) {
                // TODO
            }
            // TODO
        }
    }
    if (searchLineInfo.isValid()) {
        mSearchLineInfo.append(std::move(searchLineInfo));
    }
    // TODO parse subject:<foo> to:<foo> cc:<foo> bcc:<foo> from:<foo>
    // TODO add date ?
    // TODO add has:attachment ?
    // TODO add size: ?
    // TODO add is:important is:starred is:unread is:read
    // TODO
}

QString SearchLineCommand::dump() const
{
    // TODO
    return {};
}

QList<SearchLineCommand::SearchLineInfo> SearchLineCommand::searchLineInfo() const
{
    return mSearchLineInfo;
}

void SearchLineCommand::setSearchLineInfo(const QList<SearchLineInfo> &newSearchLineInfo)
{
    mSearchLineInfo = newSearchLineInfo;
}

bool SearchLineCommand::SearchLineInfo::isValid() const
{
    if (type == SearchLineType::Unknown) {
        return false;
    }
    // TODO
    return true;
}

bool SearchLineCommand::SearchLineInfo::operator==(const SearchLineInfo &other) const
{
    return type == other.type && argument == other.argument;
}
