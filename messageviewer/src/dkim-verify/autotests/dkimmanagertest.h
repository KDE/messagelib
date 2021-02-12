/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERTEST_H
#define DKIMMANAGERTEST_H

#include <QObject>

class DKIMManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerTest(QObject *parent = nullptr);
    ~DKIMManagerTest() override = default;
};

#endif // DKIMMANAGERTEST_H
