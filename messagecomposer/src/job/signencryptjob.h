/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "abstractencryptjob.h"
#include "contentjobbase.h"
#include "messagecomposer_export.h"
#include "part/infopart.h"

#include <Libkleo/Enum>

#include <gpgme++/key.h>
#include <vector>

namespace KMime
{
class Content;
}

namespace MessageComposer
{
class SignEncryptJobPrivate;

/**
  Signs and encrypt the contents of a message.
  Used when doing inline pgp sign+encrypt
*/
class MESSAGECOMPOSER_EXPORT SignEncryptJob : public ContentJobBase, public MessageComposer::AbstractEncryptJob
{
    Q_OBJECT

public:
    explicit SignEncryptJob(QObject *parent = nullptr);
    ~SignEncryptJob() override;

    void setContent(KMime::Content *content);
    void setCryptoMessageFormat(Kleo::CryptoMessageFormat format);
    void setSigningKeys(const std::vector<GpgME::Key> &signers);

    void setEncryptionKeys(const std::vector<GpgME::Key> &keys) override;
    void setRecipients(const QStringList &rec) override;

    void setSkeletonMessage(KMime::Message *skeletonMessage);

    void setProtectedHeaders(bool protectedHeaders);
    void setProtectedHeadersObvoscate(bool protectedHeadersObvoscate);

    [[nodiscard]] std::vector<GpgME::Key> encryptionKeys() const override;
    [[nodiscard]] QStringList recipients() const override;

    [[nodiscard]] KMime::Content *origContent();

protected Q_SLOTS:
    void doStart() override;
    void slotResult(KJob *job) override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(SignEncryptJob)
};
}
