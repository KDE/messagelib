/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mdnwarningwidgetjobtest.h"
#include "messagecomposer/mdnwarningwidgetjob.h"
#include <QTest>
QTEST_GUILESS_MAIN(MDNWarningWidgetJobTest)
MDNWarningWidgetJobTest::MDNWarningWidgetJobTest(QObject *parent)
    : QObject{parent}
{
}

void MDNWarningWidgetJobTest::shouldHaveDefaultValues()
{
    MessageComposer::MDNWarningWidgetJob job;
    QVERIFY(!job.item().isValid());
    QVERIFY(!job.canStart());
}

#include "moc_mdnwarningwidgetjobtest.cpp"
