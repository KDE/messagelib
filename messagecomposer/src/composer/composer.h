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
class ComposerPrivate;
class GlobalPart;
class InfoPart;
class TextPart;
class ItipPart;

/**
 * @brief The Composer class
 */
class MESSAGECOMPOSER_EXPORT Composer : public JobBase
{
    Q_OBJECT

public:
    explicit Composer(QObject *parent = nullptr);
    ~Composer() override;

    [[nodiscard]] QList<KMime::Message::Ptr> resultMessages() const;

    [[nodiscard]] GlobalPart *globalPart() const;
    [[nodiscard]] InfoPart *infoPart() const;
    [[nodiscard]] TextPart *textPart() const;
    [[nodiscard]] ItipPart *itipPart() const;
    void clearTextPart();
    void clearItipPart();
    [[nodiscard]] MessageCore::AttachmentPart::List attachmentParts() const;
    void addAttachmentPart(MessageCore::AttachmentPart::Ptr part, bool autoresizeImage = false);
    void addAttachmentParts(const MessageCore::AttachmentPart::List &parts, bool autoresizeImage = false);
    void removeAttachmentPart(MessageCore::AttachmentPart::Ptr part);

    // if the message and attachments should not be encrypted regardless of settings
    void setNoCrypto(bool noCrypto);
    void setSignAndEncrypt(const bool doSign, const bool doEncrypt);
    void setCryptoMessageFormat(Kleo::CryptoMessageFormat format);
    void setSigningKeys(const std::vector<GpgME::Key> &signers);
    void setEncryptionKeys(const QList<QPair<QStringList, std::vector<GpgME::Key>>> &data);

    void setAutocryptEnabled(bool autocryptEnabled);
    void setSenderEncryptionKey(const GpgME::Key &senderKey);

    void setGnupgHome(const QString &path);
    [[nodiscard]] QString gnupgHome() const;

    /// Sets if this message being composed is an auto-saved message
    ///  if so, might need different handling, such as no crypto attachments.
    void setAutoSave(bool isAutoSave);
    [[nodiscard]] bool autoSave() const;

    [[nodiscard]] bool finished() const;

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
