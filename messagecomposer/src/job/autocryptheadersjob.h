/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

#include <gpgme++/key.h>
#include <vector>

namespace KMime
{
class Content;
class Message;
}

namespace MessageComposer
{
class AutocryptHeadersJobPrivate;

/**
 */
class MESSAGECOMPOSER_EXPORT AutocryptHeadersJob : public ContentJobBase
{
    Q_OBJECT

public:
    explicit AutocryptHeadersJob(QObject *parent = nullptr);
    ~AutocryptHeadersJob() override;

    void setContent(KMime::Content *content);
    void setSkeletonMessage(KMime::Message *skeletonMessage);

    void setGnupgHome(const QString &path);

    void setSenderKey(const GpgME::Key &key);
    void setPreferEncrypted(bool preferEncrypted);
    void setGossipKeys(const std::vector<GpgME::Key> &gossipKeys);

protected Q_SLOTS:
    void process() override;

private:
    Q_DECLARE_PRIVATE(AutocryptHeadersJob)
};
}
