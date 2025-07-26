/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimutiltest.h"
using namespace Qt::Literals::StringLiterals;

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
            QString ba = u"Bla bla\n\nbli\t\tblo\nTest\n\n\n\n\n"_s;
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
    QCOMPARE(MessageViewer::DKIMUtil::emailDomain(u"foo@kde.org"_s), u"kde.org"_s);
    QCOMPARE(MessageViewer::DKIMUtil::emailDomain(u"foo@blo.bli.kde.org"_s), u"blo.bli.kde.org"_s);
}

void DKIMUtilTest::shouldVerifySubEmailDomain()
{
    {
        const QString email = u"goo@kde.org"_s;
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
    {
        const QString email = u"goo@bla.bli.kde.org"_s;
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
    {
        const QString email = u"goo@bli.kde.org"_s;
        const QString domainName = MessageViewer::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageViewer::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
}

void DKIMUtilTest::shouldConvertAuthenticationMethodEnumToString()
{
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown), QString());
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim), u"dkim"_s);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf), u"spf"_s);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc), u"dmarc"_s);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps),
             u"dkim-atps"_s);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth), u"auth"_s);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Arc), u"arc"_s);
}

void DKIMUtilTest::shouldConvertAuthenticationMethodToString()
{
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"arc"_s), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Arc);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dkim"_s), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkim);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"spf"_s), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dmarc"_s), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dkim-atps"_s),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"auth"_s), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Auth);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(u"sdfsdf"_s),
             MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
    QCOMPARE(MessageViewer::DKIMUtil::convertAuthenticationMethodStringToEnum(QString()), MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
}

#include "moc_dkimutiltest.cpp"
