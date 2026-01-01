/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcrecordjobtest.h"
using namespace Qt::Literals::StringLiterals;

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
    job.setDomainName(u"bla"_s);
    QVERIFY(job.canStart());
    job.setDomainName(QString());
    QVERIFY(!job.canStart());
}

#include "moc_dmarcrecordjobtest.cpp"
