/*
  SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef VIEWERGRANTLEETHEMESUPPORTTEST_H
#define VIEWERGRANTLEETHEMESUPPORTTEST_H

#include <QObject>

class ViewerGrantleeThemeSupportTest : public QObject
{
    Q_OBJECT
public:
    explicit ViewerGrantleeThemeSupportTest(QObject *parent = nullptr);
    ~ViewerGrantleeThemeSupportTest();
private Q_SLOTS:
    void initTestCase();
    void shouldUpdateThemeMenu();
};

#endif // VIEWERGRANTLEETHEMESUPPORTTEST_H
