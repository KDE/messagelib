/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace KMime
{
class Content;
class Message;
}

namespace MessageComposer
{
class ProtectedHeadersJobPrivate;

/**
  Copies headers from skeleton message to content.
  It is used for Protected Headers for Cryptographic E-mail
  currently a draft for RFC:
  https://datatracker.ietf.org/doc/draft-autocrypt-lamps-protected-headers/
  Used as a subjob of EncryptJob/SignJob/SignEncryptJob
*/
class MESSAGECOMPOSER_EXPORT ProtectedHeadersJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit ProtectedHeadersJob(QObject *parent = nullptr);
    ~ProtectedHeadersJob() override;

    void setContent(KMime::Content *content);
    void setSkeletonMessage(KMime::Message *skeletonMessage);

    void setObvoscate(bool obvoscate);

protected Q_SLOTS:
    void doStart() override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(ProtectedHeadersJob)
};
}
