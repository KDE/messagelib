/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "dkimchecksignaturejob.h"
#include "messagecore_private_export.h"
#include <QCryptographicHash>
#include <QString>
namespace MessageCore
{
namespace DKIMUtil
{
[[nodiscard]] MESSAGECORE_EXPORT QString bodyCanonizationRelaxed(QString body);
[[nodiscard]] MESSAGECORE_EXPORT QString bodyCanonizationSimple(QString body);
[[nodiscard]] MESSAGECORE_EXPORT QByteArray generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo);
[[nodiscard]] MESSAGECORE_EXPORT QString headerCanonizationSimple(const QString &headerName, const QString &headerValue);
[[nodiscard]] MESSAGECORE_EXPORT QString headerCanonizationRelaxed(const QString &headerName, const QString &headerValue, bool removeQuoteOnContentType);
[[nodiscard]] MESSAGECORE_EXPORT QString cleanString(QString str);
[[nodiscard]] MESSAGECORE_EXPORT QString emailDomain(const QString &emailDomain);
[[nodiscard]] MESSAGECORE_EXPORT QString emailSubDomain(const QString &emailDomain);
[[nodiscard]] MESSAGECORE_EXPORT QString defaultConfigFileName();
[[nodiscard]] MESSAGECORE_EXPORT QString convertAuthenticationMethodEnumToString(DKIMCheckSignatureJob::AuthenticationMethod);
[[nodiscard]] MESSAGECORE_EXPORT DKIMCheckSignatureJob::AuthenticationMethod convertAuthenticationMethodStringToEnum(const QString &str);
}
}
