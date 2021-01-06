/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTMANAGERTEST_H
#define REMOTECONTENTMANAGERTEST_H

#include <QObject>

class RemoteContentManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentManagerTest(QObject *parent = nullptr);
    ~RemoteContentManagerTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // REMOTECONTENTMANAGERTEST_H
