/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LOCALDATABASEMANAGERTEST_H
#define LOCALDATABASEMANAGERTEST_H

#include <QObject>

class LocalDataBaseManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataBaseManagerTest(QObject *parent = nullptr);
    ~LocalDataBaseManagerTest();
};

#endif // LOCALDATABASEMANAGERTEST_H
