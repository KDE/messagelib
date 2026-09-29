/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimutiltest.h"

#include "dkim-verify/dkimutil.h"
#include <QTest>

using namespace Qt::Literals::StringLiterals;
QTEST_GUILESS_MAIN(DKIMUtilTest)
DKIMUtilTest::DKIMUtilTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMUtilTest::shouldTestBodyCanonizationRelaxed()
{
    QBENCHMARK {
        {
            QString ba =
                u"-- \nLaurent Montel | laurent.montel@kdab.com | KDE/Qt Senior Software Engineer \nKDAB (France) S.A.S., a KDAB Group company\nTel: France "
                u"+33 "
                "(0)4 90 84 08 53, http://www.kdab.fr\nKDAB - The Qt, C++ and OpenGL Experts\n\n\n"_s;
            QString result = MessageCore::DKIMUtil::bodyCanonizationRelaxed(ba);

            QCOMPARE(MessageCore::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "jnEyWN7LwPIBgES0mElYDek3lmyrRtSwUjDR2Ge08Xw=");
        }
        {
            QString ba = u"Bla bla\n\nbli\t\tblo\nTest\n\n\n\n\n"_s;
            QString result = MessageCore::DKIMUtil::bodyCanonizationRelaxed(ba);

            QCOMPARE(MessageCore::DKIMUtil::generateHash(result.toUtf8(), QCryptographicHash::Sha256), "DrwZwEC82qsIhJtHlq76T00vAUcrSrHbJh8wY5GTAws=");
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
    QCOMPARE(MessageCore::DKIMUtil::emailDomain(u"foo@kde.org"_s), u"kde.org"_s);
    QCOMPARE(MessageCore::DKIMUtil::emailDomain(u"foo@blo.bli.kde.org"_s), u"blo.bli.kde.org"_s);
}

void DKIMUtilTest::shouldVerifySubEmailDomain()
{
    {
        const QString email = u"goo@kde.org"_s;
        const QString domainName = MessageCore::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageCore::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
    {
        const QString email = u"goo@bla.bli.kde.org"_s;
        const QString domainName = MessageCore::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageCore::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
    {
        const QString email = u"goo@bli.kde.org"_s;
        const QString domainName = MessageCore::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageCore::DKIMUtil::emailSubDomain(domainName), u"kde.org"_s);
    }
    {
        const QString email = u"goo@sub.example.co.uk"_s;
        const QString domainName = MessageCore::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageCore::DKIMUtil::emailSubDomain(domainName), u"example.co.uk"_s);
    }
    {
        const QString email = u"goo@example.co.uk"_s;
        const QString domainName = MessageCore::DKIMUtil::emailDomain(email);
        QCOMPARE(MessageCore::DKIMUtil::emailSubDomain(domainName), u"example.co.uk"_s);
    }
}

void DKIMUtilTest::shouldConvertAuthenticationMethodEnumToString()
{
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Unknown), QString());
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dkim), u"dkim"_s);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Spf), u"spf"_s);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc), u"dmarc"_s);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps),
             u"dkim-atps"_s);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Auth), u"auth"_s);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodEnumToString(MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Arc), u"arc"_s);
}

void DKIMUtilTest::shouldConvertAuthenticationMethodToString()
{
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"arc"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Arc);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dkim"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dkim);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"spf"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Spf);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dmarc"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dmarc);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"dkim-atps"_s),
             MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"auth"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Auth);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(u"sdfsdf"_s), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
    QCOMPARE(MessageCore::DKIMUtil::convertAuthenticationMethodStringToEnum(QString()), MessageCore::DKIMCheckSignatureJob::AuthenticationMethod::Unknown);
}

#include "moc_dkimutiltest.cpp"
