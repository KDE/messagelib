/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailtrackingwarningwidgettest.h"
#include "widgets/mailtrackingwarningwidget.h"
#include <QTest>

QTEST_MAIN(MailTrackingWarningWidgetTest)

MailTrackingWarningWidgetTest::MailTrackingWarningWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void MailTrackingWarningWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::MailTrackingWarningWidget w;
    QVERIFY(!w.isVisible());
    QVERIFY(!w.isCloseButtonVisible());
    QVERIFY(w.wordWrap());
    QVERIFY(!w.text().isEmpty());
}
