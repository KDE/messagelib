/*
 * SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "searchcollectionindexingwarning.h"
#include "messagelist_debug.h"

#include <AkonadiCore/cachepolicy.h>
#include <AkonadiCore/collectionfetchjob.h>
#include <AkonadiCore/collectionfetchscope.h>
#include <AkonadiCore/collectionstatistics.h>
#include <AkonadiCore/entityhiddenattribute.h>
#include <AkonadiCore/persistentsearchattribute.h>

#include <PimCommon/PimUtil>

#include <KLocalizedString>

#include <AkonadiSearch/PIM/indexeditems.h>

using namespace MessageList::Core;

SearchCollectionIndexingWarning::SearchCollectionIndexingWarning(QWidget *parent)
    : KMessageWidget(parent)
    , mIndexedItems(new Akonadi::Search::PIM::IndexedItems(this))
{
    setVisible(false);
    setWordWrap(true);
    setText(
        i18n("Some of the search folders in this query are still being indexed "
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
    auto fetch = new Akonadi::CollectionFetchJob(cols, type, this);
    fetch->fetchScope().setAncestorRetrieval(Akonadi::CollectionFetchScope::None);
    fetch->fetchScope().setContentMimeTypes(QStringList() << Akonadi::Collection::mimeType() << QStringLiteral("message/rfc822"));
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

    const auto attr = collection.attribute<Akonadi::PersistentSearchAttribute>();
    Akonadi::Collection::List cols;
    const QVector<qint64> queryCols = attr->queryCollections();
    cols.reserve(queryCols.count());
    for (qint64 col : queryCols) {
        cols.push_back(Akonadi::Collection(col));
    }
    if (cols.isEmpty()) {
        return;
    }

    // First retrieve the top-level collections
    Akonadi::CollectionFetchJob *fetch = fetchCollections(cols, false);
    fetch->setProperty("recursiveQuery", attr->isRecursive());
    connect(fetch, &Akonadi::CollectionFetchJob::finished, this, &SearchCollectionIndexingWarning::queryRootCollectionFetchFinished);
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
    for (const Akonadi::Collection &col : std::as_const(mCollections)) {
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
