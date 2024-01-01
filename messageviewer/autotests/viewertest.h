/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once
#include <QObject>

class ViewerTest : public QObject
{
    Q_OBJECT
public:
    ViewerTest();
private Q_SLOTS:
    void shouldHaveDefaultValuesOnCreation();
    void shouldDisplayMessage();
    void shouldSwitchToAnotherMessage();
};
