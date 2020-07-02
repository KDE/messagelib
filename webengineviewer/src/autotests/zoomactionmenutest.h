/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ZOOMACTIONMENUTEST_H
#define ZOOMACTIONMENUTEST_H

#include <QObject>

class ZoomActionMenuTest : public QObject
{
    Q_OBJECT
public:
    explicit ZoomActionMenuTest(QObject *parent = nullptr);
    ~ZoomActionMenuTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAssignZoomFactor();
};

#endif // ZOOMACTIONMENUTEST_H
