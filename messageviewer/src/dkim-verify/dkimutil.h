/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "dkimchecksignaturejob.h"
#include "messageviewer_private_export.h"
#include <QCryptographicHash>
#include <QString>
namespace MessageViewer
{
namespace DKIMUtil
{
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString bodyCanonizationRelaxed(QString body);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString bodyCanonizationSimple(QString body);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QByteArray generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString headerCanonizationSimple(const QString &headerName, const QString &headerValue);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString headerCanonizationRelaxed(const QString &headerName,
                                                                           const QString &headerValue,
                                                                           bool removeQuoteOnContentType);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString cleanString(QString str);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString emailDomain(const QString &emailDomain);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString emailSubDomain(const QString &emailDomain);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString defaultConfigFileName();
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT QString convertAuthenticationMethodEnumToString(DKIMCheckSignatureJob::AuthenticationMethod);
[[nodiscard]] MESSAGEVIEWER_TESTS_EXPORT DKIMCheckSignatureJob::AuthenticationMethod convertAuthenticationMethodStringToEnum(const QString &str);
}
}
