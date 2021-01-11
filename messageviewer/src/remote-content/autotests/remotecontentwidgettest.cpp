/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidgettest.h"
#include "remote-content/remotecontentwidget.h"
#include <QTest>
QTEST_MAIN(RemoteContentWidgetTest)
RemoteContentWidgetTest::RemoteContentWidgetTest(QObject *parent)
    : QObject(parent)
{

}

void RemoteContentWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentWidget w;
    //TODO
}
