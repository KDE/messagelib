/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BACKOFFMODEMANAGERTEST_H
#define BACKOFFMODEMANAGERTEST_H

#include <QObject>

class BackOffModeManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit BackOffModeManagerTest(QObject *parent = nullptr);
    ~BackOffModeManagerTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldStartBackOffMode();

    void shouldIncreaseBackOff();
};

#endif // BACKOFFMODEMANAGERTEST_H
