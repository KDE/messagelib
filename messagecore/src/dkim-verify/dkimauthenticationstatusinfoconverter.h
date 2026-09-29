/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "dkimauthenticationstatusinfo.h"
#include "dkimchecksignaturejob.h"
#include "messagecore_private_export.h"

namespace MessageCore
{
class MESSAGECORE_EXPORT DKIMAuthenticationStatusInfoConverter
{
public:
    DKIMAuthenticationStatusInfoConverter();
    ~DKIMAuthenticationStatusInfoConverter();

    [[nodiscard]] MessageCore::DKIMAuthenticationStatusInfo statusInfo() const;
    void setStatusInfo(const MessageCore::DKIMAuthenticationStatusInfo &statusInfo);

    [[nodiscard]] QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> convert() const;

private:
    MessageCore::DKIMAuthenticationStatusInfo mStatusInfo;
};
}
