/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimgeneraterulejobtest.h"
#include "dkim-verify/dkimgeneraterulejob.h"
#include <QTest>
QTEST_GUILESS_MAIN(DKIMGenerateRuleJobTest)

DKIMGenerateRuleJobTest::DKIMGenerateRuleJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMGenerateRuleJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMGenerateRuleJob job;
    QVERIFY(!job.canStart());
    QVERIFY(!job.result().isValid());
}
