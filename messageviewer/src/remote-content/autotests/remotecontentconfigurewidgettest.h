/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREWIDGETTEST_H
#define REMOTECONTENTCONFIGUREWIDGETTEST_H

#include <QObject>

class RemoteContentConfigureWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureWidgetTest(QObject *parent = nullptr);
    ~RemoteContentConfigureWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // REMOTECONTENTCONFIGUREWIDGETTEST_H
