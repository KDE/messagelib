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
#include "dkimauthenticationstatusinfo.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7601
DKIMCheckAuthenticationStatusJob::DKIMCheckAuthenticationStatusJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckAuthenticationStatusJob::~DKIMCheckAuthenticationStatusJob()
{
}

void DKIMCheckAuthenticationStatusJob::start()
{
    //TODO we need to parse all headers! not just one !
    if (!canStart()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start job";
        deleteLater();
        Q_EMIT result({}, mItem);
        return;
    }
    DKIMAuthenticationStatusInfo info;
    if (!info.parseAuthenticationStatus(mAuthenticationResult)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to parse authentication status header";
        Q_EMIT result({}, mItem);
        deleteLater();
        return;
    }

    Q_EMIT result(info, mItem);
    deleteLater();
}

bool DKIMCheckAuthenticationStatusJob::canStart() const
{
    return !mAuthenticationResult.isEmpty();
}

QString DKIMCheckAuthenticationStatusJob::authenticationResult() const
{
    return mAuthenticationResult;
}

void DKIMCheckAuthenticationStatusJob::setAuthenticationResult(const QString &authenticationResult)
{
    mAuthenticationResult = authenticationResult;
}

Akonadi::Item DKIMCheckAuthenticationStatusJob::item() const
{
    return mItem;
}

void DKIMCheckAuthenticationStatusJob::setItem(const Akonadi::Item &item)
{
    mItem = item;
}
