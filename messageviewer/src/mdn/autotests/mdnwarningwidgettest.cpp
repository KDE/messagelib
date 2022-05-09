/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "mdnwarningwidgettest.h"
#include "mdn/mdnwarningwidget.h"

#include <QAction>
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
    QCOMPARE(w.actions().count(), 2);
    const auto acts{w.actions()};
    for (const auto &name : {QStringLiteral("mIgnoreAction"), QStringLiteral("mSendAction")}) {
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
