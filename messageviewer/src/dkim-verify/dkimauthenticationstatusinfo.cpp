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
#include "dkimauthenticationstatusinfoutil.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <QRegularExpressionMatch>
using namespace MessageViewer;
//see https://tools.ietf.org/html/rfc7601
DKIMAuthenticationStatusInfo::DKIMAuthenticationStatusInfo()
{
}

bool DKIMAuthenticationStatusInfo::parseAuthenticationStatus(const QString &key)
{
    QString valueKey = key;
    // https://tools.ietf.org/html/rfc7601#section-2.2
    // authres-header = "Authentication-Results:" [CFWS] authserv-id
    //                                            [ CFWS authres-version ]
    //                                            ( no-result / 1*resinfo ) [CFWS] CRLF

    // 1) extract AuthservId and AuthVersion
    QRegularExpressionMatch match;
    const QString regStr = DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1String("(?:") + DKIMAuthenticationStatusInfoUtil::cfws_p() + QLatin1String("([0-9]+)") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String(" )?");
    //qDebug() << " regStr" << regStr;
    int index = valueKey.indexOf(QRegularExpression(regStr), 0, &match);
    if (index != -1) {
        mAuthservId = match.captured(1);
        if (!match.captured(2).isEmpty()) {
            mAuthVersion = match.captured(2).toInt();
        } else {
            mAuthVersion = 1;
        }
        valueKey = valueKey.right(valueKey.length() - (index + match.captured(0).length()));
        qDebug() << " match.captured(0)"<<match.captured(0)<<"match.captured(1)" <<match.captured(1) << match.captured(2);
        qDebug() << " valueKey" << valueKey;
    } else {
        return false;
    }
    // check if message authentication was performed
    index = valueKey.indexOf(QRegularExpression(DKIMAuthenticationStatusInfoUtil::value_cp() + QLatin1String(";") + DKIMAuthenticationStatusInfoUtil::cfws_op() + QLatin1String("?none")), 0, &match);
    if (index != -1) {
        //no result
        return false;
    }




    // 2) extract methodspec

    // 3) extract reasonspec (optional)

    // 4) extract propspec (optional)

    return true;
}

int DKIMAuthenticationStatusInfo::authVersion() const
{
    return mAuthVersion;
}

void DKIMAuthenticationStatusInfo::setAuthVersion(int authVersion)
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

bool DKIMAuthenticationStatusInfo::operator==(const DKIMAuthenticationStatusInfo &other) const
{
    return mAuthservId == other.authservId() && mAuthVersion == other.authVersion() && mReasonSpec == other.reasonSpec();
}

QString DKIMAuthenticationStatusInfo::authservId() const
{
    return mAuthservId;
}

void DKIMAuthenticationStatusInfo::setAuthservId(const QString &authservId)
{
    mAuthservId = authservId;
}

QDebug operator <<(QDebug d, const DKIMAuthenticationStatusInfo &t)
{
    d << "mAuthservId: " << t.authservId();
    d << "mReasonSpec: " << t.reasonSpec();
    d << "mAuthVersion: " << t.authVersion();
    return d;
}
