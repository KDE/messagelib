/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingwarningwidgettest.h"
#include "widgets/trackingwarningwidget.h"
#include <QTest>

QTEST_MAIN(TrackingWarningWidgetTest)

TrackingWarningWidgetTest::TrackingWarningWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void TrackingWarningWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::TrackingWarningWidget w;
    QVERIFY(!w.isVisible());
    QVERIFY(!w.isCloseButtonVisible());
    QVERIFY(w.wordWrap());
    QVERIFY(!w.text().isEmpty());
}
