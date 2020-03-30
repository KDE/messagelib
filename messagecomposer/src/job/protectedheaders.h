/*
  Copyright (C) 2020 Sandro Knauß <sknauss@kde.org>

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

#ifndef MESSAGECOMPOSER_PROTECTEDHEADERSJOB_H
#define MESSAGECOMPOSER_PROTECTEDHEADERSJOB_H

#include "contentjobbase.h"
#include "infopart.h"
#include "messagecomposer_export.h"

namespace KMime {
class Content;
}

namespace MessageComposer {
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

#endif
