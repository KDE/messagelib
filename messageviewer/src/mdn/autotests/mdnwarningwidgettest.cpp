/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "mdnwarningwidgettest.h"
#include "mdn/mdnwarningwidget.h"

#include <QAction>
#include <QSignalSpy>
#include <QTest>

QTEST_MAIN(MDNWarningWidgetTest)

MDNWarningWidgetTest::MDNWarningWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void MDNWarningWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::MDNWarningWidget w;
    QVERIFY(!w.isVisible());
    QVERIFY(w.wordWrap());

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
        if (a->objectName() == QStringLiteral("mIgnoreAction")) {
            found = true;
            QSignalSpy ignoreMdn(&w, &MessageViewer::MDNWarningWidget::ignoreMdn);
            a->trigger();
            QCOMPARE(ignoreMdn.count(), 1);
            break;
        }
    }

    found = false;
    for (auto a : acts) {
        if (a->objectName() == QStringLiteral("mSendAction")) {
            found = true;
            QSignalSpy sendMdn(&w, &MessageViewer::MDNWarningWidget::sendMdn);
            a->trigger();
            QCOMPARE(sendMdn.count(), 1);
            break;
        }
    }

    found = false;
    for (auto a : acts) {
        if (a->objectName() == QStringLiteral("mSendDenyAction")) {
            found = true;
            QSignalSpy sendDeny(&w, &MessageViewer::MDNWarningWidget::sendDeny);
            a->trigger();
            QCOMPARE(sendDeny.count(), 1);
            break;
        }
    }

    QVERIFY(found);
}
