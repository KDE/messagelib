/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingwarningwidgettest.h"
#include "widgets/tracking/trackingwarningwidget.h"
#include <QTest>

QTEST_MAIN(TrackingWarningWidgetTest)

TrackingWarningWidgetTest::TrackingWarningWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void TrackingWarningWidgetTest::shouldHaveDefaultValues()
{
    WebEngineViewer::TrackingWarningWidget w;
    QVERIFY(!w.isVisible());
    QVERIFY(!w.isCloseButtonVisible());
    QVERIFY(w.wordWrap());
    QVERIFY(!w.text().isEmpty());
}

#include "moc_trackingwarningwidgettest.cpp"
