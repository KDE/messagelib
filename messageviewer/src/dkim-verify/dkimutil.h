/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

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
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString bodyCanonizationRelaxed(QString body);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString bodyCanonizationSimple(QString body);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QByteArray generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString headerCanonizationSimple(const QString &headerName, const QString &headerValue);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString headerCanonizationRelaxed(const QString &headerName,
                                                                               const QString &headerValue,
                                                                               bool removeQuoteOnContentType);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString cleanString(QString str);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString emailDomain(const QString &emailDomain);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString emailSubDomain(const QString &emailDomain);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString defaultConfigFileName();
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT QString convertAuthenticationMethodEnumToString(DKIMCheckSignatureJob::AuthenticationMethod);
MESSAGEVIEWER_TESTS_EXPORT Q_REQUIRED_RESULT DKIMCheckSignatureJob::AuthenticationMethod convertAuthenticationMethodStringToEnum(const QString &str);
}
}

