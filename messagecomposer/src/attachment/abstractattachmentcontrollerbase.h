/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 * SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
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
#include <KJob>
#include <KService>
#include <MessageCore/AttachmentPart>

namespace MessageComposer
{
class AttachmentModel;
/// \brief The AbstractAttachmentControllerBase class, this class will be used to configure
/// that files get sent as an attachment to a mail.
///
/// This class is suitable to be used directly from QML. See AttachmentControllerBase for a
/// QWidgets implentation.
class MESSAGECOMPOSER_EXPORT AbstractAttachmentControllerBase : public QObject
{
    Q_OBJECT

    /// Specify if message sent by the composer view should include the Vcard of the sender.
    Q_PROPERTY(bool attachOwnVcard READ attachOwnVcard WRITE setAttachOwnVcard NOTIFY attachOwnVcardChanged)

    /// True if the identity currently selected has an Vcard attached.
    Q_PROPERTY(bool identityHasOwnVcard READ identityHasOwnVcard WRITE setIdentityHasOwnVcard NOTIFY identityHasOwnVcardChanged)

    /// Specify if the message should be encrypted.
    Q_PROPERTY(bool encryptEnabled READ encryptEnabled WRITE setEncryptEnabled NOTIFY encryptEnabledChanged)

    /// Specify if the message should be signed.
    Q_PROPERTY(bool signEnabled READ signEnabled WRITE setSignEnabled NOTIFY signEnabledChanged)

    /// Specify if it is possible for the user to attach a public key to the message.
    Q_PROPERTY(bool canAttachPublicKey READ canAttachPublicKey WRITE enableAttachPublicKey NOTIFY canAttachPublicKeyChanged)

    /// Specify if it is possible for the user to attach own public key to the message.
    Q_PROPERTY(bool canAttachOwnPublicKey READ canAttachOwnPublicKey WRITE enableAttachMyPublicKey NOTIFY canAttachOwnPublicKeyChanged)

public:
    /// Create an AbstractAttachmentControllerBase
    /// \param model the AttachmentModel that will get manipulated.
    AbstractAttachmentControllerBase(MessageComposer::AttachmentModel *model, QObject *parent = nullptr);
    ~AbstractAttachmentControllerBase() override;

    /// Sets the selected attachments.
    void setSelectedParts(const MessageCore::AttachmentPart::List &selectedParts);

    void setAttachOwnVcard(bool attachVcard);
    Q_REQUIRED_RESULT bool attachOwnVcard() const;

    void setIdentityHasOwnVcard(bool state);
    Q_REQUIRED_RESULT bool identityHasOwnVcard() const;

    Q_REQUIRED_RESULT bool encryptEnabled() const;
    Q_REQUIRED_RESULT bool signEnabled() const;

    Q_REQUIRED_RESULT bool canAttachOwnPublicKey() const;
    void enableAttachMyPublicKey(bool enable);

    Q_REQUIRED_RESULT bool canAttachPublicKey() const;
    void enableAttachPublicKey(bool enable);

public Q_SLOTS:
    void setSignEnabled(bool enabled);
    void setEncryptEnabled(bool enabled);

    /// compression is async...
    void compressAttachment(const MessageCore::AttachmentPart::Ptr &part, bool compress);
    void openAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void viewAttachment(const MessageCore::AttachmentPart::Ptr &part);

    /// sets sign, encrypt, shows properties dialog if so configured
    void addAttachment(const MessageCore::AttachmentPart::Ptr &part);
    void addAttachment(const QUrl &url);
    void addAttachmentUrlSync(const QUrl &url);
    void addAttachments(const QList<QUrl> &urls);
    void showAttachClipBoard();
    virtual void attachMyPublicKey();

    /// Update the attachment, called when handling a compressedAttachmentLargerOriginalOccured signal.
    void overwriteAttachment(KJob *job);

Q_SIGNALS:
    void identityHasOwnVcardChanged();
    void attachOwnVcardChanged();
    void canAttachOwnPublicKeyChanged();
    void canAttachPublicKeyChanged();
    void encryptEnabledChanged();
    void signEnabledChanged();
    void refreshSelection();
    void showAttachment(KMime::Content *content, const QByteArray &charset);
    void selectedAllAttachment();
    void addOwnVcard(bool);
    void fileAttached();

    /// Happens when the compressed attachment is larger than the original.
    /// By default nothing happens but the user can force overwriting the original
    /// attachment by calling. overwriteAttachment(job)
    void compressedAttachmentLargerOriginalOccured(KJob *job);

    /// \brief Signal triggered when an error occured.
    /// \param text    Error message.
    /// \param caption Error title.
    void errorOccured(const QString &text, const QString &caption);

protected:
    void exportPublicKey(const QString &fingerprint);
    void byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite = false);
    virtual void openWith(const KService::Ptr &offer = KService::Ptr());
    void slotPutResult(KJob *job);
    void slotOpenWithDialog();

    class Private;

private:
    Private *const d;
};
} //
