/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimdownloadkeyjobtest.h"
using namespace Qt::Literals::StringLiterals;

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
    job.setDomainName(u"bla"_s);
    QVERIFY(!job.canStart());
    job.setSelectorName(u"bli"_s);
    QVERIFY(job.canStart());
    job.setDomainName(QString());
    QVERIFY(!job.canStart());
}

void DKIMDownloadKeyJobTest::shouldVerifyResolveDns()
{
    MessageViewer::DKIMDownloadKeyJob job;
    job.setDomainName(u"bla"_s);
    job.setSelectorName(u"bli"_s);
    QCOMPARE(job.resolvDnsValue(), u"bli._domainkey.bla"_s);
}

#include "moc_dkimdownloadkeyjobtest.cpp"
