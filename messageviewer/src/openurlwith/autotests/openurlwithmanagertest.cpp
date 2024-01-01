/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

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
    QFETCH(QList<MessageViewer::OpenWithUrlInfo>, infos);
    QFETCH(QUrl, url);
    QFETCH(bool, openWithInfoIsValid);
    MessageViewer::OpenUrlWithManager w;
    w.setOpenWithUrlInfo(infos);
    QCOMPARE(w.openWith(url).isValid(), openWithInfoIsValid);
}

void OpenUrlWithManagerTest::shouldSearchOpenWithInfo_data()
{
    QTest::addColumn<QList<MessageViewer::OpenWithUrlInfo>>("infos");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("openWithInfoIsValid");
    {
        QList<MessageViewer::OpenWithUrlInfo> lst;
        QTest::newRow("empty") << lst << QUrl() << false;
    }
    {
        QList<MessageViewer::OpenWithUrlInfo> lst;
        MessageViewer::OpenWithUrlInfo i;
        i.setUrl(QStringLiteral("http://www.kde.org"));
        i.setCommand(QStringLiteral("bla"));
        lst.append(i);

        QTest::newRow("empty-1") << lst << QUrl() << false;

        QTest::newRow("valid-1") << lst << QUrl(QStringLiteral("http://www.kde.org")) << true;
        QTest::newRow("invalid-1") << lst << QUrl(QStringLiteral("http://www.bla.org")) << false;
    }
}

#include "moc_openurlwithmanagertest.cpp"
