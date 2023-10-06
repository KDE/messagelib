/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"
#include <Libkleo/Enum>
#include <MessageComposer/InfoPart>

#include <gpgme++/key.h>
#include <vector>

namespace KMime
{
class Content;
}

namespace MessageComposer
{
class SignJobPrivate;

/**
  Signs the contents of a message.
  Used as a subjob of CryptoMessage
*/
class MESSAGECOMPOSER_EXPORT SignJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit SignJob(QObject *parent = nullptr);
    ~SignJob() override;

    void setContent(KMime::Content *content);
    void setCryptoMessageFormat(Kleo::CryptoMessageFormat format);
    void setSigningKeys(const std::vector<GpgME::Key> &signers);

    void setSkeletonMessage(KMime::Message *skeletonMessage);
    void setProtectedHeaders(bool protectedHeaders);

    [[nodiscard]] KMime::Content *origContent();

protected Q_SLOTS:
    void doStart() override;
    void slotResult(KJob *job) override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(SignJob)
};
}
