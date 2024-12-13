/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/filter.h"
#include "config-messagelist.h"
#include "core/messageitem.h"
#include <MessageCore/StringUtil>
#include <TextUtils/ConvertText>

#include <KRandom>
#if !FORCE_DISABLE_AKONADI_SEARCH
#include <PIM/emailquery.h>
#include <PIM/resultiterator.h>
#endif
using namespace MessageList::Core;

Filter::Filter(QObject *parent)
    : QObject(parent)
{
    generateRandomIdentifier();
}

bool Filter::containString(const QString &searchInString) const
{
    bool found = false;
    const QString searchInStringNormalize{TextUtils::ConvertText::normalize(searchInString)};
    for (const QString &str : std::as_const(mSearchList)) {
        if (searchInStringNormalize.contains(TextUtils::ConvertText::normalize(str), Qt::CaseInsensitive)) {
            found = true;
        } else {
            found = false;
            break;
        }
    }
    return found;
}

const QString &Filter::iconName() const
{
    return mIconName;
}

void Filter::setIconName(const QString &newIconName)
{
    mIconName = newIconName;
}

void Filter::setOptions(SearchMessageByButtons::SearchOptions newOptions)
{
    mOptions = newOptions;
}

const QString &Filter::filterName() const
{
    return mFilterName;
}

void Filter::setFilterName(const QString &newFilterName)
{
    mFilterName = newFilterName;
}

void Filter::setIdentifier(const QString &newIdentifier)
{
    mIdentifier = newIdentifier;
}

bool Filter::match(const MessageItem *item) const
{
    if (!mStatus.isEmpty()) {
        for (Akonadi::MessageStatus status : std::as_const(mStatus)) {
            if (!(status & item->status())) {
                return false;
            }
        }
    }

    if (!mSearchString.isEmpty()) {
        if (mMatchingItemIds.contains(item->itemId())) {
            return true;
        }

        bool searchMatches = false;
        bool searchEveryWhere = (mOptions & SearchMessageByButtons::SearchEveryWhere);
        if (containString(item->subject()) && ((mOptions & SearchMessageByButtons::SearchAgainstSubject) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->sender()) && ((mOptions & SearchMessageByButtons::SearchAgainstFrom) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->receiver()) && ((mOptions & SearchMessageByButtons::SearchAgainstTo) || searchEveryWhere)) {
            searchMatches = true;
        }
        if (!searchMatches) {
            return false;
        }
    }

    if (!mTagId.isEmpty()) {
        // mTagId is a Akonadi::Tag::url
        const bool tagMatches = item->findTag(mTagId) != nullptr;
        if (!tagMatches) {
            return false;
        }
    }

    return true;
}

QList<Akonadi::MessageStatus> Filter::status() const
{
    return mStatus;
}

void Filter::setStatus(const QList<Akonadi::MessageStatus> &lstStatus)
{
    mStatus = lstStatus;
}

bool Filter::isEmpty() const
{
    if (!mStatus.isEmpty()) {
        return false;
    }

    if (!mSearchString.isEmpty()) {
        return false;
    }

    if (!mTagId.isEmpty()) {
        return false;
    }

    return true;
}

void Filter::clear()
{
    mStatus.clear();
    mSearchString.clear();
    mTagId.clear();
    mMatchingItemIds.clear();
    mSearchList.clear();
}

void Filter::setCurrentFolder(const Akonadi::Collection &folder)
{
    mCurrentFolder = folder;
}

QList<SearchLineCommand::SearchLineInfo> Filter::searchLineCommands() const
{
    const QString text = mSearchString;
    QList<SearchLineCommand::SearchLineInfo> infos;
    if (mOptions & SearchMessageByButtons::SearchAgainstBody) {
        SearchLineCommand::SearchLineInfo i;
        i.type = SearchLineCommand::SearchLineType::Body;
        i.argument = mSearchString;
        if (i.isValid()) {
            infos.append(std::move(i));
        }
    }
    if (mOptions & SearchMessageByButtons::SearchAgainstSubject) {
        SearchLineCommand::SearchLineInfo i;
        i.type = SearchLineCommand::SearchLineType::Subject;
        i.argument = mSearchString;
        if (i.isValid()) {
            infos.append(std::move(i));
        }
    }
    if (mOptions & SearchMessageByButtons::SearchAgainstBcc) {
        SearchLineCommand::SearchLineInfo i;
        i.type = SearchLineCommand::SearchLineType::Bcc;
        i.argument = mSearchString;
        if (i.isValid()) {
            infos.append(std::move(i));
        }
    }
    if (mOptions & SearchMessageByButtons::SearchAgainstBcc) {
        SearchLineCommand::SearchLineInfo i;
        i.type = SearchLineCommand::SearchLineType::Cc;
        i.argument = mSearchString;
        if (i.isValid()) {
            infos.append(std::move(i));
        }
    }

    for (Akonadi::MessageStatus status : status()) {
        if (status.hasAttachment()) {
            SearchLineCommand::SearchLineInfo i;
            i.type = SearchLineCommand::SearchLineType::HasAttachment;
            if (i.isValid()) {
                infos.append(std::move(i));
            }
        } else if (status.hasInvitation()) {
            SearchLineCommand::SearchLineInfo i;
            i.type = SearchLineCommand::SearchLineType::HasInvitation;
            if (i.isValid()) {
                infos.append(std::move(i));
            }
        } else if (status.isImportant()) {
            SearchLineCommand::SearchLineInfo i;
            i.type = SearchLineCommand::SearchLineType::IsImportant;
            if (i.isValid()) {
                infos.append(std::move(i));
            }
        } else if (status.isReplied()) {
            SearchLineCommand::SearchLineInfo i;
            i.type = SearchLineCommand::SearchLineType::IsReplied;
            if (i.isValid()) {
                infos.append(std::move(i));
            }
        } else if (status.isForwarded()) {
            SearchLineCommand::SearchLineInfo i;
            i.type = SearchLineCommand::SearchLineType::IsForwarded;
            if (i.isValid()) {
                infos.append(std::move(i));
            }
        }
        // TODO add more
    }

#if 0

    if (mContainsOutboundMessages) {
        mButtonGroup->button(SearchMessageByButtons::SearchAgainstTo)->setChecked(opts & SearchMessageByButtons::SearchAgainstTo);
    } else {
        mButtonGroup->button(SearchMessageByButtons::SearchAgainstTo)->setChecked(opts & SearchMessageByButtons::SearchAgainstFrom);
    }
#endif
    SearchLineCommand command;
    command.parseSearchLineCommand(text);
    infos += command.searchLineInfo();
    return infos;
}

const QString &Filter::searchString() const
{
    return mSearchString;
}

SearchMessageByButtons::SearchOptions Filter::currentOptions() const
{
    return mOptions;
}

void Filter::save(const KSharedConfig::Ptr &config, const QString &filtername, const QString &iconName, int numFilter)
{
    KConfigGroup grp(config, QStringLiteral("General"));
    int numberFilter = (numFilter == -1) ? grp.readEntry("NumberFilter").toInt() : numFilter;
    KConfigGroup newGroup(config, QStringLiteral("Filter_%1").arg(numberFilter++));
    newGroup.writeEntry("name", filtername);
    if (!iconName.isEmpty()) {
        newGroup.writeEntry("iconName", iconName);
    }
    newGroup.writeEntry("searchString", mSearchString);
    newGroup.writeEntry("searchOptions", static_cast<int>(mOptions));
    newGroup.writeEntry("tagId", mTagId);
    newGroup.writeEntry("identifier", mIdentifier);
    QList<qint32> lst;
    lst.reserve(mStatus.count());
    for (const auto s : std::as_const(mStatus)) {
        lst << s.toQInt32();
    }
    newGroup.writeEntry("status", lst);
    newGroup.sync();
    grp.writeEntry("NumberFilter", numberFilter);
    grp.sync();
    config->reparseConfiguration();
}

Filter *Filter::load(const KSharedConfig::Ptr &config, int filternumber)
{
    KConfigGroup grp(config, QStringLiteral("General"));
    int numberFilter = grp.readEntry("NumberFilter").toInt();
    if (filternumber < numberFilter) {
        KConfigGroup newGroup(config, QStringLiteral("Filter_%1").arg(filternumber));
        return loadFromConfigGroup(newGroup);
    }
    return nullptr;
}

Filter *Filter::loadFromConfigGroup(const KConfigGroup &newGroup)
{
    auto filter = new Filter();
    filter->setSearchString(newGroup.readEntry("searchString"),
                            static_cast<SearchMessageByButtons::SearchOptions>(newGroup.readEntry("searchOptions").toInt()));
    filter->setTagId(newGroup.readEntry("tagId"));
    filter->setIdentifier(newGroup.readEntry("identifier"));
    filter->setFilterName(newGroup.readEntry("name"));
    filter->setIconName(newGroup.readEntry("iconName"));
    const QList<qint32> lst = newGroup.readEntry("status", QList<qint32>());
    QList<Akonadi::MessageStatus> messageStatusLst;
    messageStatusLst.reserve(lst.count());
    for (const auto s : std::as_const(lst)) {
        Akonadi::MessageStatus status;
        status.fromQInt32(s);
        messageStatusLst << status;
    }
    filter->setStatus(messageStatusLst);
    filter->setOptions(static_cast<SearchMessageByButtons::SearchOptions>(newGroup.readEntry("searchOptions").toInt()));
    return filter;
}

void Filter::setSearchString(const SearchLineCommand &command)
{
#if !FORCE_DISABLE_AKONADI_SEARCH
    mMatchingItemIds.clear();
    if (command.isEmpty()) {
        return;
    }
    const QList<SearchLineCommand::SearchLineInfo> infos = command.searchLineInfo();
    QList<Akonadi::MessageStatus> lstStatus;
    Akonadi::Search::PIM::EmailQuery query;
    for (const auto &info : infos) {
        switch (info.type) {
        case SearchLineCommand::Literal: {
            QString newStr;
            const QStringList searchListTmp = info.argument.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            bool needToSplitString = false;
            for (const QString &text : searchListTmp) {
                if (text.size() >= 3) {
                    if (!newStr.isEmpty()) {
                        newStr += QLatin1Char(' ');
                    }
                    newStr += text;
                }
            }
            needToSplitString = true;

            mSearchString = newStr;
            query.matches(newStr);
            query.setSplitSearchMatchString(needToSplitString);
            break;
        }
        case SearchLineCommand::Subject: {
            mSearchString = info.argument;
            query.subjectMatches(mSearchString);
            break;
        }
        case SearchLineCommand::Body: {
            mSearchString = info.argument;
            query.bodyMatches(mSearchString);
            break;
        }
        case SearchLineCommand::Unknown:
        case SearchLineCommand::HasStateOrAttachment:
            // Nothing
            break;
        case SearchLineCommand::Larger:
        case SearchLineCommand::Smaller:
        case SearchLineCommand::OlderThan:
        case SearchLineCommand::NewerThan:
        case SearchLineCommand::Date:
        case SearchLineCommand::Size:
        case SearchLineCommand::Category:
            // TODO implement tag support
            break;
        case SearchLineCommand::HasAttachment: {
            Akonadi::MessageStatus status;
            status.setHasAttachment(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::HasInvitation: {
            Akonadi::MessageStatus status;
            status.setHasInvitation(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsImportant: {
            Akonadi::MessageStatus status;
            status.setImportant(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsRead: {
            Akonadi::MessageStatus status;
            status.setRead(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsUnRead: {
            // TODO verify
            Akonadi::MessageStatus status;
            status.setRead(false);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsIgnored: {
            Akonadi::MessageStatus status;
            status.setIgnored(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsHam: {
            Akonadi::MessageStatus status;
            status.setHam(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsSpam: {
            Akonadi::MessageStatus status;
            status.setSpam(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsWatched: {
            Akonadi::MessageStatus status;
            status.setWatched(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsReplied: {
            Akonadi::MessageStatus status;
            status.setReplied(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::IsForwarded: {
            Akonadi::MessageStatus status;
            status.setForwarded(true);
            lstStatus.append(status);
            break;
        }
        case SearchLineCommand::To:
            mSearchString = info.argument;
            query.addTo(info.argument);
            break;
        case SearchLineCommand::Bcc:
            mSearchString = info.argument;
            query.addBcc(info.argument);
            break;
        case SearchLineCommand::From:
            mSearchString = info.argument;
            query.addFrom(info.argument);
            break;
        case SearchLineCommand::Cc:
            mSearchString = info.argument;
            query.addCc(info.argument);
            break;
        }
    }

    setStatus(lstStatus);
    // If the collection is virtual we're probably trying to filter the search collection, so we just search globally
    if (mCurrentFolder.isValid() && !mCurrentFolder.isVirtual()) {
        query.addCollection(mCurrentFolder.id());
    }

    Akonadi::Search::PIM::ResultIterator it = query.exec();
    while (it.next()) {
        mMatchingItemIds << it.id();
    }
    Q_EMIT finished();
#endif
}

void Filter::setSearchString(const QString &search, SearchMessageByButtons::SearchOptions options)
{
#if !FORCE_DISABLE_AKONADI_SEARCH
    const QString trimStr = search.trimmed();
    if ((mSearchString == trimStr) && (mOptions == options)) {
        return;
    }
    mOptions = options;
    mSearchString = trimStr;
    mMatchingItemIds.clear();

    if (mSearchString.isEmpty()) {
        return;
    }
    bool needToSplitString = false;
    QString newStr = mSearchString;
    if (mSearchString.startsWith(QLatin1Char('"')) && mSearchString.startsWith(QLatin1Char('"'))) {
        newStr.remove(0, 1);
        newStr.remove(newStr.length() - 1, 1);
        mSearchList = QStringList() << newStr;
    } else {
        const QStringList searchListTmp = mSearchString.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        mSearchList.clear();
        newStr.clear();
        for (const QString &text : searchListTmp) {
            if (text.size() >= 3) {
                mSearchList << text;
                if (!newStr.isEmpty()) {
                    newStr += QLatin1Char(' ');
                }
                newStr += text;
            }
        }
        needToSplitString = true;
    }
    if (!newStr.trimmed().isEmpty()) {
        Akonadi::Search::PIM::EmailQuery query;
        if (options & SearchMessageByButtons::SearchEveryWhere) {
            query.matches(newStr);
            query.setSplitSearchMatchString(needToSplitString);
        } else if (options & SearchMessageByButtons::SearchAgainstSubject) {
            query.subjectMatches(newStr);
        } else if (options & SearchMessageByButtons::SearchAgainstBody) {
            query.bodyMatches(newStr);
        } else if (options & SearchMessageByButtons::SearchAgainstFrom) {
            query.setFrom(newStr);
        } else if (options & SearchMessageByButtons::SearchAgainstBcc) {
            query.setBcc(QStringList() << newStr);
        } else if (options & SearchMessageByButtons::SearchAgainstCc) {
            query.setCc(QStringList() << newStr);
        } else if (options & SearchMessageByButtons::SearchAgainstTo) {
            query.setTo(QStringList() << newStr);
        }

        // If the collection is virtual we're probably trying to filter the search collection, so we just search globally
        if (mCurrentFolder.isValid() && !mCurrentFolder.isVirtual()) {
            query.addCollection(mCurrentFolder.id());
        }

        Akonadi::Search::PIM::ResultIterator it = query.exec();
        while (it.next()) {
            mMatchingItemIds << it.id();
        }
    }
    Q_EMIT finished();
#endif
}

const QString &Filter::tagId() const
{
    return mTagId;
}

void Filter::setTagId(const QString &tagId)
{
    mTagId = tagId;
}

void Filter::generateRandomIdentifier()
{
    mIdentifier = KRandom::randomString(16);
}

QString Filter::identifier() const
{
    return mIdentifier;
}

QDebug operator<<(QDebug d, const MessageList::Core::Filter &t)
{
    d << "filtername " << t.filterName();
    d << "identifier " << t.identifier();
    d << "search string " << t.searchString();
    d << "search option " << t.currentOptions();
    d << "status " << t.status();
    return d;
}

#include "moc_filter.cpp"
