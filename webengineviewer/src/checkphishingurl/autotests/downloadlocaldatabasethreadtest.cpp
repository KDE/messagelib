/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "downloadlocaldatabasethreadtest.h"
#include "../downloadlocaldatabasethread.h"

#include <QTest>

DownloadLocalDatabaseThreadTest::DownloadLocalDatabaseThreadTest(QObject *parent)
    : QObject(parent)
{
}

DownloadLocalDatabaseThreadTest::~DownloadLocalDatabaseThreadTest() = default;

QTEST_GUILESS_MAIN(DownloadLocalDatabaseThreadTest)

#include "moc_downloadlocaldatabasethreadtest.cpp"
