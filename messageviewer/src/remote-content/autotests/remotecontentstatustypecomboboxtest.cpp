/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentstatustypecomboboxtest.h"
#include "remote-content/remotecontentstatustypecombobox.h"
#include <QTest>
QTEST_MAIN(RemoteContentStatusTypeComboBoxTest)
RemoteContentStatusTypeComboBoxTest::RemoteContentStatusTypeComboBoxTest(QObject *parent)
    : QObject(parent)
{
}

void RemoteContentStatusTypeComboBoxTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentStatusTypeComboBox w;
    QCOMPARE(w.count(), 2);
}
