/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <messagelist/storagemodelbase.h>
#include <messagelist_export.h>

#include <QVector>
#include <collection.h>
#include <item.h>
#include <kmime/kmime_message.h>

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

    Q_REQUIRED_RESULT Akonadi::Collection::List displayedCollections() const;

    Q_REQUIRED_RESULT QString id() const override;
    Q_REQUIRED_RESULT bool containsOutboundMessages() const override;

    virtual Q_REQUIRED_RESULT bool isOutBoundFolder(const Akonadi::Collection &c) const;

    Q_REQUIRED_RESULT int initialUnreadRowCountGuess() const override;
    Q_REQUIRED_RESULT bool initializeMessageItem(MessageList::Core::MessageItem *mi, int row, bool bUseReceiver) const override;
    void fillMessageItemThreadingData(MessageList::Core::MessageItem *mi, int row, ThreadingDataSubset subset) const override;
    void updateMessageItemData(MessageList::Core::MessageItem *mi, int row) const override;
    void setMessageItemStatus(MessageList::Core::MessageItem *mi, int row, Akonadi::MessageStatus status) override;

    Q_REQUIRED_RESULT int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_REQUIRED_RESULT QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT QModelIndex parent(const QModelIndex &index) const override;
    Q_REQUIRED_RESULT int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QMimeData *mimeData(const QVector<MessageList::Core::MessageItem *> &) const override;
    using MessageList::Core::StorageModel::mimeData;

    Q_REQUIRED_RESULT Akonadi::Item itemForRow(int row) const;
    Q_REQUIRED_RESULT Akonadi::Collection parentCollectionForRow(int row) const;
    Q_REQUIRED_RESULT KMime::Message::Ptr messageForRow(int row) const;

    Q_REQUIRED_RESULT Akonadi::Collection collectionForId(Akonadi::Collection::Id colId) const;

    void resetModelStorage();

private:
    class Private;
    Private *const d;
};
} // namespace MessageList

