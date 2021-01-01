/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREDIALOGTEST_H
#define REMOTECONTENTCONFIGUREDIALOGTEST_H

#include <QObject>

class RemoteContentConfigureDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureDialogTest(QObject *parent = nullptr);
    ~RemoteContentConfigureDialogTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // REMOTECONTENTCONFIGUREDIALOGTEST_H
