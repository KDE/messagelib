/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "MessageComposer/JobBase"
#include <Libkleo/Enum>

#include "messagecomposer_export.h"
#include <QList>
#include <QStringList>

#include <KMime/Message>

#include <MessageCore/AttachmentPart>

#include <gpgme++/key.h>
#include <vector>

namespace MessageComposer
{
class ComposerJobPrivate;
class GlobalPart;
class InfoPart;
class TextPart;
class ItipPart;

/*!
 \class MessageComposer::ComposerJob
 \inmodule MessageComposer
 \inheaderfile MessageComposer/ComposerJob

 The Composer job.
 */
class MESSAGECOMPOSER_EXPORT ComposerJob : public JobBase
{
    Q_OBJECT

public:
    /*! \brief Constructs a ComposerJob.
        \param parent The parent object.
    */
    explicit ComposerJob(QObject *parent = nullptr);
    /*! \brief Destroys the ComposerJob. */
    ~ComposerJob() override;

    /*! \brief Returns the list of composed MIME messages. */
    [[nodiscard]] QList<std::shared_ptr<KMime::Message>> resultMessages() const;

    /*! \brief Returns the global part containing global message settings. */
    [[nodiscard]] GlobalPart *globalPart() const;
    /*! \brief Returns the info part containing header information. */
    [[nodiscard]] InfoPart *infoPart() const;
    /*! \brief Returns the text part containing message body. */
    [[nodiscard]] TextPart *textPart() const;
    /*! \brief Returns the iCalendar invitation part. */
    [[nodiscard]] ItipPart *itipPart() const;
    /*! \brief Clears the text part content. */
    void clearTextPart();
    /*! \brief Clears the iCalendar invitation part. */
    void clearItipPart();
    /*! \brief Returns the list of attachment parts. */
    [[nodiscard]] MessageCore::AttachmentPart::List attachmentParts() const;
    /*! \brief Adds an attachment part to the message.
        \param part The attachment part to add.
        \param autoresizeImage Whether to automatically resize the image.
    */
    void addAttachmentPart(MessageCore::AttachmentPart::Ptr part, bool autoresizeImage = false);
    /*! \brief Adds multiple attachment parts to the message.
        \param parts The list of attachment parts to add.
        \param autoresizeImage Whether to automatically resize images.
    */
    void addAttachmentParts(const MessageCore::AttachmentPart::List &parts, bool autoresizeImage = false);
    /*! \brief Removes an attachment part from the message.
        \param part The attachment part to remove.
    */
    void removeAttachmentPart(MessageCore::AttachmentPart::Ptr part);

    // if the message and attachments should not be encrypted regardless of settings
    /*! \brief Sets whether to skip encryption and signing for this message.
        \param noCrypto True to disable cryptography.
    */
    void setNoCrypto(bool noCrypto);
    /*! \brief Sets whether the message should be signed and/or encrypted.
        \param doSign True to sign the message.
        \param doEncrypt True to encrypt the message.
    */
    void setSignAndEncrypt(const bool doSign, const bool doEncrypt);
    /*! \brief Sets the cryptographic message format to use.
        \param format The message format (e.g., OpenPGP, S/MIME).
    */
    void setCryptoMessageFormat(Kleo::CryptoMessageFormat format);
    /*! \brief Sets the keys to use for signing the message.
        \param signers The list of keys to sign with.
    */
    void setSigningKeys(const std::vector<GpgME::Key> &signers);
    /*! \brief Sets the keys to use for encrypting the message.
        \param data List of recipient key lists.
    */
    void setEncryptionKeys(const QList<QPair<QStringList, std::vector<GpgME::Key>>> &data);

    /*! \brief Enables or disables Autocrypt support for this message.
        \param autocryptEnabled True to enable Autocrypt.
    */
    void setAutocryptEnabled(bool autocryptEnabled);
    /*! \brief Sets the sender's encryption key for Autocrypt.
        \param senderKey The sender's OpenPGP key.
    */
    void setSenderEncryptionKey(const GpgME::Key &senderKey);

    /*! \brief Sets the GnuPG home directory to use.
        \param path The path to the GnuPG home directory.
    */
    void setGnupgHome(const QString &path);
    /*! \brief Returns the GnuPG home directory path. */
    [[nodiscard]] QString gnupgHome() const;

    /// Sets if this message being composed is an auto-saved message
    ///  if so, might need different handling, such as no crypto attachments.
    void setAutoSave(bool isAutoSave);
    /*! \brief Returns whether this message is being auto-saved. */
    [[nodiscard]] bool autoSave() const;

    /*! \brief Returns whether the job has finished composing the message. */
    [[nodiscard]] bool finished() const;

public Q_SLOTS:
    /*! \brief Starts the composer job asynchronously. */
    void start() override;

protected Q_SLOTS:
    /*! \brief Called when a sub-job has finished.
        \param job The finished job.
    */
    void slotResult(KJob *job) override;

private:
    Q_DECLARE_PRIVATE(ComposerJob)

    Q_PRIVATE_SLOT(d_func(), void doStart())
};
}
