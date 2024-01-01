/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcpolicyjobtest.h"
#include "dkim-verify/dmarcpolicyjob.h"
#include <QTest>
QTEST_GUILESS_MAIN(DMARCPolicyJobTest)

DMARCPolicyJobTest::DMARCPolicyJobTest(QObject *parent)
    : QObject(parent)
{
}

void DMARCPolicyJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DMARCPolicyJob job;
    QVERIFY(job.emailAddress().isEmpty());
    QVERIFY(!job.canStart());
    // TODO
}

#include "moc_dmarcpolicyjobtest.cpp"
