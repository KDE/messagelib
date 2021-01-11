/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentdialogtest.h"
#include "remote-content/remotecontentdialog.h"
#include <QTest>
QTEST_MAIN(RemoteContentDialogTest)

RemoteContentDialogTest::RemoteContentDialogTest(QObject *parent)
    : QObject(parent)
{

}

void RemoteContentDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentDialog d;
    //TODO
}
