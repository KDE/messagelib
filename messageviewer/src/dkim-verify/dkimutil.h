/*
   SPDX-FileCopyrightText: 2018-2022 Laurent Montel <montel@kde.org>

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
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString bodyCanonizationRelaxed(QString body);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString bodyCanonizationSimple(QString body);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QByteArray generateHash(const QByteArray &body, QCryptographicHash::Algorithm algo);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString headerCanonizationSimple(const QString &headerName, const QString &headerValue);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString headerCanonizationRelaxed(const QString &headerName,
                                                                               const QString &headerValue,
                                                                               bool removeQuoteOnContentType);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString cleanString(QString str);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString emailDomain(const QString &emailDomain);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString emailSubDomain(const QString &emailDomain);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString defaultConfigFileName();
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT QString convertAuthenticationMethodEnumToString(DKIMCheckSignatureJob::AuthenticationMethod);
Q_REQUIRED_RESULT MESSAGEVIEWER_TESTS_EXPORT DKIMCheckSignatureJob::AuthenticationMethod convertAuthenticationMethodStringToEnum(const QString &str);
}
}
