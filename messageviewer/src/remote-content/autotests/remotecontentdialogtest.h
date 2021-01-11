/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTDIALOGTEST_H
#define REMOTECONTENTDIALOGTEST_H

#include <QObject>

class RemoteContentDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentDialogTest(QObject *parent = nullptr);
    ~RemoteContentDialogTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // REMOTECONTENTDIALOGTEST_H
