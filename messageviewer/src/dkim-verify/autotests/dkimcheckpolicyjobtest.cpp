/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckpolicyjobtest.h"
#include "dkim-verify/dkimcheckpolicyjob.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMCheckPolicyJobTest)
DKIMCheckPolicyJobTest::DKIMCheckPolicyJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMCheckPolicyJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMCheckPolicyJob job;
    QVERIFY(job.emailAddress().isEmpty());
    QVERIFY(!job.canStart());
    QVERIFY(!job.checkResult().isValid());
}

#include "moc_dkimcheckpolicyjobtest.cpp"
