/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "dkimmanagerkeytest.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMManagerKeyTest)

DKIMManagerKeyTest::DKIMManagerKeyTest(QObject *parent)
    : QObject(parent)
{
}

#include "moc_dkimmanagerkeytest.cpp"
