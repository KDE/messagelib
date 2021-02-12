/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERKEYTEST_H
#define DKIMMANAGERKEYTEST_H

#include <QObject>

class DKIMManagerKeyTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyTest(QObject *parent = nullptr);
    ~DKIMManagerKeyTest() override = default;
};

#endif // DKIMMANAGERKEYTEST_H
