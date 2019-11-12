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

#include "dkimauthenticationstatusinfo.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QRegularExpressionMatch>
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7601
DKIMAuthenticationStatusInfo::DKIMAuthenticationStatusInfo()
{
}

bool DKIMAuthenticationStatusInfo::parseAuthenticationStatus(const QString &key)
{
    // https://tools.ietf.org/html/rfc7601#section-2.2
    // authres-header = "Authentication-Results:" [CFWS] authserv-id
    //                                            [ CFWS authres-version ]
    //                                            ( no-result / 1*resinfo ) [CFWS] CRLF

    // 1) extract AuthservId and AuthVersion
    QRegularExpressionMatch match;
    const int index = key.indexOf(QRegularExpression(QStringLiteral("todo defined!")), 0, &match);
    if (index != -1) {
    }

    // 2) extract methodspec

    // 3) extract reasonspec (optional)

    // 4) extract propspec (optional)

    //TODO
    return true;
}

QString DKIMAuthenticationStatusInfo::authVersion() const
{
    return mAuthVersion;
}

void DKIMAuthenticationStatusInfo::setAuthVersion(const QString &authVersion)
{
    mAuthVersion = authVersion;
}

QString DKIMAuthenticationStatusInfo::reasonSpec() const
{
    return mReasonSpec;
}

void DKIMAuthenticationStatusInfo::setReasonSpec(const QString &reasonSpec)
{
    mReasonSpec = reasonSpec;
}

QString DKIMAuthenticationStatusInfo::authservId() const
{
    return mAuthservId;
}

void DKIMAuthenticationStatusInfo::setAuthservId(const QString &authservId)
{
    mAuthservId = authservId;
}
