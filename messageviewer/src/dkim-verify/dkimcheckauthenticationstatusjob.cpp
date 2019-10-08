/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimcheckauthenticationstatusjob.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DKIMCheckAuthenticationStatusJob::DKIMCheckAuthenticationStatusJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckAuthenticationStatusJob::~DKIMCheckAuthenticationStatusJob()
{
}

void DKIMCheckAuthenticationStatusJob::start()
{
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start job";
        deleteLater();
        return;
    }
    if (auto hrd = mMessage->headerByType("Authentication-Results")) {
        mAuthenticationResult = hrd->asUnicodeString();
    }
    if (mAuthenticationResult.isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Authencation result is empty";
        deleteLater();
        return;
    }
    //TODO
    deleteLater();
}

bool DKIMCheckAuthenticationStatusJob::canStart() const
{
    //TODO
    return false;
}

KMime::Message::Ptr DKIMCheckAuthenticationStatusJob::message() const
{
    return mMessage;
}

void DKIMCheckAuthenticationStatusJob::setMessage(const KMime::Message::Ptr &message)
{
    mMessage = message;
}

QString DKIMCheckAuthenticationStatusJob::authenticationResult() const
{
    return mAuthenticationResult;
}

void DKIMCheckAuthenticationStatusJob::setAuthenticationResult(const QString &authenticationResult)
{
    mAuthenticationResult = authenticationResult;
}
