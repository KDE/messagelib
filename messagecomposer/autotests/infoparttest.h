/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef INFOPARTTEST_H
#define INFOPARTTEST_H

#include <QObject>

class InfoPartTest : public QObject
{
    Q_OBJECT
public:
    explicit InfoPartTest(QObject *parent = nullptr);
    ~InfoPartTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // INFOPARTTEST_H
