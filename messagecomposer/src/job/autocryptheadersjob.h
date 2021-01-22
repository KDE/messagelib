/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_AUTOCRYPTHEADERSJOB_H
#define MESSAGECOMPOSER_AUTOCRYPTHEADERSJOB_H

#include "messagecomposer_export.h"

#include <KJob>
#include <gpgme++/key.h>
#include <vector>

namespace KMime {
class Content;
class Message;
}

namespace MessageComposer {
class AutocryptHeadersJobPrivate;

/**
*/
class MESSAGECOMPOSER_EXPORT AutocryptHeadersJob : public KJob
{
    Q_OBJECT

public:
    explicit AutocryptHeadersJob(QObject *parent = nullptr);
    ~AutocryptHeadersJob() override;

    void setContent(KMime::Content *content);
    void setSkeletonMessage(KMime::Message *skeletonMessage);

    void setSenderKey(const GpgME::Key &key);
    void setPreferEncrypted(bool preferEncrypted);
    void setGossipKeys(const std::vector<GpgME::Key> &gossipKeys);
    
    void start() override;

protected Q_SLOTS:
    void doStart();

private:
    AutocryptHeadersJobPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(AutocryptHeadersJob)
};
}

#endif
