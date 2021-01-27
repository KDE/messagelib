/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMAUTHENTICATIONSTATUSINFOCONVERTER_H
#define DKIMAUTHENTICATIONSTATUSINFOCONVERTER_H
#include "dkimauthenticationstatusinfo.h"
#include "dkimchecksignaturejob.h"
#include "messageviewer_private_export.h"

namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMAuthenticationStatusInfoConverter
{
public:
    DKIMAuthenticationStatusInfoConverter();
    ~DKIMAuthenticationStatusInfoConverter();

    Q_REQUIRED_RESULT MessageViewer::DKIMAuthenticationStatusInfo statusInfo() const;
    void setStatusInfo(const MessageViewer::DKIMAuthenticationStatusInfo &statusInfo);

    Q_REQUIRED_RESULT QVector<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> convert() const;

private:
    MessageViewer::DKIMAuthenticationStatusInfo mStatusInfo;
};
}
#endif // DKIMAUTHENTICATIONSTATUSINFOCONVERTER_H
