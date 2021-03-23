/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "MessageComposer/JobBase"
#include <Libkleo/Enum>

#include "messagecomposer_export.h"
#include <QStringList>
#include <QVector>

#include <kmime/kmime_message.h>

#include <MessageCore/AttachmentPart>

#include <gpgme++/key.h>
#include <vector>

namespace MessageComposer
{
class ComposerPrivate;
class GlobalPart;
class InfoPart;
class TextPart;

/**
 * @brief The Composer class
 */
class MESSAGECOMPOSER_EXPORT Composer : public JobBase
{
    Q_OBJECT

public:
    explicit Composer(QObject *parent = nullptr);
    ~Composer() override;

    Q_REQUIRED_RESULT QVector<KMime::Message::Ptr> resultMessages() const;

    Q_REQUIRED_RESULT GlobalPart *globalPart() const;
    Q_REQUIRED_RESULT InfoPart *infoPart() const;
    Q_REQUIRED_RESULT TextPart *textPart() const;
    Q_REQUIRED_RESULT MessageCore::AttachmentPart::List attachmentParts() const;
    void addAttachmentPart(MessageCore::AttachmentPart::Ptr part, bool autoresizeImage = false);
    void addAttachmentParts(const MessageCore::AttachmentPart::List &parts, bool autoresizeImage = false);
    void removeAttachmentPart(MessageCore::AttachmentPart::Ptr part);

    // if the message and attachments should not be encrypted regardless of settings
    void setNoCrypto(bool noCrypto);
    void setSignAndEncrypt(const bool doSign, const bool doEncrypt);
    void setMessageCryptoFormat(Kleo::CryptoMessageFormat format);
    void setSigningKeys(std::vector<GpgME::Key> &signers);
    void setEncryptionKeys(const QVector<QPair<QStringList, std::vector<GpgME::Key>>> &data);

    void setAutocryptEnabled(bool autocryptEnabled);
    void setSenderEncryptionKey(const GpgME::Key &senderKey);

    void setGnupgHome(const QString &path);
    Q_REQUIRED_RESULT QString gnupgHome() const;

    /// Sets if this message being composed is an auto-saved message
    ///  if so, might need different handling, such as no crypto attachments.
    void setAutoSave(bool isAutoSave);
    Q_REQUIRED_RESULT bool autoSave() const;

    Q_REQUIRED_RESULT bool finished() const;

public Q_SLOTS:
    void start() override;

protected Q_SLOTS:
    void slotResult(KJob *job) override;

private:
    Q_DECLARE_PRIVATE(Composer)

    Q_PRIVATE_SLOT(d_func(), void doStart())
    Q_PRIVATE_SLOT(d_func(), void contentJobFinished(KJob *))
    Q_PRIVATE_SLOT(d_func(), void attachmentsFinished(KJob *))
};
}

