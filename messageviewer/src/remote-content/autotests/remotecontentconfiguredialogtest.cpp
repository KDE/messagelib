/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialogtest.h"
#include "remote-content/remotecontentconfiguredialog.h"
#include <QTest>
QTEST_MAIN(RemoteContentConfigureDialogTest)
RemoteContentConfigureDialogTest::RemoteContentConfigureDialogTest(QObject *parent)
    : QObject(parent)
{

}

void RemoteContentConfigureDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentConfigureDialog d;
    //TODO
}
