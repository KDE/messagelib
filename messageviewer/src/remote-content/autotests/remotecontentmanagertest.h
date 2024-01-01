/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "remote-content/remotecontentmanager.h"

class RemoteContentManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentManagerTest(QObject *parent = nullptr);
    ~RemoteContentManagerTest() override;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldIsBlocked_data();
    void shouldIsBlocked();

private:
    MessageViewer::RemoteContentManager mRemoveContentManager;
};
