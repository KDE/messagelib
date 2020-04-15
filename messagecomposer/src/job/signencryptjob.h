/*
  Copyright (C) 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Leo Franchi <lfranchi@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef MESSAGECOMPOSER_SIGNENCRYPTJOB_H
#define MESSAGECOMPOSER_SIGNENCRYPTJOB_H

#include "abstractencryptjob.h"
#include "contentjobbase.h"
#include "part/infopart.h"
#include "messagecomposer_export.h"

#include <Libkleo/Enum>

#include <gpgme++/key.h>
#include <vector>

namespace KMime {
class Content;
}

namespace MessageComposer {
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

    Q_REQUIRED_RESULT std::vector<GpgME::Key> encryptionKeys() const override;
    Q_REQUIRED_RESULT QStringList recipients() const override;

    Q_REQUIRED_RESULT KMime::Content *origContent();

protected Q_SLOTS:
    void doStart() override;
    void slotResult(KJob *job) override;
    void process() override;

private:
    Q_DECLARE_PRIVATE(SignEncryptJob)
};
}

#endif
