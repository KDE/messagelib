/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#include "dkimutiltest.h"
#include "dkim-verify/dkimutil.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMUtilTest)
DKIMUtilTest::DKIMUtilTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMUtilTest::shouldTestBodyCanonizationRelaxed()
{
    {
        QString ba = QStringLiteral("-- \nLaurent Montel | laurent.montel@kdab.com | KDE/Qt Senior Software Engineer \nKDAB (France) S.A.S., a KDAB Group company\nTel: France +33 (0)4 90 84 08 53, http://www.kdab.fr\nKDAB - The Qt, C++ and OpenGL Experts\n\n\n");
        QString result = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(ba);

        QCOMPARE(MessageViewer::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "jnEyWN7LwPIBgES0mElYDek3lmyrRtSwUjDR2Ge08Xw=");
    }
    {
        QString ba = QStringLiteral("Bla bla\n\nbli\t\tblo\nTest\n\n\n\n\n");
        QString result = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(ba);

        QCOMPARE(MessageViewer::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "DrwZwEC82qsIhJtHlq76T00vAUcrSrHbJh8wY5GTAws=");
    }
}

void DKIMUtilTest::shouldVerifyEmailDomain()
{
    QCOMPARE(MessageViewer::DKIMUtil::emailDomain(QStringLiteral("foo@kde.org")), QStringLiteral("kde.org"));
    QCOMPARE(MessageViewer::DKIMUtil::emailDomain(QStringLiteral("foo@blo.bli.kde.org")), QStringLiteral("blo.bli.kde.org"));
}

void DKIMUtilTest::shouldVerifySubEmailDomain()
{
    {
        const QString email = QStringLiteral("goo@kde.org");
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), QStringLiteral("kde.org"));
    }
    {
        const QString email = QStringLiteral("goo@bla.bli.kde.org");
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), QStringLiteral("kde.org"));
    }
    {
        const QString email = QStringLiteral("goo@bli.kde.org");
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), QStringLiteral("kde.org"));
    }
}

void DKIMUtilTest::shouldConvertAuthenticationMethodEnumToString()
{
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown), QString());
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim), QStringLiteral("dkim"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf), QStringLiteral("spf"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc), QStringLiteral("dmarc"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps), QStringLiteral("dkim-atps"));
}

void DKIMUtilTest::shouldConvertAuthenticationMethodToString()
{
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dkim")), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("spf")), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dmarc")), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dkim-atps")), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("sdfsdf")), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QString()), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
}
