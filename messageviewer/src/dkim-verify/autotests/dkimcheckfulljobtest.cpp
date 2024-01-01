/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckfulljobtest.h"
#include "dkim-verify/dkimcheckfulljob.h"

#include <QTest>
QTEST_MAIN(DKIMCheckFullJobTest)
DKIMCheckFullJobTest::DKIMCheckFullJobTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMCheckFullJobTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMCheckFullJob job;
    // TODO
}

#include "moc_dkimcheckfulljobtest.cpp"
