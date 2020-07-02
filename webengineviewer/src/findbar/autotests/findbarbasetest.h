/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FINDBARBASETEST_H
#define FINDBARBASETEST_H

#include <QObject>

class FindBarBaseTest : public QObject
{
    Q_OBJECT
public:
    explicit FindBarBaseTest(QObject *parent = nullptr);
    ~FindBarBaseTest();

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldClearLineWhenClose();
    void shouldEnableDisableNextPreviousButton();
    void shouldClearAllWhenShowBar();
};

#endif // FINDBARBASETEST_H
