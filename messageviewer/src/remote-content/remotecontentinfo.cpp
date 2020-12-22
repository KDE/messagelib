/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentinfo.h"
using namespace MessageViewer;
RemoteContentInfo::RemoteContentInfo()
{

}

RemoteContentInfo::~RemoteContentInfo()
{

}

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

void RemoteContentInfo::setStatus(const RemoteContentInfoStatus &status)
{
    mStatus = status;
}
