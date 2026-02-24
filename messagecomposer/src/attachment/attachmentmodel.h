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
/*!
 * \class MessageComposer::AttachmentModel
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/AttachmentModel
 * \brief The AttachmentModel class
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

    /*! \brief Constructs an AttachmentModel.
        \param parent The parent object.
    */
    explicit AttachmentModel(QObject *parent);
    /*! \brief Destroys the AttachmentModel. */
    ~AttachmentModel() override;

    /*! \brief Handles dropping of MIME data onto the attachment model.
        \param data The MIME data being dropped.
        \param action The drop action.
        \param row The row index where the drop occurred.
        \param column The column index where the drop occurred.
        \param parent The parent model index.
        \return True if the data was successfully handled.
    */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    /*! \brief Returns MIME data for the selected attachments.
        \param indexes The list of selected model indexes.
        \return The MIME data containing the selected attachments.
    */
    [[nodiscard]] QMimeData *mimeData(const QModelIndexList &indexes) const override;
    /*! \brief Returns the list of supported MIME types for drag and drop. */
    [[nodiscard]] QStringList mimeTypes() const override;
    /*! \brief Returns the supported drop actions for the model. */
    [[nodiscard]] Qt::DropActions supportedDropActions() const override;

    /// for the save/discard warning
    [[nodiscard]] bool isModified() const;
    /*! \brief Sets whether the model has been modified and needs saving.
        \param modified True if modified, false otherwise.
    */
    void setModified(bool modified);

    /*! \brief Returns whether encryption is enabled for attachments. */
    [[nodiscard]] bool isEncryptEnabled() const;
    /*! \brief Sets whether encryption is enabled for attachments.
        \param enabled True to enable encryption.
    */
    void setEncryptEnabled(bool enabled);
    /*! \brief Returns whether signing is enabled for attachments. */
    [[nodiscard]] bool isSignEnabled() const;
    /*! \brief Sets whether signing is enabled for attachments.
        \param enabled True to enable signing.
    */
    void setSignEnabled(bool enabled);
    /*! \brief Returns whether encryption is selected for all attachments. */
    [[nodiscard]] bool isEncryptSelected() const;
    /// sets for all
    /*! \brief Sets encryption for all attachments.
        \param selected True to encrypt all attachments.
    */
    void setEncryptSelected(bool selected);
    /*! \brief Returns whether signing is selected for all attachments. */
    [[nodiscard]] bool isSignSelected() const;
    /// sets for all
    /*! \brief Sets signing for all attachments.
        \param selected True to sign all attachments.
    */
    void setSignSelected(bool selected);

    /*! \brief Returns whether automatic display is enabled for attachments. */
    [[nodiscard]] bool isAutoDisplayEnabled() const;
    /*! \brief Sets whether automatic display is enabled for attachments.
        \param enabled True to enable automatic display.
    */
    void setAutoDisplayEnabled(bool enabled);

    /*! \brief Returns data for the given model index and role.
        \param index The model index.
        \param role The data role to retrieve.
        \return The data at the specified index for the given role.
    */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*! \brief Sets data for the given model index and role.
        \param index The model index.
        \param value The value to set.
        \param role The data role to set.
        \return True if the data was successfully set.
    */
    [[nodiscard]] bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    /*! \brief Adds an attachment to the model.
        \param part The attachment part to add.
    */
    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    /*! \brief Updates an existing attachment in the model.
        \param part The attachment part to update.
        \return True if the update was successful.
    */
    bool updateAttachment(const MessageCore::AttachmentPart::Ptr &part);
    /*! \brief Replaces an old attachment with a new one.
        \param oldPart The old attachment part to replace.
        \param newPart The new attachment part.
        \return True if the replacement was successful.
    */
    [[nodiscard]] bool replaceAttachment(const MessageCore::AttachmentPart::Ptr &oldPart, const MessageCore::AttachmentPart::Ptr &newPart);
    /*! \brief Removes an attachment from the model.
        \param part The attachment part to remove.
        \return True if the removal was successful.
    */
    [[nodiscard]] bool removeAttachment(const MessageCore::AttachmentPart::Ptr &part);
    /*! \brief Returns the list of all attachments in the model. */
    [[nodiscard]] MessageCore::AttachmentPart::List attachments() const;

    /*! \brief Returns the flags for the given model index.
        \param index The model index.
        \return The item flags.
    */
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    /*! \brief Returns the header data for the given section.
        \param section The section index.
        \param orientation The header orientation.
        \param role The data role.
        \return The header data.
    */
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    /*! \brief Returns the model index at the given row and column.
        \param row The row index.
        \param column The column index.
        \param parent The parent model index.
        \return The model index.
    */
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    /*! \brief Returns the parent model index for the given index.
        \param index The child model index.
        \return The parent model index.
    */
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    /*! \brief Returns the number of rows in the model.
        \param parent The parent model index.
        \return The number of rows.
    */
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /*! \brief Returns the number of columns in the model.
        \param parent The parent model index.
        \return The number of columns.
    */
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;

Q_SIGNALS:
    /*! \brief Emitted when encryption is enabled or disabled.
        \param enabled True if encryption is now enabled.
    */
    void encryptEnabled(bool enabled);
    /*! \brief Emitted when signing is enabled or disabled.
        \param enabled True if signing is now enabled.
    */
    void signEnabled(bool enabled);
    /*! \brief Emitted when automatic display is enabled or disabled.
        \param enabled True if auto-display is now enabled.
    */
    void autoDisplayEnabled(bool enabled);
    /*! \brief Emitted when URLs should be attached to the message.
        \param urls The list of URLs to attach.
    */
    void attachUrlsRequested(const QList<QUrl> &urls);
    /*! \brief Emitted when items from Akonadi should be attached.
        \param items The list of Akonadi items to attach.
    */
    void attachItemsRequester(const Akonadi::Item::List &);
    /*! \brief Emitted when an attachment is removed.
        \param part The attachment part that was removed.
    */
    void attachmentRemoved(MessageCore::AttachmentPart::Ptr part);
    /*! \brief Emitted when an attachment compression state should change.
        \param part The attachment part to compress or decompress.
        \param compress True to compress, false to decompress.
    */
    void attachmentCompressRequested(MessageCore::AttachmentPart::Ptr part, bool compress);

private:
    class AttachmentModelPrivate;
    friend class AttachmentModelPrivate;
    std::unique_ptr<AttachmentModelPrivate> const d;
};
} //
