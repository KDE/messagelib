/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "abstractencryptjob.h"
#include "contentjobbase.h"
#include "infopart.h"
#include "messagecomposer_export.h"

#include <Libkleo/Enum>

#include <gpgme++/key.h>
#include <vector>

namespace KMime
{
class Content;
}

namespace MessageComposer
{
class EncryptJobPrivate;

/**
  Encrypt the contents of a message .
  Used as a subjob of CryptoMessage
*/
class MESSAGECOMPOSER_EXPORT EncryptJob : public ContentJobBase, public MessageComposer::AbstractEncryptJob
{
    Q_OBJECT

public:
    explicit EncryptJob(QObject *parent = nullptr);
    ~EncryptJob() override;

    void setContent(KMime::Content *content);
    void setCryptoMessageFormat(Kleo::CryptoMessageFormat format);
    void setEncryptionKeys(const std::vector<GpgME::Key> &keys) override;
    void setRecipients(const QStringList &rec) override;
    void setSkeletonMessage(KMime::Message *skeletonMessage);

    void setProtectedHeaders(bool protectedHeaders);
    void setProtectedHeadersObvoscate(bool protectedHeadersObvoscate);

    void setGnupgHome(const QString &path);

    [[nodiscard]] std::vector<GpgME::Key> encryptionKeys() const override;
    [[nodiscard]] QStringList recipients() const override;

protected Q_SLOTS:
    void doStart() override;
    void slotResult(KJob *job) override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(EncryptJob)
};
}
