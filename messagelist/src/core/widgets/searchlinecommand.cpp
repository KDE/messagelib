/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommand.h"
#include <KLocalizedString>
#include <QDateTime>

// #define DEBUG_COMMAND_PARSER 1

using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
QMap<QString, SearchLineCommand::SearchLineType> SearchLineCommand::mKeyList = {
    {SearchLineCommand::searchLineTypeToString(Subject), SearchLineCommand::SearchLineType::Subject},
    {SearchLineCommand::searchLineTypeToString(Body), SearchLineCommand::SearchLineType::Body},
    {SearchLineCommand::searchLineTypeToString(To), SearchLineCommand::SearchLineType::To},
    {SearchLineCommand::searchLineTypeToString(Cc), SearchLineCommand::SearchLineType::Cc},
    {SearchLineCommand::searchLineTypeToString(Bcc), SearchLineCommand::SearchLineType::Bcc},
    {SearchLineCommand::searchLineTypeToString(From), SearchLineCommand::SearchLineType::From},
    {SearchLineCommand::searchLineTypeToString(HasAttachment), SearchLineCommand::SearchLineType::HasAttachment},
    {SearchLineCommand::searchLineTypeToString(HasInvitation), SearchLineCommand::SearchLineType::HasInvitation},
    {SearchLineCommand::searchLineTypeToString(IsUnRead), SearchLineCommand::SearchLineType::IsUnRead},
    {SearchLineCommand::searchLineTypeToString(IsRead), SearchLineCommand::SearchLineType::IsRead},
    {SearchLineCommand::searchLineTypeToString(IsImportant), SearchLineCommand::SearchLineType::IsImportant},
    {SearchLineCommand::searchLineTypeToString(IsIgnored), SearchLineCommand::SearchLineType::IsIgnored},
    {SearchLineCommand::searchLineTypeToString(IsHam), SearchLineCommand::SearchLineType::IsHam},
    {SearchLineCommand::searchLineTypeToString(IsSpam), SearchLineCommand::SearchLineType::IsSpam},
    {SearchLineCommand::searchLineTypeToString(IsWatched), SearchLineCommand::SearchLineType::IsWatched},
    {SearchLineCommand::searchLineTypeToString(IsReplied), SearchLineCommand::SearchLineType::IsReplied},
    {SearchLineCommand::searchLineTypeToString(IsForwarded), SearchLineCommand::SearchLineType::IsForwarded},
    {SearchLineCommand::searchLineTypeToString(IsEncrypted), SearchLineCommand::SearchLineType::IsEncrypted},
    {SearchLineCommand::searchLineTypeToString(IsQueued), SearchLineCommand::SearchLineType::IsQueued},
    {SearchLineCommand::searchLineTypeToString(IsSent), SearchLineCommand::SearchLineType::IsSent},
    {SearchLineCommand::searchLineTypeToString(IsDeleted), SearchLineCommand::SearchLineType::IsDeleted},
    {SearchLineCommand::searchLineTypeToString(IsAction), SearchLineCommand::SearchLineType::IsAction},
    {SearchLineCommand::searchLineTypeToString(Size), SearchLineCommand::SearchLineType::Size},
    {SearchLineCommand::searchLineTypeToString(Smaller), SearchLineCommand::SearchLineType::Larger},
    {SearchLineCommand::searchLineTypeToString(Larger), SearchLineCommand::SearchLineType::Smaller},
    {SearchLineCommand::searchLineTypeToString(OlderThan), SearchLineCommand::SearchLineType::OlderThan},
    {SearchLineCommand::searchLineTypeToString(NewerThan), SearchLineCommand::SearchLineType::NewerThan},
    {SearchLineCommand::searchLineTypeToString(Category), SearchLineCommand::SearchLineType::Category},
    // after:before:older:newer:
    // category:
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
        || type == OlderThan || type == NewerThan || type == Body || type == Category;
}

bool SearchLineCommand::mustBeUnique(SearchLineType type)
{
    return type == HasAttachment || type == IsImportant || type == IsRead || type == IsUnRead || type == IsIgnored || type == IsHam || type == IsSpam
        || type == IsWatched || type == IsReplied || type == IsForwarded || type == IsEncrypted || type == IsQueued || type == IsSent || type == IsDeleted
        || type == IsAction;
}

bool SearchLineCommand::isEmpty() const
{
    return mSearchLineInfo.isEmpty();
}

QString SearchLineCommand::searchLineTypeToString(SearchLineType type)
{
    switch (type) {
    case Unknown:
    case HasStateOrAttachment:
    case Literal:
    case Date:
        return {};
    case OlderThan:
        return u"older_than"_s;
    case NewerThan:
        return u"newer_than"_s;
    case Smaller:
        return u"smaller"_s;
    case Larger:
        return u"larger"_s;
    case Size:
        return u"size"_s;
    case To:
        return u"to"_s;
    case Bcc:
        return u"bcc"_s;
    case Cc:
        return u"cc"_s;
    case From:
        return u"from"_s;
    case Subject:
        return u"subject"_s;
    case Body:
        return u"body"_s;
    case Category:
        return u"category"_s;
    case HasAttachment:
        return u"has:attachment"_s;
    case HasInvitation:
        return u"has:invitation"_s;
    case IsImportant:
        return u"is:important"_s;
    case IsRead:
        return u"is:read"_s;
    case IsUnRead:
        return u"is:unread"_s;
    case IsIgnored:
        return u"is:ignored"_s;
    case IsHam:
        return u"is:ham"_s;
    case IsSpam:
        return u"is:spam"_s;
    case IsWatched:
        return u"is:watched"_s;
    case IsReplied:
        return u"is:replied"_s;
    case IsForwarded:
        return u"is:forwarded"_s;
    case IsEncrypted:
        return u"is:encrypted"_s;
    case IsQueued:
        return u"is:queued"_s;
    case IsSent:
        return u"is:sent"_s;
    case IsAction:
        return u"is:action"_s;
    case IsDeleted:
        return u"is:deleted"_s;
    }
    return {};
}

QString SearchLineCommand::convertSearchLinetypeToTranslatedString(SearchLineCommand::SearchLineType type) const
{
    switch (type) {
    case Unknown:
    case HasStateOrAttachment:
        return {};
        // TODO implement Date
    case Date:
    case OlderThan:
    case NewerThan:
        return {};
    case Literal:
        return i18n("Literal string");
    case Smaller:
        return i18n("Size is smaller than");
    case Larger:
        return i18n("Size is larger than");
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
    case Body:
        return i18n("Body contains");
    case Category:
        return i18n("Mail has tag");
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
    case IsEncrypted:
        return i18n("Mail is encrypted");
    case IsQueued:
        return i18n("Mail is queued");
    case IsSent:
        return i18n("Mail is sent");
    case IsDeleted:
        return i18n("Mail is deleted");
    case IsAction:
        return i18n("Mail is action");
    }
    return {};
}

QString SearchLineCommand::generateCommadLineStr() const
{
    QString result;
    for (const auto &info : mSearchLineInfo) {
        if (!result.isEmpty()) {
            result += u' ' + i18n("AND") + u' ';
        }
        const QString translatedType = convertSearchLinetypeToTranslatedString(info.type);
        if (!translatedType.isEmpty()) {
            result += translatedType;
        }
        if (!info.argument.isEmpty()) {
            if (!translatedType.isEmpty()) {
                result += u' ';
            }
            result += info.argument;
        }
    }
    return result;
}

SearchLineCommand::SearchLineInfo SearchLineCommand::isAnotherInfo(QString tmp, SearchLineInfo searchLineInfo)
{
    if (!tmp.contains(u' ')) {
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
            if (!searchLineInfo.argument.isEmpty() && searchLineInfo.type == Unknown) {
                searchLineInfo.type = Literal;
            }
            if (searchLineInfo.isValid()) {
                appendSearchLineInfo(searchLineInfo);
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " Add  searchLineInfo" << searchLineInfo << mSearchLineInfo;
#endif

            SearchLineInfo info;
            info.type = mKeyList.value(key);
            return info;
        }
    }
    return {};
}

bool SearchLineCommand::hasOnlyOneLiteralCommand() const
{
    return mSearchLineInfo.count() == 1 && (mSearchLineInfo.at(0).type == SearchLineType::Literal);
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
        if (ch == u':') {
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " tmp ! " << tmp;
#endif
            const SearchLineCommand::SearchLineInfo newInfo = isAnotherInfo(tmp, searchLineInfo);
            if (newInfo.type != Unknown) {
                tmp.clear();
                searchLineInfo = newInfo;
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
            const SearchLineCommand::SearchLineInfo newInfo = isAnotherInfo(tmp, searchLineInfo);
            if (newInfo.type != Unknown) {
                tmp.clear();
                searchLineInfo = newInfo;
            } else if (mKeyList.contains(tmp)) { // We can use is:... or has:...
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
                appendSearchLineInfo(searchLineInfo);
                searchLineInfo.clear();
                tmp.clear();
            }
        } else if (ch == u'(') {
            parentheses++;
            if (parentheses > 1) {
                tmp += ch;
            }
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " parenthese ( equal " << parentheses;
#endif
        } else if (ch == u')') {
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
                appendSearchLineInfo(searchLineInfo);
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
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " type is HasStateOrAttachment";
#endif
            if (mKeyList.contains(tmp)) {
                searchLineInfo.type = mKeyList.value(tmp);
                appendSearchLineInfo(searchLineInfo);
            }
        } else {
            if (!tmp.isEmpty()) {
#ifdef DEBUG_COMMAND_PARSER
                qDebug() << " add as original searchLineInfo" << searchLineInfo;
#endif
                const SearchLineCommand::SearchLineInfo newInfo = isAnotherInfo(tmp, searchLineInfo);
                if (newInfo.type != Unknown) {
                    searchLineInfo = newInfo;
                } else {
                    searchLineInfo.argument = tmp;
                }
                appendSearchLineInfo(searchLineInfo);
            }
        }
    } else {
        if (!tmp.isEmpty()) {
            searchLineInfo.type = Literal;
            searchLineInfo.argument = tmp;
            appendSearchLineInfo(searchLineInfo);
        }
    }
#ifdef DEBUG_COMMAND_PARSER
    qDebug() << " END " << mSearchLineInfo;
#endif
    // TODO add date ?
    // TODO add support for double quote
    // We need to extend emailquery or creating query by hand
}

void SearchLineCommand::appendSearchLineInfo(SearchLineInfo searchLineInfo)
{
    if (searchLineInfo.mustBeUnique()) {
        if (mSearchLineInfo.contains(searchLineInfo)) {
#ifdef DEBUG_COMMAND_PARSER
            qDebug() << " Already exist " << searchLineInfo;
#endif
            return;
        }
    }
    mSearchLineInfo.append(std::move(searchLineInfo));
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
    if (type == SearchLineType::Unknown || type == SearchLineCommand::HasStateOrAttachment) {
        return false;
    }
    if (type == SearchLineType::Literal && !argument.isEmpty()) {
        return true;
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

bool SearchLineCommand::SearchLineInfo::isValidDate() const
{
    if (argument.isEmpty()) {
        return false;
    }
    return QDateTime::fromString(argument).isValid();
}

bool SearchLineCommand::SearchLineInfo::mustBeUnique() const
{
    return SearchLineCommand::mustBeUnique(type);
}

qint64 SearchLineCommand::SearchLineInfo::convertArgumentAsSize() const
{
    // TODO convert it
    return {};
}

QDebug operator<<(QDebug d, const MessageList::Core::SearchLineCommand::SearchLineInfo &info)
{
    d << " type " << info.type;
    d << " argument " << info.argument;
    return d;
}

QString SearchLineCommand::generateCommandText(SearchLineCommand::SearchLineType type)
{
    bool needSpace = SearchLineCommand::mustBeUnique(type);
    return SearchLineCommand::searchLineTypeToString(type) + (needSpace ? u" "_s : QStringLiteral(":"));
}

#include "moc_searchlinecommand.cpp"
