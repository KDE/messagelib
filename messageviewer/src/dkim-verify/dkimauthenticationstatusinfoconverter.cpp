/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoconverter.h"

#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
using namespace Qt::Literals::StringLiterals;
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
        const QString infoResult = info.result.toLower();
        convertedResult.infoResult = infoResult;
        if (infoResult == "none"_L1) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned;
        } else if (infoResult == "pass"_L1) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Valid;
            QString sdid;
            QString auid;
            for (const DKIMAuthenticationStatusInfo::AuthStatusInfo::Property &prop : info.header) {
                if (prop.type == u'd') {
                    sdid = prop.value;
                } else if (prop.type == u'i') {
                    auid = prop.value;
                }
            }
            if (!auid.isEmpty() || !sdid.isEmpty()) {
                if (sdid.isEmpty()) {
                    sdid = MessageViewer::DKIMUtil::emailDomain(auid);
                } else if (auid.isEmpty()) {
                    auid = u'@' + sdid;
                }
                convertedResult.sdid = std::move(sdid);
                convertedResult.auid = std::move(auid);
            }
        } else if (infoResult == "fail"_L1 || infoResult == "policy"_L1 || infoResult == "neutral"_L1 || infoResult == "softfail"_L1
                   || infoResult == "permerror"_L1) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
            if (!info.reason.isEmpty()) {
                convertedResult.errorStr = info.reason;
            }
        } else if (infoResult == "temperror"_L1) {
            convertedResult.status = DKIMCheckSignatureJob::DKIMStatus::Invalid;
            if (!info.reason.isEmpty()) {
                convertedResult.errorStr = info.reason;
            }
        } else {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Invalid result type " << infoResult;
            continue;
        }
        lstResult.append(std::move(convertedResult));
    }

    return lstResult;
}
