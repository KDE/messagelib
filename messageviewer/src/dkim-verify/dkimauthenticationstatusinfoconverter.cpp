/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

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

#include "dkimauthenticationstatusinfoconverter.h"
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DKIMAuthenticationStatusInfoConverter::DKIMAuthenticationStatusInfoConverter()
{
}

DKIMAuthenticationStatusInfoConverter::~DKIMAuthenticationStatusInfoConverter()
{
}

MessageViewer::DKIMAuthenticationStatusInfo DKIMAuthenticationStatusInfoConverter::statusInfo() const
{
    return mStatusInfo;
}

void DKIMAuthenticationStatusInfoConverter::setStatusInfo(const MessageViewer::DKIMAuthenticationStatusInfo &statusInfo)
{
    mStatusInfo = statusInfo;
}

QVector<DKIMCheckSignatureJob::CheckSignatureResult> DKIMAuthenticationStatusInfoConverter::convert() const
{
    QVector<DKIMCheckSignatureJob::CheckSignatureResult> lstResult;
    const QVector<DKIMAuthenticationStatusInfo::AuthStatusInfo> lstInfo = mStatusInfo.listAuthStatusInfo();
    for (const DKIMAuthenticationStatusInfo::AuthStatusInfo &info : lstInfo) {
        DKIMCheckSignatureJob::CheckSignatureResult convertedResult;
        const QString &infoResult = info.result;
        if (infoResult == QLatin1String("none")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned;
        } else if (infoResult == QLatin1String("pass")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Valid;
            QString sdid;
            QString auid;
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.header) {
                if (prop.type == QLatin1String("d")) {
                    sdid = prop.value;
                } else if (prop.type == QLatin1String("i")) {
                    auid = prop.value;
                }
            }
            if (!auid.isEmpty() || !sdid.isEmpty()) {
                if (sdid.isEmpty()) {
                    sdid = MessageViewer::DKIMUtil::emailDomain(auid);
                } else if (auid.isEmpty()) {
                    auid = QLatin1Char('@') + sdid;
                }
                convertedResult.sdid = sdid;
                convertedResult.auid = auid;
            }
        } else if (infoResult == QLatin1String("fail") ||
                   infoResult == QLatin1String("policy") ||
                   infoResult == QLatin1String("neutral") ||
                   infoResult == QLatin1String("permerror")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
        } else if (infoResult == QLatin1String("temperror")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
        } else {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid result type " << infoResult;
            continue;
        }
        lstResult.append(convertedResult);
    }

    return lstResult;
}
