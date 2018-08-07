/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#include "core/filter.h"
#include "core/messageitem.h"

#include <AkonadiSearch/SearchRunner>
#include <AkonadiCore/SearchQuery>

#include <KMime/Message>

#include <QRegularExpression>

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
        newStr = newStr.remove(0, 1);
        newStr = newStr.remove(newStr.length() - 1, 1);
        mSearchList = QStringList() << newStr;
    } else {
        const QStringList searchListTmp = mSearchString.split(QLatin1Char(' '), QString::SkipEmptyParts);
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
    if (newStr.trimmed().isEmpty()) {
        Q_EMIT finished();
        return;
    }

    Akonadi::SearchQuery query;
    if (options & QuickSearchLine::SearchEveryWhere) {
        if (needToSplitString) {
            const QStringList list = newStr.split(QRegularExpression(QStringLiteral("\\s")), QString::SkipEmptyParts);
            Akonadi::SearchTerm andTerm(Akonadi::SearchTerm::RelAnd);
            for (const auto &str : list) {
                andTerm.addSubTerm(Akonadi::EmailSearchTerm::matches(str));
            }
            query.addTerm(andTerm);
        } else {
            query.addTerm(Akonadi::EmailSearchTerm::matches(newStr));
        }
    } else if (options & QuickSearchLine::SearchAgainstSubject) {
        query.addTerm(Akonadi::EmailSearchTerm::subjectMatches(newStr));
    } else if (options & QuickSearchLine::SearchAgainstBody) {
        query.addTerm(Akonadi::EmailSearchTerm::bodyMatches(newStr));
    } else if (options & QuickSearchLine::SearchAgainstFrom) {
        query.addTerm(Akonadi::EmailSearchTerm::from({newStr}));
    } else if (options & QuickSearchLine::SearchAgainstBcc) {
        query.addTerm(Akonadi::EmailSearchTerm::bcc({newStr}));
    } else if (options & QuickSearchLine::SearchAgainstTo) {
        query.addTerm(Akonadi::EmailSearchTerm::to({newStr}));
    }

    //If the collection is virtual we're probably trying to filter the search collection, so we just search globally
    if (mCurrentFolder.isValid() && !mCurrentFolder.isVirtual()) {
        query.addTerm(Akonadi::SearchTerm::inCollection(mCurrentFolder.id()));
    }

    auto runner = new Akonadi::Search::SearchRunner(query, KMime::Message::mimeType());
    connect(runner, &Akonadi::Search::SearchRunner::finished,
            this, [this](Akonadi::Search::ResultIterator iter) {
                while (iter.next()) {
                    mMatchingItemIds.insert(iter.id());
                }
                Q_EMIT finished();
            });
    runner->start();
}
