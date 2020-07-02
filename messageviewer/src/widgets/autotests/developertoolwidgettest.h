/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEVELOPERTOOLWIDGETTEST_H
#define DEVELOPERTOOLWIDGETTEST_H

#include <QObject>

class DeveloperToolWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit DeveloperToolWidgetTest(QObject *parent = nullptr);
    ~DeveloperToolWidgetTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DEVELOPERTOOLWIDGETTEST_H
