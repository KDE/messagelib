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

#include <QObject>

#include <QUrl>

#include "messagecomposer_export.h"
#include <KService>
#include <MessageCore/AttachmentPart>
#include <MessageViewer/EditorWatcher>

class KActionCollection;
class QAction;
class KJob;
namespace MessageComposer
{
class AttachmentModel;
/**
 * @brief The AttachmentControllerBase class
 */
class MESSAGECOMPOSER_EXPORT AttachmentControllerBase : public QObject
{
    Q_OBJECT

public:
    AttachmentControllerBase(MessageComposer::AttachmentModel *model, QWidget *wParent, KActionCollection *actionCollection);
    ~AttachmentControllerBase() override;

    void createActions();

    // TODO dnd stuff...

    void setSelectedParts(const MessageCore::AttachmentPart::List &selectedParts);

    void setAttachOwnVcard(bool attachVcard);
    Q_REQUIRED_RESULT bool attachOwnVcard() const;
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
                        MessageViewer::EditorWatcher::OpenWithOption option = MessageViewer::EditorWatcher::NoOpenWithDialog);
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
    void actionsCreated();
    void refreshSelection();
    void showAttachment(KMime::Content *content, const QByteArray &charset);
    void selectedAllAttachment();
    void addOwnVcard(bool);
    void fileAttached();

protected:
    void exportPublicKey(const QString &fingerprint);
    void enableAttachPublicKey(bool enable);
    void enableAttachMyPublicKey(bool enable);
    void byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite = false);
    void openWith(const KService::Ptr &offer = KService::Ptr());

private:
    void attachDirectory(const QUrl &url);
    void slotPutResult(KJob *job);
    void slotOpenWithDialog();
    void slotOpenWithAction(QAction *act);

private:
    class Private;
    Private *const d;
};
} //

