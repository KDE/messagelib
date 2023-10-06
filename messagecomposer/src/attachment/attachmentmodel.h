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

#include <Akonadi/Item>
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
    enum {
        AttachmentPartRole = Qt::UserRole,
        NameRole,
        SizeRole,
        EncodingRole,
        MimeTypeRole,
        CompressRole,
        EncryptRole,
        SignRole,
        AutoDisplayRole,
    };

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
        LastColumn, ///< @internal
    };

    explicit AttachmentModel(QObject *parent);
    ~AttachmentModel() override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
    [[nodiscard]] QStringList mimeTypes() const override;
    [[nodiscard]] Qt::DropActions supportedDropActions() const override;

    /// for the save/discard warning
    [[nodiscard]] bool isModified() const;
    void setModified(bool modified);

    [[nodiscard]] bool isEncryptEnabled() const;
    void setEncryptEnabled(bool enabled);
    [[nodiscard]] bool isSignEnabled() const;
    void setSignEnabled(bool enabled);
    [[nodiscard]] bool isEncryptSelected() const;
    /// sets for all
    void setEncryptSelected(bool selected);
    [[nodiscard]] bool isSignSelected() const;
    /// sets for all
    void setSignSelected(bool selected);

    [[nodiscard]] bool isAutoDisplayEnabled() const;
    void setAutoDisplayEnabled(bool enabled);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    bool updateAttachment(const MessageCore::AttachmentPart::Ptr &part);
    [[nodiscard]] bool replaceAttachment(const MessageCore::AttachmentPart::Ptr &oldPart, const MessageCore::AttachmentPart::Ptr &newPart);
    [[nodiscard]] bool removeAttachment(const MessageCore::AttachmentPart::Ptr &part);
    [[nodiscard]] MessageCore::AttachmentPart::List attachments() const;

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    void encryptEnabled(bool enabled);
    void signEnabled(bool enabled);
    void autoDisplayEnabled(bool enabled);
    void attachUrlsRequested(const QList<QUrl> &urls);
    void attachItemsRequester(const Akonadi::Item::List &);
    void attachmentRemoved(MessageCore::AttachmentPart::Ptr part);
    void attachmentCompressRequested(MessageCore::AttachmentPart::Ptr part, bool compress);

private:
    class AttachmentModelPrivate;
    friend class AttachmentModelPrivate;
    std::unique_ptr<AttachmentModelPrivate> const d;
};
} //
