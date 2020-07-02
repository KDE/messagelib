/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DEVELOPERTOOLDIALOGTEST_H
#define DEVELOPERTOOLDIALOGTEST_H

#include <QObject>

class DeveloperToolDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit DeveloperToolDialogTest(QObject *parent = nullptr);
    ~DeveloperToolDialogTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DEVELOPERTOOLDIALOGTEST_H
