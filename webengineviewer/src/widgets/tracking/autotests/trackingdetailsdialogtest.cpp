/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingdetailsdialogtest.h"
#include "widgets/tracking/trackingdetailsdialog.h"
#include <QStandardPaths>
#include <QTest>

QTEST_MAIN(TrackingDetailsDialogTest)

TrackingDetailsDialogTest::TrackingDetailsDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void TrackingDetailsDialogTest::shouldHaveDefaultValue()
{
    WebEngineViewer::TrackingDetailsDialog d;
    // TODO
}

#include "moc_trackingdetailsdialogtest.cpp"
