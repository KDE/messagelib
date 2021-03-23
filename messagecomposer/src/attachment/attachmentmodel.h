/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "messagecomposer_export.h"

#include <QAbstractItemModel>

#include <QUrl>

#include <AkonadiCore/item.h>
#include <MessageCore/AttachmentPart>

namespace MessageComposer
{
/**
 * @brief The AttachmentModel class
 */
class MESSAGECOMPOSER_EXPORT AttachmentModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum { AttachmentPartRole = Qt::UserRole, NameRole, SizeRole, EncodingRole, MimeTypeRole, CompressRole, EncryptRole, SignRole, AutoDisplayRole };

    /**
     * @todo: get rid of columns and use the roles instead.
     */
    enum Column {
        NameColumn,
        SizeColumn,
        EncodingColumn,
        MimeTypeColumn,
        CompressColumn,
        EncryptColumn,
        SignColumn,
        AutoDisplayColumn,
        LastColumn ///< @internal
    };

    explicit AttachmentModel(QObject *parent);
    ~AttachmentModel() override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Q_REQUIRED_RESULT QMimeData *mimeData(const QModelIndexList &indexes) const override;
    Q_REQUIRED_RESULT QStringList mimeTypes() const override;
    Q_REQUIRED_RESULT Qt::DropActions supportedDropActions() const override;

    /// for the save/discard warning
    Q_REQUIRED_RESULT bool isModified() const;
    void setModified(bool modified);

    Q_REQUIRED_RESULT bool isEncryptEnabled() const;
    void setEncryptEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isSignEnabled() const;
    void setSignEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEncryptSelected() const;
    /// sets for all
    void setEncryptSelected(bool selected);
    Q_REQUIRED_RESULT bool isSignSelected() const;
    /// sets for all
    void setSignSelected(bool selected);

    Q_REQUIRED_RESULT bool isAutoDisplayEnabled() const;
    void setAutoDisplayEnabled(bool enabled);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_REQUIRED_RESULT bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    Q_REQUIRED_RESULT bool updateAttachment(const MessageCore::AttachmentPart::Ptr &part);
    Q_REQUIRED_RESULT bool replaceAttachment(const MessageCore::AttachmentPart::Ptr &oldPart, const MessageCore::AttachmentPart::Ptr &newPart);
    Q_REQUIRED_RESULT bool removeAttachment(const MessageCore::AttachmentPart::Ptr &part);
    Q_REQUIRED_RESULT MessageCore::AttachmentPart::List attachments() const;

    Q_REQUIRED_RESULT Qt::ItemFlags flags(const QModelIndex &index) const override;
    Q_REQUIRED_RESULT QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_REQUIRED_RESULT QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT QModelIndex parent(const QModelIndex &index) const override;
    Q_REQUIRED_RESULT int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT int columnCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void encryptEnabled(bool enabled);
    void signEnabled(bool enabled);
    void autoDisplayEnabled(bool enabled);
    void attachUrlsRequested(const QList<QUrl> &urls);
    void attachItemsRequester(const Akonadi::Item::List &);
    void attachmentRemoved(MessageCore::AttachmentPart::Ptr part);
    void attachmentCompressRequested(MessageCore::AttachmentPart::Ptr part, bool compress);

private:
    class Private;
    friend class Private;
    Private *const d;
};
} //

