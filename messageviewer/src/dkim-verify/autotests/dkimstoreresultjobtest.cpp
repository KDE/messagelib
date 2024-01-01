/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimstoreresultjobtest.h"
#include "dkim-verify/dkimstoreresultjob.h"
#include <QTest>
QTEST_GUILESS_MAIN(DKIMStoreResultJobTest)
DKIMStoreResultJobTest::DKIMStoreResultJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMStoreResultJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMStoreResultJob w;
    QVERIFY(!w.canStart());
}

#include "moc_dkimstoreresultjobtest.cpp"
