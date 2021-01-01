/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DOWNLOADLOCALDATABASETHREADTEST_H
#define DOWNLOADLOCALDATABASETHREADTEST_H

#include <QObject>

class DownloadLocalDatabaseThreadTest : public QObject
{
    Q_OBJECT
public:
    explicit DownloadLocalDatabaseThreadTest(QObject *parent = nullptr);
    ~DownloadLocalDatabaseThreadTest();
};

#endif // DOWNLOADLOCALDATABASETHREADTEST_H
