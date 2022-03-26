/*
  SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class BodyPartUrlHandlerManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit BodyPartUrlHandlerManagerTest(QObject *parent = nullptr);

private Q_SLOTS:
    void testHandleClick_data();
    void testHandleClick();
};
