/*
 * SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include "config-messagelist.h"
#include "messagelist_private_export.h"
#include <Akonadi/Collection>
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

private:
    MESSAGELIST_NO_EXPORT void queryRootCollectionFetchFinished(KJob *job);
    MESSAGELIST_NO_EXPORT void queryCollectionFetchFinished(KJob *job);
    MESSAGELIST_NO_EXPORT void queryIndexerStatus();
    [[nodiscard]] MESSAGELIST_NO_EXPORT Akonadi::CollectionFetchJob *fetchCollections(const Akonadi::Collection::List &cols, bool recursive);

    Akonadi::Collection mCollection;
    Akonadi::Collection::List mCollections;
#if !FORCE_DISABLE_AKONADI_SEARCH
    Akonadi::Search::PIM::IndexedItems *const mIndexedItems;
#endif
};
}
}
