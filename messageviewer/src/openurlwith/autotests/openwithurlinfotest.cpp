/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openwithurlinfotest.h"
#include "openurlwith/openwithurlinfo.h"
#include <QTest>
QTEST_MAIN(OpenWithUrlInfoTest)
OpenWithUrlInfoTest::OpenWithUrlInfoTest(QObject *parent)
    : QObject{parent}
{
}

void OpenWithUrlInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::OpenWithUrlInfo info;
    QVERIFY(!info.isValid());
    QVERIFY(info.url().isEmpty());
    QVERIFY(info.command().isEmpty());
    QVERIFY(info.commandLine().isEmpty());
    QVERIFY(info.isLocalOpenWithInfo());
    QVERIFY(info.enabled());
}

#include "moc_openwithurlinfotest.cpp"
