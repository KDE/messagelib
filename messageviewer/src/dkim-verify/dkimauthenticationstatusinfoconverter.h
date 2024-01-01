/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
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

    [[nodiscard]] MessageViewer::DKIMAuthenticationStatusInfo statusInfo() const;
    void setStatusInfo(const MessageViewer::DKIMAuthenticationStatusInfo &statusInfo);

    [[nodiscard]] QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> convert() const;

private:
    MessageViewer::DKIMAuthenticationStatusInfo mStatusInfo;
};
}
