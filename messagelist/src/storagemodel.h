/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist/storagemodelbase.h"
#include "messagelist_export.h"

#include <Akonadi/Collection>
#include <Akonadi/Item>
#include <KMime/Message>
#include <QList>

class QAbstractItemModel;
class QItemSelectionModel;

namespace Akonadi
{
class Item;
}

namespace MessageList
{
namespace Core
{
class MessageItem;
}

/**
 * The Akonadi specific implementation of the Core::StorageModel.
 */
class MESSAGELIST_EXPORT StorageModel : public MessageList::Core::StorageModel
{
    Q_OBJECT

public:
    /**
     * Create a StorageModel wrapping the specified folder.
     */
    explicit StorageModel(QAbstractItemModel *model, QItemSelectionModel *selectionModel, QObject *parent = nullptr);
    ~StorageModel() override;

    [[nodiscard]] Akonadi::Collection::List displayedCollections() const;

    [[nodiscard]] QString id() const override;
    [[nodiscard]] bool containsOutboundMessages() const override;

    [[nodiscard]] virtual bool isOutBoundFolder(const Akonadi::Collection &c) const;

    [[nodiscard]] int initialUnreadRowCountGuess() const override;
    [[nodiscard]] bool initializeMessageItem(MessageList::Core::MessageItem *mi, int row, bool bUseReceiver) const override;
    void fillMessageItemThreadingData(MessageList::Core::MessageItem *mi, int row, ThreadingDataSubset subset) const override;
    void updateMessageItemData(MessageList::Core::MessageItem *mi, int row) const override;
    void setMessageItemStatus(MessageList::Core::MessageItem *mi, int row, Akonadi::MessageStatus status) override;

    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QMimeData *mimeData(const QList<MessageList::Core::MessageItem *> &) const override;
    using MessageList::Core::StorageModel::mimeData;

    [[nodiscard]] Akonadi::Item itemForRow(int row) const;
    [[nodiscard]] Akonadi::Collection parentCollectionForRow(int row) const;
    [[nodiscard]] KMime::Message::Ptr messageForRow(int row) const;

    [[nodiscard]] Akonadi::Collection collectionForId(Akonadi::Collection::Id colId) const;

    void resetModelStorage();

private:
    class StorageModelPrivate;
    std::unique_ptr<StorageModelPrivate> const d;
};
} // namespace MessageList
