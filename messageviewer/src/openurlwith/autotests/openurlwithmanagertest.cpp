/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanagertest.h"
#include "openurlwith/openurlwithmanager.h"
#include <QStandardPaths>
#include <QTest>
QTEST_MAIN(OpenUrlWithManagerTest)

OpenUrlWithManagerTest::OpenUrlWithManagerTest(QObject *parent)
    : QObject{parent}
{
    QStandardPaths::setTestModeEnabled(true);
}

void OpenUrlWithManagerTest::shouldHaveDefaultValues()
{
    MessageViewer::OpenUrlWithManager w;
    QVERIFY(w.openWithUrlInfo().isEmpty());
}

void OpenUrlWithManagerTest::shouldSearchOpenWithInfo()
{
    QFETCH(QUrl, url);
    QFETCH(bool, openWithInfoIsValid);
    MessageViewer::OpenUrlWithManager w;
    QCOMPARE(w.openWith(url).isValid(), openWithInfoIsValid);
}

void OpenUrlWithManagerTest::shouldSearchOpenWithInfo_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("openWithInfoIsValid");

    QTest::newRow("empty") << QUrl() << false;
}
