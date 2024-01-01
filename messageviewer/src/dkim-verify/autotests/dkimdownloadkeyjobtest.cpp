/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimdownloadkeyjobtest.h"
#include "dkim-verify/dkimdownloadkeyjob.h"
#include <QTest>
QTEST_MAIN(DKIMDownloadKeyJobTest)

DKIMDownloadKeyJobTest::DKIMDownloadKeyJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMDownloadKeyJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMDownloadKeyJob job;
    QVERIFY(job.domainName().isEmpty());
    QVERIFY(job.selectorName().isEmpty());
    QVERIFY(!job.canStart());
    QVERIFY(!job.dnsLookup());
}

void DKIMDownloadKeyJobTest::shouldTestCanStart()
{
    MessageViewer::DKIMDownloadKeyJob job;
    QVERIFY(!job.canStart());
    job.setDomainName(QStringLiteral("bla"));
    QVERIFY(!job.canStart());
    job.setSelectorName(QStringLiteral("bli"));
    QVERIFY(job.canStart());
    job.setDomainName(QString());
    QVERIFY(!job.canStart());
}

void DKIMDownloadKeyJobTest::shouldVerifyResolveDns()
{
    MessageViewer::DKIMDownloadKeyJob job;
    job.setDomainName(QStringLiteral("bla"));
    job.setSelectorName(QStringLiteral("bli"));
    QCOMPARE(job.resolvDnsValue(), QStringLiteral("bli._domainkey.bla"));
}

#include "moc_dkimdownloadkeyjobtest.cpp"
