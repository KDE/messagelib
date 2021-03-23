/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QAbstractItemModel>
#include <QVector>
namespace Akonadi
{
class MessageStatus;
}

namespace MessageList
{
namespace Core
{
class MessageItem;

/**
 * The QAbstractItemModel based interface that you need
 * to provide for your storage to work with MessageList.
 */
class StorageModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit StorageModel(QObject *parent = nullptr);
    ~StorageModel() override;

    /**
     * Returns an unique id for this Storage collection.
     * FIXME: This could be embedded in "name()" ?
     */
    virtual QString id() const = 0;

    /**
     * Returns the unique id of the last selected message for this StorageModel.
     * Returns 0 if this value isn't stored in the configuration.
     */
    unsigned long preSelectedMessage() const;

    /**
     * Stores in the unique id of the last selected message for the specified StorageModel.
     * The uniqueIdOfMessage may be 0 (which means that no selection shall be stored in the configuration).
     */
    void savePreSelectedMessage(unsigned long uniqueIdOfMessage);

    /**
     * Returns true if this StorageModel (folder) contains outbound messages and false otherwise.
     */
    virtual bool containsOutboundMessages() const = 0;

    /**
     * Returns (a guess for) the number of unread messages: must be pessimistic (i.e. if you
     * have no idea just return rowCount(), which is also what the default implementation does).
     * This must be (and is) queried ONLY when the folder is first opened. It doesn't actually need
     * to keep the number of messages in sync as they later arrive to the storage.
     */
    virtual int initialUnreadRowCountGuess() const;

    /**
     * This method should use the inner model implementation to fill in the
     * base data for the specified MessageItem from the underlying storage slot at
     * the specified row index. Must return true if the data fetch was successful
     * and false otherwise. For base data we intend: subject, sender, receiver,
     * senderOrReceiver, size, date, encryption state, signature state and status.
     * If bUseReceiver is true then the "senderOrReceiver"
     * field must be set to the receiver, otherwise it must be set to the sender.
     */
    virtual bool initializeMessageItem(MessageItem *it, int row, bool bUseReceiver) const = 0;

    enum ThreadingDataSubset {
        PerfectThreadingOnly, ///< Only the data for messageIdMD5 and inReplyToMD5 is needed
        PerfectThreadingPlusReferences, ///< messageIdMD5, inReplyToMD5, referencesIdMD5
        PerfectThreadingReferencesAndSubject ///< All of the above plus subject stuff
    };

    /**
     * This method should use the inner model implementation to fill in the specified subset of
     * threading data for the specified MessageItem from the underlying storage slot at
     * the specified row index.
     */
    virtual void fillMessageItemThreadingData(MessageItem *mi, int row, ThreadingDataSubset subset) const = 0;

    /**
     * This method should use the inner model implementation to re-fill the date, the status,
     * the encryption state, the signature state and eventually update the min/max dates
     * for the specified MessageItem from the underlying storage slot at the specified row index.
     */
    virtual void updateMessageItemData(MessageItem *mi, int row) const = 0;

    /**
     * This method should use the inner model implementation to associate the new status
     * to the specified message item. The new status should be stored (but doesn't need
     * to be set as mi->status() itself as the caller is responsable for this).
     */
    virtual void setMessageItemStatus(MessageItem *mi, int row, Akonadi::MessageStatus status) = 0;

    /**
     * The implementation-specific mime data for this list of items.
     *    Called when the user initiates a drag from the messagelist.
     */
    virtual QMimeData *mimeData(const QVector<MessageItem *> &) const = 0;
    using QAbstractItemModel::mimeData;
};
} // namespace Core
} // namespace MessageList

