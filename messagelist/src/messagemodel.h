/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2026 Claudio Cambra <claudio.cambra@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *******************************************************************************/

#pragma once

#include "messagelist_export.h"

#include <Akonadi/Item>
#include <MessageList/Aggregation>
#include <QIdentityProxyModel>
#include <QList>

#include <memory>

class QAbstractItemModel;
class QItemSelectionModel;

namespace MessageList
{
/**
 * Public hierarchical message model for custom, non-widget user interfaces.
 *
 * Use this model for consumers such as Qt Quick that need the MessageList
 * threading implementation without constructing the traditional QWidget
 * message-list view. Existing QWidget users should continue to use
 * MessageList::Widget or MessageList::Pane.
 *
 * Setting the threading mode to Core::Aggregation::NoThreading produces a flat
 * message list; the model is therefore useful for both flat and threaded
 * presentations while keeping Core::Model private.
 *
 * The model owns the Core::Model and its Akonadi storage adapter. The entity
 * tree model and collection selection model remain owned by the caller.
 */
class MESSAGELIST_EXPORT MessageModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit MessageModel(QObject *parent = nullptr);
    ~MessageModel() override;

    void setEntityTreeModel(QAbstractItemModel *model);
    void setCollectionSelectionModel(QItemSelectionModel *selectionModel);
    /**
     * Sets how message relationships are inferred. Use NoThreading for a flat
     * list, PerfectOnly for In-Reply-To matching, PerfectAndReferences to also
     * use References, or PerfectReferencesAndSubject to additionally use
     * subject matching as a heuristic.
     */
    void setThreading(Core::Aggregation::Threading threading);

    [[nodiscard]] Core::Aggregation::Threading threading() const;
    [[nodiscard]] QModelIndex indexForItemId(Akonadi::Item::Id id) const;
    [[nodiscard]] QModelIndex threadRoot(const QModelIndex &index) const;
    [[nodiscard]] QList<Akonadi::Item::Id> threadItemIds(const QModelIndex &index) const;

private:
    class Private;
    std::unique_ptr<Private> const d;
};
}
