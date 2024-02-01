/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoconverter.h"
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace MessageViewer;

DKIMAuthenticationStatusInfoConverter::DKIMAuthenticationStatusInfoConverter() = default;

DKIMAuthenticationStatusInfoConverter::~DKIMAuthenticationStatusInfoConverter() = default;

MessageViewer::DKIMAuthenticationStatusInfo DKIMAuthenticationStatusInfoConverter::statusInfo() const
{
    return mStatusInfo;
}

void DKIMAuthenticationStatusInfoConverter::setStatusInfo(const MessageViewer::DKIMAuthenticationStatusInfo &statusInfo)
{
    mStatusInfo = statusInfo;
}

QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> DKIMAuthenticationStatusInfoConverter::convert() const
{
    QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> lstResult;
    const QList<DKIMAuthenticationStatusInfo::AuthStatusInfo> lstInfo = mStatusInfo.listAuthStatusInfo();
    for (const DKIMAuthenticationStatusInfo::AuthStatusInfo &info : lstInfo) {
        DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult convertedResult;
        convertedResult.method = MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(info.method);
        const QString &infoResult = info.result;
        convertedResult.infoResult = infoResult;
        if (infoResult == QLatin1StringView("none")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned;
        } else if (infoResult == QLatin1StringView("pass")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Valid;
            QString sdid;
            QString auid;
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.header) {
                if (prop.type == QLatin1Char('d')) {
                    sdid = prop.value;
                } else if (prop.type == QLatin1Char('i')) {
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
        } else if (infoResult == QLatin1StringView("fail") || infoResult == QLatin1StringView("policy") || infoResult == QLatin1StringView("neutral")
                   || infoResult == QLatin1StringView("permerror")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
            if (!info.reason.isEmpty()) {
                convertedResult.errorStr = info.reason;
            }
        } else if (infoResult == QLatin1StringView("temperror")) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
            if (!info.reason.isEmpty()) {
                convertedResult.errorStr = info.reason;
            }
        } else {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid result type " << infoResult;
            continue;
        }
        lstResult.append(convertedResult);
    }

    return lstResult;
}
