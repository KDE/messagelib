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
    QVERIFY(!w.alwaysRuleForHost(QUrl(QStringLiteral("http://www.kde.org"))));
    QVERIFY(w.hosts().isEmpty());
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

void OpenUrlWithManagerTest::shouldSearchOpenWithInfoEnabled()
{
    QFETCH(QList<MessageViewer::OpenWithUrlInfo>, infos);
    QFETCH(QUrl, url);
    QFETCH(bool, openWithInfoIsValid);
    MessageViewer::OpenUrlWithManager w;
    w.setOpenWithUrlInfo(infos);
    QCOMPARE(w.openWith(url).isValid(), openWithInfoIsValid);
}

void OpenUrlWithManagerTest::shouldSearchOpenWithInfoEnabled_data()
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
    {
        QList<MessageViewer::OpenWithUrlInfo> lst;
        MessageViewer::OpenWithUrlInfo i;
        i.setUrl(QStringLiteral("http://www.kde.org"));
        i.setCommand(QStringLiteral("bla"));
        i.setEnabled(false);
        lst.append(i);

        QTest::newRow("empty-2") << lst << QUrl() << false;

        QTest::newRow("valid-2") << lst << QUrl(QStringLiteral("http://www.kde.org")) << false;
        QTest::newRow("invalid-2") << lst << QUrl(QStringLiteral("http://www.bla.org")) << false;
    }
}

void OpenUrlWithManagerTest::shouldTestAlreadyRuleForHost()
{
    MessageViewer::OpenUrlWithManager w;
    QList<MessageViewer::OpenWithUrlInfo> infos;
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(QStringLiteral("http://www.kde.org"));
        info.setCommand(QStringLiteral("bla"));
        QVERIFY(info.isValid());
        infos.append(info);
    }
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(QStringLiteral("http://www.kde1.org"));
        info.setCommand(QStringLiteral("bla"));
        QVERIFY(info.isValid());
        infos.append(info);
    }
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(QStringLiteral("http://www.bla.org"));
        info.setCommand(QStringLiteral("bla"));
        QVERIFY(info.isValid());
        infos.append(info);
    }
    w.setOpenWithUrlInfo(infos);
    QVERIFY(!w.alwaysRuleForHost(QUrl(QStringLiteral("http://www.jjo.org"))));
    QVERIFY(w.alwaysRuleForHost(QUrl(QStringLiteral("http://www.kde1.org"))));
    QVERIFY(w.alwaysRuleForHost(QUrl(QStringLiteral("http://www.kde.org"))));
}

#include "moc_openurlwithmanagertest.cpp"
