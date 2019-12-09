/*
    Copyright (c) 2009 Kevin Ottens <ervin@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef MESSAGELIST_STORAGEMODEL_H
#define MESSAGELIST_STORAGEMODEL_H

#include <messagelist_export.h>
#include <messagelist/storagemodelbase.h>

#include <collection.h>
#include <item.h>

#include <kmime/kmime_message.h>

class QAbstractItemModel;
class QItemSelectionModel;

namespace Akonadi {
class Item;
}

namespace MessageList {
namespace Core {
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

    QMimeData *mimeData(const QList< MessageList::Core::MessageItem * > &) const override;
    using MessageList::Core::StorageModel::mimeData;

    Q_REQUIRED_RESULT Akonadi::Item itemForRow(int row) const;
    Q_REQUIRED_RESULT Akonadi::Collection parentCollectionForRow(int row) const;
    Q_REQUIRED_RESULT KMime::Message::Ptr messageForRow(int row) const;

    void resetModelStorage();

private:
    class Private;
    Private *const d;
};
} // namespace MessageList

#endif //!__MESSAGELIST_STORAGEMODEL_H
