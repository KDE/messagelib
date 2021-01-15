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

using namespace MessageList::Core;

Filter::Filter(QObject *parent)
    : QObject(parent)
{
}

bool Filter::containString(const QString &searchInString) const
{
    bool found = false;
    for (const QString &str : qAsConst(mSearchList)) {
        if (searchInString.contains(str, Qt::CaseInsensitive)) {
            found = true;
        } else {
            found = false;
            break;
        }
    }
    return found;
}

bool Filter::match(const MessageItem *item) const
{
    if (!mStatus.isEmpty()) {
        for (Akonadi::MessageStatus status : qAsConst(mStatus)) {
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
        if (containString(item->subject()) && ((mOptions &QuickSearchLine::SearchAgainstSubject) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->sender()) && ((mOptions &QuickSearchLine::SearchAgainstFrom) || searchEveryWhere)) {
            searchMatches = true;
        } else if (containString(item->receiver()) && ((mOptions &QuickSearchLine::SearchAgainstTo) || searchEveryWhere)) {
            searchMatches = true;
        }
        if (!searchMatches) {
            return false;
        }
    }

    if (!mTagId.isEmpty()) {
        //mTagId is a Akonadi::Tag::url
        const bool tagMatches = item->findTag(mTagId) != nullptr;
        if (!tagMatches) {
            return false;
        }
    }

    return true;
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

QuickSearchLine::SearchOptions Filter::currentOptions() const
{
    return mOptions;
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

        //If the collection is virtual we're probably trying to filter the search collection, so we just search globally
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
