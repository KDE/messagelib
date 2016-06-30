/*
 * Copyright (c) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "searchcollectionindexingwarning.h"
#include "messagelist_debug.h"

#include <AkonadiCore/persistentsearchattribute.h>
#include <AkonadiCore/collectionfetchjob.h>
#include <AkonadiCore/collectionfetchscope.h>
#include <AkonadiCore/collectionstatistics.h>
#include <AkonadiCore/entityhiddenattribute.h>
#include <AkonadiCore/cachepolicy.h>

#include <PimCommon/PimUtil>

#include <QDBusInterface>

#include <KLocalizedString>

#include <AkonadiSearch/PIM/indexeditems.h>

using namespace MessageList::Core;

SearchCollectionIndexingWarning::SearchCollectionIndexingWarning(QWidget *parent)
    : KMessageWidget(parent),
      mIndexedItems(new Akonadi::Search::PIM::IndexedItems(this))
{
    setVisible(false);
    setWordWrap(true);
    setText(i18n("Some of the search folders in this query are still being indexed "
                 "or are excluded from indexing completely. The results below may be incomplete."));
    setCloseButtonVisible(true);
    setMessageType(Information);
}

SearchCollectionIndexingWarning::~SearchCollectionIndexingWarning()
{
}

Akonadi::CollectionFetchJob *SearchCollectionIndexingWarning::fetchCollections(const Akonadi::Collection::List &cols, bool recursive)
{
    const Akonadi::CollectionFetchJob::Type type = recursive ? Akonadi::CollectionFetchJob::Recursive : Akonadi::CollectionFetchJob::Base;
    Akonadi::CollectionFetchJob *fetch = new Akonadi::CollectionFetchJob(cols, type, this);
    fetch->fetchScope().setAncestorRetrieval(Akonadi::CollectionFetchScope::None);
    fetch->fetchScope().setContentMimeTypes(QStringList() << Akonadi::Collection::mimeType()
                                            << QStringLiteral("message/rfc822"));
    fetch->fetchScope().setIncludeStatistics(true);
    return fetch;
}

void SearchCollectionIndexingWarning::setCollection(const Akonadi::Collection &collection)
{
    if (collection == mCollection) {
        return;
    }

    animatedHide();

    mCollection = collection;
    mCollections.clear();

    // Not a search collection?
    if (!collection.hasAttribute<Akonadi::PersistentSearchAttribute>()) {
        return;
    }

    Akonadi::PersistentSearchAttribute *attr = collection.attribute<Akonadi::PersistentSearchAttribute>();
    Akonadi::Collection::List cols;
    Q_FOREACH (qint64 col, attr->queryCollections()) {
        cols.push_back(Akonadi::Collection(col));
    }

    // First retrieve the top-level collections
    Akonadi::CollectionFetchJob *fetch = fetchCollections(cols, false);
    fetch->setProperty("recursiveQuery", attr->isRecursive());
    connect(fetch, SIGNAL(finished(KJob*)), this, SLOT(queryRootCollectionFetchFinished(KJob*)));
}

void SearchCollectionIndexingWarning::queryRootCollectionFetchFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGELIST_LOG) << job->errorString();
        return;
    }

    // Store the root collections
    mCollections = qobject_cast<Akonadi::CollectionFetchJob *>(job)->collections();

    if (job->property("recursiveQuery").toBool()) {
        // Fetch all descendants, if necessary
        Akonadi::CollectionFetchJob *fetch = fetchCollections(mCollections, true);
        connect(fetch, &Akonadi::CollectionFetchJob::finished, this, &SearchCollectionIndexingWarning::queryCollectionFetchFinished);
    } else {
        queryIndexerStatus();
    }
}

void SearchCollectionIndexingWarning::queryCollectionFetchFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGELIST_LOG) << job->errorString();
        return;
    }

    mCollections += qobject_cast<Akonadi::CollectionFetchJob *>(job)->collections();
    queryIndexerStatus();
}

void SearchCollectionIndexingWarning::queryIndexerStatus()
{
    bool allFullyIndexed = true;
    Q_FOREACH (const Akonadi::Collection &col, mCollections) {
        if (col.hasAttribute<Akonadi::EntityHiddenAttribute>()) {
            continue;
        }
        if (PimCommon::Util::isImapResource(col.resource()) && !col.cachePolicy().localParts().contains(QLatin1String("RFC822"))) {
            continue;
        }
        const qlonglong result = mIndexedItems->indexedItems(col.id());

        qCDebug(MESSAGELIST_LOG) << "Collection:" << col.displayName() << "(" << col.id() << "), count:" << col.statistics().count() << ", index:" << result;
        if (col.statistics().count() != result) {
            allFullyIndexed = false;
            break;
        }
    }
    if (!allFullyIndexed) {
        animatedShow();
    }
}
