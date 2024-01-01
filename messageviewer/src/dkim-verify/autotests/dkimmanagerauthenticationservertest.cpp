/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerauthenticationservertest.h"
#include "dkim-verify/dkimmanagerauthenticationserver.h"
#include <QStandardPaths>
#include <QTest>
QTEST_GUILESS_MAIN(DKIMManagerAuthenticationServerTest)

DKIMManagerAuthenticationServerTest::DKIMManagerAuthenticationServerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMManagerAuthenticationServerTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMManagerAuthenticationServer w;
    QVERIFY(w.serverList().isEmpty());
}

#include "moc_dkimmanagerauthenticationservertest.cpp"
