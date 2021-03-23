/*
 * SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include "messagelist_private_export.h"
#include <AkonadiCore/Collection>
#include <KMessageWidget>
class KJob;

namespace Akonadi
{
namespace Search
{
namespace PIM
{
class IndexedItems;
}
}
}
namespace Akonadi
{
class CollectionFetchJob;
}
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchCollectionIndexingWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit SearchCollectionIndexingWarning(QWidget *parent = nullptr);
    ~SearchCollectionIndexingWarning() override;

    void setCollection(const Akonadi::Collection &collection);

private Q_SLOTS:
    void queryRootCollectionFetchFinished(KJob *job);
    void queryCollectionFetchFinished(KJob *job);

private:
    Akonadi::CollectionFetchJob *fetchCollections(const Akonadi::Collection::List &cols, bool recursive);
    void queryIndexerStatus();

    Akonadi::Collection mCollection;
    Akonadi::Collection::List mCollections;
    Akonadi::Search::PIM::IndexedItems *const mIndexedItems;
};
}
}

