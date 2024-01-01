/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcrecordjobtest.h"
#include "dkim-verify/dmarcrecordjob.h"
#include <QTest>
QTEST_GUILESS_MAIN(DMARCRecordJobTest)

DMARCRecordJobTest::DMARCRecordJobTest(QObject *parent)
    : QObject(parent)
{
}

void DMARCRecordJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DMARCRecordJob job;
    QVERIFY(!job.canStart());
    QVERIFY(job.domainName().isEmpty());
}

void DMARCRecordJobTest::shouldBeAbleToStart()
{
    MessageViewer::DMARCRecordJob job;
    QVERIFY(!job.canStart());
    job.setDomainName(QStringLiteral("bla"));
    QVERIFY(job.canStart());
    job.setDomainName(QString());
    QVERIFY(!job.canStart());
}

#include "moc_dmarcrecordjobtest.cpp"
