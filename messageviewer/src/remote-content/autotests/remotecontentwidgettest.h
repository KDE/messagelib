/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTWIDGETTEST_H
#define REMOTECONTENTWIDGETTEST_H

#include <QObject>

class RemoteContentWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentWidgetTest(QObject *parent = nullptr);
    ~RemoteContentWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // REMOTECONTENTWIDGETTEST_H
