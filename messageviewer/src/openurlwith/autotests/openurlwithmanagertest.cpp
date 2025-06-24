/*
   SPDX-FileCopyrightText: 2022-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanagertest.h"
using namespace Qt::Literals::StringLiterals;

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
    // QVERIFY(w.openWithUrlInfo().isEmpty());
    QVERIFY(!w.alwaysRuleForHost(QUrl(u"http://www.kde.org"_s)));
    // QVERIFY(w.hosts().isEmpty());
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
        i.setUrl(u"http://www.kde.org"_s);
        i.setCommand(u"bla"_s);
        lst.append(i);

        QTest::newRow("empty-1") << lst << QUrl() << false;

        QTest::newRow("valid-1") << lst << QUrl(u"http://www.kde.org"_s) << true;
        QTest::newRow("invalid-1") << lst << QUrl(u"http://www.bla.org"_s) << false;
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
        i.setUrl(u"http://www.kde.org"_s);
        i.setCommand(u"bla"_s);
        lst.append(i);

        QTest::newRow("empty-1") << lst << QUrl() << false;

        QTest::newRow("valid-1") << lst << QUrl(u"http://www.kde.org"_s) << true;
        QTest::newRow("invalid-1") << lst << QUrl(u"http://www.bla.org"_s) << false;
    }
    {
        QList<MessageViewer::OpenWithUrlInfo> lst;
        MessageViewer::OpenWithUrlInfo i;
        i.setUrl(u"http://www.kde.org"_s);
        i.setCommand(u"bla"_s);
        i.setEnabled(false);
        lst.append(i);

        QTest::newRow("empty-2") << lst << QUrl() << false;

        QTest::newRow("valid-2") << lst << QUrl(u"http://www.kde.org"_s) << false;
        QTest::newRow("invalid-2") << lst << QUrl(u"http://www.bla.org"_s) << false;
    }
}

void OpenUrlWithManagerTest::shouldTestAlreadyRuleForHost()
{
    MessageViewer::OpenUrlWithManager w;
    QList<MessageViewer::OpenWithUrlInfo> infos;
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(u"http://www.kde.org"_s);
        info.setCommand(u"bla"_s);
        QVERIFY(info.isValid());
        infos.append(info);
    }
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(u"http://www.kde1.org"_s);
        info.setCommand(u"bla"_s);
        QVERIFY(info.isValid());
        infos.append(info);
    }
    {
        MessageViewer::OpenWithUrlInfo info;
        info.setUrl(u"http://www.bla.org"_s);
        info.setCommand(u"bla"_s);
        QVERIFY(info.isValid());
        infos.append(info);
    }
    w.setOpenWithUrlInfo(infos);
    QVERIFY(!w.alwaysRuleForHost(QUrl(u"http://www.jjo.org"_s)));
    QVERIFY(w.alwaysRuleForHost(QUrl(u"http://www.kde1.org"_s)));
    QVERIFY(w.alwaysRuleForHost(QUrl(u"http://www.kde.org"_s)));
}

#include "moc_openurlwithmanagertest.cpp"
