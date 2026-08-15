/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2026 Claudio Cambra <claudio.cambra@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *******************************************************************************/

#include "messagemodel.h"

#include "core/aggregation.h"
#include "core/model.h"
#include "core/modelviewinterface.h"
#include "core/sortorder.h"
#include "core/theme.h"
#include "storagemodel.h"

#include <Akonadi/EntityTreeModel>
#include <QItemSelectionModel>
#include <QPointer>

namespace MessageList
{
namespace
{
class MessageModelViewAdapter final : public Core::ModelViewInterface
{
public:
    void setSelectionModel(QItemSelectionModel *selectionModel)
    {
        mSelectionModel = selectionModel;
    }

    QItemSelectionModel *selectionModel() const override
    {
        return mSelectionModel;
    }

private:
    QItemSelectionModel *mSelectionModel = nullptr;
};

std::unique_ptr<Core::Theme> createMessageModelTheme()
{
    // Core::Model currently uses Theme for its column structure and sorting
    // metadata as well as widget presentation. Keep this minimal schema here
    // until those responsibilities are separated.
    auto theme = std::make_unique<Core::Theme>();
    const auto column = new Core::Theme::Column;
    column->setMessageSorting(Core::SortOrder::SortMessagesByDateTimeOfMostRecent);

    const auto messageRow = new Core::Theme::Row;
    messageRow->addLeftItem(new Core::Theme::ContentItem(Core::Theme::ContentItem::Subject));
    column->addMessageRow(messageRow);

    const auto groupHeaderRow = new Core::Theme::Row;
    groupHeaderRow->addLeftItem(new Core::Theme::ContentItem(Core::Theme::ContentItem::GroupHeaderLabel));
    column->addGroupHeaderRow(groupHeaderRow);

    theme->addColumn(column);
    return theme;
}
}

class MessageModel::Private
{
public:
    explicit Private(MessageModel *q)
        : q(q)
        , theme(createMessageModelTheme())
        , modelViewAdapter(std::make_unique<MessageModelViewAdapter>())
        , coreModel(std::make_unique<Core::Model>(modelViewAdapter.get(), q))
        , selectionModel(std::make_unique<QItemSelectionModel>(coreModel.get(), q))
    {
        aggregation.setGrouping(Core::Aggregation::NoGrouping);
        aggregation.setThreading(threading);
        aggregation.setThreadLeader(Core::Aggregation::MostRecentMessage);
        aggregation.setThreadExpandPolicy(Core::Aggregation::NeverExpandThreads);
        aggregation.setFillViewStrategy(Core::Aggregation::FavorInteractivity);

        sortOrder.setMessageSorting(Core::SortOrder::SortMessagesByDateTimeOfMostRecent);
        sortOrder.setMessageSortDirection(Core::SortOrder::Descending);

        modelViewAdapter->setSelectionModel(selectionModel.get());
        coreModel->setTheme(theme.get());
        coreModel->setSortOrder(&sortOrder);
        coreModel->setAggregation(&aggregation);
    }

    ~Private()
    {
        // Core::Model clears its selection model while detaching its storage
        // model, so both objects must remain alive until that operation is
        // complete.
        coreModel->setStorageModel(nullptr);
        storageModel.reset();
        coreModel.reset();
        selectionModel.reset();
    }

    void rebuildStorageModel()
    {
        coreModel->setStorageModel(nullptr);
        storageModel.reset();

        if (entityTreeModel && collectionSelectionModel) {
            storageModel = std::make_unique<MessageList::StorageModel>(entityTreeModel, collectionSelectionModel, q);
            coreModel->setStorageModel(storageModel.get(), Core::PreSelectNone);
        }
    }

    MessageModel *const q;
    std::unique_ptr<Core::Theme> theme;
    Core::Aggregation aggregation;
    Core::SortOrder sortOrder;
    std::unique_ptr<MessageModelViewAdapter> modelViewAdapter;
    std::unique_ptr<Core::Model> coreModel;
    std::unique_ptr<QItemSelectionModel> selectionModel;
    std::unique_ptr<MessageList::StorageModel> storageModel;
    QPointer<QAbstractItemModel> entityTreeModel;
    QPointer<QItemSelectionModel> collectionSelectionModel;
    Core::Aggregation::Threading threading = Core::Aggregation::PerfectReferencesAndSubject;
};

MessageModel::MessageModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , d(new Private(this))
{
    setSourceModel(d->coreModel.get());
}

MessageModel::~MessageModel()
{
    setSourceModel(nullptr);
}

void MessageModel::setEntityTreeModel(QAbstractItemModel *model)
{
    if (d->entityTreeModel == model) {
        return;
    }

    d->entityTreeModel = model;
    if (model) {
        connect(model, &QObject::destroyed, this, [this] {
            if (!d->entityTreeModel) {
                d->rebuildStorageModel();
            }
        });
    }
    d->rebuildStorageModel();
}

void MessageModel::setCollectionSelectionModel(QItemSelectionModel *selectionModel)
{
    if (d->collectionSelectionModel == selectionModel) {
        return;
    }

    d->collectionSelectionModel = selectionModel;
    if (selectionModel) {
        connect(selectionModel, &QObject::destroyed, this, [this] {
            if (!d->collectionSelectionModel) {
                d->rebuildStorageModel();
            }
        });
    }
    d->rebuildStorageModel();
}

void MessageModel::setThreading(Core::Aggregation::Threading threading)
{
    if (d->threading == threading) {
        return;
    }

    d->threading = threading;
    d->aggregation.setThreading(threading);
    d->coreModel->setAggregation(&d->aggregation);
    if (d->storageModel) {
        d->coreModel->setStorageModel(d->storageModel.get(), Core::PreSelectNone);
    }
}

Core::Aggregation::Threading MessageModel::threading() const
{
    return d->threading;
}

QModelIndex MessageModel::indexForItemId(Akonadi::Item::Id id) const
{
    if (id <= 0) {
        return {};
    }

    QList<QModelIndex> pending;
    for (int row = 0; row < rowCount(); ++row) {
        pending.append(index(row, 0));
    }

    while (!pending.isEmpty()) {
        const auto candidate = pending.takeFirst();
        if (candidate.data(Akonadi::EntityTreeModel::ItemIdRole).toLongLong() == id) {
            return candidate;
        }

        for (int row = 0; row < rowCount(candidate); ++row) {
            pending.append(index(row, 0, candidate));
        }
    }

    return {};
}

QModelIndex MessageModel::threadRoot(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }

    auto root = index;
    while (true) {
        const auto parentIndex = parent(root);
        if (!parentIndex.isValid()) {
            break;
        }
        root = parentIndex;
    }
    return root;
}

QList<Akonadi::Item::Id> MessageModel::threadItemIds(const QModelIndex &index) const
{
    QList<Akonadi::Item::Id> ids;
    if (!index.isValid()) {
        return ids;
    }

    const auto itemId = index.data(Akonadi::EntityTreeModel::ItemIdRole).toLongLong();
    if (itemId > 0) {
        ids.append(itemId);
    }

    for (int row = 0; row < rowCount(index); ++row) {
        ids.append(threadItemIds(this->index(row, 0, index)));
    }
    return ids;
}
}
