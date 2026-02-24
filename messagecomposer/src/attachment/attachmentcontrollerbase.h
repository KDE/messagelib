/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "messagecomposer_export.h"

#include "editorwatcher.h"

#include <QObject>
#include <QUrl>

#include <KService>
#include <MessageCore/AttachmentPart>

class KActionCollection;
class QAction;
class KJob;
namespace MessageComposer
{
class AttachmentModel;
/*!
 * \class MessageComposer::AttachmentControllerBase
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/AttachmentControllerBase
 * \brief The AttachmentControllerBase class
 */
class MESSAGECOMPOSER_EXPORT AttachmentControllerBase : public QObject
{
    Q_OBJECT

public:
    /*! \brief Constructs an AttachmentControllerBase.
        \param model The attachment model to use.
        \param wParent The parent widget.
        \param actionCollection The action collection for menu actions.
    */
    AttachmentControllerBase(MessageComposer::AttachmentModel *model, QWidget *wParent, KActionCollection *actionCollection);
    /*! \brief Destroys the AttachmentControllerBase. */
    ~AttachmentControllerBase() override;

    /*! \brief Creates and initializes the attachment-related actions. */
    void createActions();

    // TODO dnd stuff...

    /*! \brief Sets the list of currently selected attachment parts.
        \param selectedParts The list of selected parts.
    */
    void setSelectedParts(const MessageCore::AttachmentPart::List &selectedParts);

    /*! \brief Enables or disables attaching the user's own vcard.
        \param attachVcard True to attach the user's vcard.
    */
    void setAttachOwnVcard(bool attachVcard);
    /*! \brief Returns whether the user's own vcard is attached. */
    [[nodiscard]] bool attachOwnVcard() const;
    /*! \brief Sets whether the current identity has its own vcard.
        \param state True if the identity has a vcard.
    */
    void setIdentityHasOwnVcard(bool state);

public Q_SLOTS:
    /// model sets these
    void setEncryptEnabled(bool enabled);
    void setSignEnabled(bool enabled);
    /// compression is async...
    void compressAttachment(const MessageCore::AttachmentPart::Ptr &part, bool compress);
    void showContextMenu();
    void openAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void viewAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void editAttachment(MessageCore::AttachmentPart::Ptr part,
                        MessageComposer::EditorWatcher::OpenWithOption option = MessageComposer::EditorWatcher::NoOpenWithDialog);
    void editAttachmentWith(const MessageCore::AttachmentPart::Ptr &part);
    void saveAttachmentAs(const MessageCore::AttachmentPart::Ptr &part);
    void attachmentProperties(const MessageCore::AttachmentPart::Ptr &part);
    void showAddAttachmentFileDialog();
    void showAddAttachmentCompressedDirectoryDialog();
    /// sets sign, encrypt, shows properties dialog if so configured
    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void addAttachment(const QUrl &url);
    void addAttachmentUrlSync(const QUrl &url);
    void addAttachments(const QList<QUrl> &urls);
    void showAttachPublicKeyDialog();
    void showAttachVcard();
    void showAttachClipBoard();
    virtual void attachMyPublicKey();

Q_SIGNALS:
    /*! \brief Emitted when the attachment actions have been created. */
    void actionsCreated();
    /*! \brief Emitted when the selection needs to be refreshed. */
    void refreshSelection();
    /*! \brief Emitted when an attachment should be displayed.
        \param content The MIME content to display.
        \param charset The character set of the content.
    */
    void showAttachment(const std::shared_ptr<KMime::Content> &content, const QByteArray &charset);
    /*! \brief Emitted when all attachments are selected. */
    void selectedAllAttachment();
    /*! \brief Emitted when the user's vcard should be added.
        \param attach True if the vcard should be added.
    */
    void addOwnVcard(bool);
    /*! \brief Emitted when a file has been attached. */
    void fileAttached();

protected:
    void exportPublicKey(const QString &fingerprint);
    void enableAttachPublicKey(bool enable);
    void enableAttachMyPublicKey(bool enable);
    void byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite = false);
    void openWith(const KService::Ptr &offer = KService::Ptr());

private:
    MESSAGECOMPOSER_NO_EXPORT void attachDirectory(const QUrl &url);
    MESSAGECOMPOSER_NO_EXPORT void slotPutResult(KJob *job);
    MESSAGECOMPOSER_NO_EXPORT void slotOpenWithDialog();
    MESSAGECOMPOSER_NO_EXPORT void slotOpenWithAction(QAction *act);

private:
    class AttachmentControllerBasePrivate;
    std::unique_ptr<AttachmentControllerBasePrivate> const d;
};
} //
