/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "mdnwarningwidgettest.h"
#include "mdn/mdnwarningwidget.h"

#include <QAction>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(MDNWarningWidgetTest)
Q_DECLARE_METATYPE(MessageViewer::MDNWarningWidget::ResponseType)
MDNWarningWidgetTest::MDNWarningWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void MDNWarningWidgetTest::shouldHaveDefaultValues()
{
    qRegisterMetaType<MessageViewer::MDNWarningWidget::ResponseType>();
    MessageViewer::MDNWarningWidget w;
    QVERIFY(!w.isVisible());
    QVERIFY(w.wordWrap());
    QVERIFY(w.text().isEmpty());
    QVERIFY(!w.isCloseButtonVisible());

    QCOMPARE(w.messageType(), KMessageWidget::Information);
    QCOMPARE(w.actions().count(), 3);
    const auto acts{w.actions()};
    for (const auto &name : {QStringLiteral("mIgnoreAction"), QStringLiteral("mSendAction"), QStringLiteral("mSendDenyAction")}) {
        bool found = false;
        for (auto a : acts) {
            if (a->objectName() == name) {
                found = true;
                break;
            }
        }
        QVERIFY(found);
    }
}

void MDNWarningWidgetTest::shouldEmitSignals()
{
    MessageViewer::MDNWarningWidget w;
    const auto acts{w.actions()};
    bool found = false;
    for (auto a : acts) {
        if (a->objectName() == QLatin1StringView("mIgnoreAction")) {
            found = true;
            QSignalSpy ignoreMdn(&w, &MessageViewer::MDNWarningWidget::sendResponse);
            a->trigger();
            QCOMPARE(ignoreMdn.count(), 1);
            QCOMPARE(ignoreMdn.at(0).at(0).value<MessageViewer::MDNWarningWidget::ResponseType>(), MessageViewer::MDNWarningWidget::ResponseType::Ignore);
            break;
        }
    }

    found = false;
    for (auto a : acts) {
        if (a->objectName() == QLatin1StringView("mSendAction")) {
            found = true;
            QSignalSpy sendMdn(&w, &MessageViewer::MDNWarningWidget::sendResponse);
            a->trigger();
            QCOMPARE(sendMdn.count(), 1);
            QCOMPARE(sendMdn.at(0).at(0).value<MessageViewer::MDNWarningWidget::ResponseType>(), MessageViewer::MDNWarningWidget::ResponseType::Send);
            break;
        }
    }

    found = false;
    for (auto a : acts) {
        if (a->objectName() == QLatin1StringView("mSendDenyAction")) {
            found = true;
            QSignalSpy sendDeny(&w, &MessageViewer::MDNWarningWidget::sendResponse);
            a->trigger();
            QCOMPARE(sendDeny.count(), 1);
            QCOMPARE(sendDeny.at(0).at(0).value<MessageViewer::MDNWarningWidget::ResponseType>(), MessageViewer::MDNWarningWidget::ResponseType::SendDeny);
            break;
        }
    }

    QVERIFY(found);
}

void MDNWarningWidgetTest::shouldChangeInformation()
{
    MessageViewer::MDNWarningWidget w;
    const QString str{QStringLiteral("bla-bla")};
    w.setInformation(str);
    QCOMPARE(w.text(), str);
}

#include "moc_mdnwarningwidgettest.cpp"
