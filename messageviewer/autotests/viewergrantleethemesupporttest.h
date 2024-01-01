/*
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class ViewerGrantleeThemeSupportTest : public QObject
{
    Q_OBJECT
public:
    explicit ViewerGrantleeThemeSupportTest(QObject *parent = nullptr);
    ~ViewerGrantleeThemeSupportTest() override;
private Q_SLOTS:
    void initTestCase();
    void shouldUpdateThemeMenu();
};
