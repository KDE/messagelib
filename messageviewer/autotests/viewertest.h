/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef VIEWERTEST_H
#define VIEWERTEST_H
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

#endif // VIEWERTEST_H
