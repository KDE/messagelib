/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentinfo.h"
using namespace MessageViewer;
RemoteContentInfo::RemoteContentInfo() = default;

RemoteContentInfo::~RemoteContentInfo() = default;

QString RemoteContentInfo::url() const
{
    return mUrl;
}

void RemoteContentInfo::setUrl(const QString &url)
{
    mUrl = url;
}

RemoteContentInfo::RemoteContentInfoStatus RemoteContentInfo::status() const
{
    return mStatus;
}

void RemoteContentInfo::setStatus(RemoteContentInfoStatus status)
{
    mStatus = status;
}

bool RemoteContentInfo::isValid() const
{
    return !mUrl.isEmpty() && (mStatus != RemoteContentInfoStatus::Unknown);
}

QDebug operator<<(QDebug d, const MessageViewer::RemoteContentInfo &t)
{
    d << "mUrl " << t.url();
    d << "mStatus " << t.status();
    return d;
}

#include "moc_remotecontentinfo.cpp"
