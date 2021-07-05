/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/filter.h"
#include "core/messageitem.h"

#include <AkonadiSearch/PIM/emailquery.h>
#include <AkonadiSearch/PIM/resultiterator.h>
#include <KRandom>
using namespace MessageList::Core;

Filter::Filter(QObject *parent)
    : QObject(parent)
{
    generateRandomIdentifier();
}

bool Filter::containString(const QString &searchInString) const
{
    bool found = false;
    for (const QString &str : std::as_const(mSearchList)) {
        if (searchInString.contains(str, Qt::CaseInsensitive)) {
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

void Filter::setOptions(QuickSearchLine::SearchOptions newOptions)
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
        bool searchEveryWhere = (mOptions & QuickSearchLine::SearchEveryWhere);
        if (containString(item->subject()) && ((mOptions & QuickSearchLine::SearchAgainstSubject) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->sender()) && ((mOptions & QuickSearchLine::SearchAgainstFrom) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->receiver()) && ((mOptions & QuickSearchLine::SearchAgainstTo) || searchEveryWhere)) {
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

QVector<Akonadi::MessageStatus> Filter::status() const
{
    return mStatus;
}

void Filter::setStatus(const QVector<Akonadi::MessageStatus> &lstStatus)
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

const QString &Filter::searchString() const
{
    return mSearchString;
}

QuickSearchLine::SearchOptions Filter::currentOptions() const
{
    return mOptions;
}

void Filter::save(const KSharedConfig::Ptr &config, const QString &filtername, const QString &iconName)
{
    KConfigGroup grp(config, "General");
    int numberFilter = grp.readEntry("NumberFilter").toInt();
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
    KConfigGroup grp(config, "General");
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
    filter->setSearchString(newGroup.readEntry("searchString"), static_cast<QuickSearchLine::SearchOptions>(newGroup.readEntry("searchOptions").toInt()));
    filter->setTagId(newGroup.readEntry("tagId"));
    filter->setIdentifier(newGroup.readEntry("identifier"));
    filter->setFilterName(newGroup.readEntry("name"));
    filter->setIconName(newGroup.readEntry("iconName"));
    QList<qint32> lst;
    lst = newGroup.readEntry("status", QList<qint32>());
    QVector<Akonadi::MessageStatus> messageStatusLst;
    messageStatusLst.reserve(lst.count());
    for (const auto s : std::as_const(lst)) {
        Akonadi::MessageStatus status;
        status.fromQInt32(s);
        messageStatusLst << status;
    }
    filter->setStatus(messageStatusLst);
    filter->setOptions(static_cast<QuickSearchLine::SearchOptions>(newGroup.readEntry("searchOptions").toInt()));
    return filter;
}

void Filter::setSearchString(const QString &search, QuickSearchLine::SearchOptions options)
{
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
        if (options & QuickSearchLine::SearchEveryWhere) {
            query.matches(newStr);
            query.setSplitSearchMatchString(needToSplitString);
        } else if (options & QuickSearchLine::SearchAgainstSubject) {
            query.subjectMatches(newStr);
        } else if (options & QuickSearchLine::SearchAgainstBody) {
            query.bodyMatches(newStr);
        } else if (options & QuickSearchLine::SearchAgainstFrom) {
            query.setFrom(newStr);
        } else if (options & QuickSearchLine::SearchAgainstBcc) {
            query.setBcc(QStringList() << newStr);
        } else if (options & QuickSearchLine::SearchAgainstTo) {
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
