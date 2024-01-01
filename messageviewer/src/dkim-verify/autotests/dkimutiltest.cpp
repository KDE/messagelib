/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    QBENCHMARK {
        {
            QString ba = QStringLiteral(
                "-- \nLaurent Montel | laurent.montel@kdab.com | KDE/Qt Senior Software Engineer \nKDAB (France) S.A.S., a KDAB Group company\nTel: France +33 "
                "(0)4 90 84 08 53, http://www.kdab.fr\nKDAB - The Qt, C++ and OpenGL Experts\n\n\n");
            QString result = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(ba);

            QCOMPARE(MessageViewer::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "jnEyWN7LwPIBgES0mElYDek3lmyrRtSwUjDR2Ge08Xw=");
        }
        {
            QString ba = QStringLiteral("Bla bla\n\nbli\t\tblo\nTest\n\n\n\n\n");
            QString result = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(ba);

            QCOMPARE(MessageViewer::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "DrwZwEC82qsIhJtHlq76T00vAUcrSrHbJh8wY5GTAws=");
        }
    }
    //    BEFORE
    //    RESULT : DKIMUtilTest::shouldTestBodyCanonizationRelaxed():
    //      0.087 msecs per iteration (total: 90, iterations: 1024)

    //    AFTER
    //            RESULT : DKIMUtilTest::shouldTestBodyCanonizationRelaxed():
    //      0.014 msecs per iteration (total: 59, iterations: 4096)
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
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim),
             QStringLiteral("dkim"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf),
             QStringLiteral("spf"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc),
             QStringLiteral("dmarc"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps),
             QStringLiteral("dkim-atps"));
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth),
             QStringLiteral("auth"));
}

void DKIMUtilTest::shouldConvertAuthenticationMethodToString()
{
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dkim")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("spf")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dmarc")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("dkim-atps")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("auth")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QStringLiteral("sdfsdf")),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QString()), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
}

#include "moc_dkimutiltest.cpp"
