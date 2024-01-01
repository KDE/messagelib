/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagertest.h"
#include "dkim-verify/dkimmanager.h"
#include <QStandardPaths>
#include <QTest>
QTEST_MAIN(DKIMManagerTest)

DKIMManagerTest::DKIMManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

#include "moc_dkimmanagertest.cpp"
