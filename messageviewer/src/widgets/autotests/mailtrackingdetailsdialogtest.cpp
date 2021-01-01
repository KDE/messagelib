/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailtrackingdetailsdialogtest.h"
#include "widgets/mailtrackingdetailsdialog.h"
#include <QTest>
#include <QStandardPaths>

QTEST_MAIN(MailTrackingDetailsDialogTest)

MailTrackingDetailsDialogTest::MailTrackingDetailsDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void MailTrackingDetailsDialogTest::shouldHaveDefaultValue()
{
    //TOOD
}
