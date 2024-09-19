/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommand.h"
using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
QStringList SearchLineCommand::mKeyList = {
    "subject"_L1,
    "to"_L1,
    "cc"_L1,
    "bcc"_L1,
    "from"_L1,
    // TODO add more
};
SearchLineCommand::SearchLineCommand()
{
}

SearchLineCommand::~SearchLineCommand() = default;

void SearchLineCommand::parseSearchLineCommand(const QString &str)
{
    if (str.isEmpty()) {
        return;
    }
    QString tmp;
    for (int i = 0, total = str.length(); i < total; ++i) {
        const QChar ch = str.at(i);
        if (ch != QLatin1Char(':')) {
            tmp += ch;
        } else if (ch.isSpace()) {
            // TODO
        } else { // Compare with keys
            for (const auto &key : std::as_const(mKeyList)) {
                if (tmp == key) {
                    // TODO
                    break;
                }
            }
            // TODO
        }
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
