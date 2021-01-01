/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TEXTPARTTEST_H
#define TEXTPARTTEST_H
#include <QObject>

class TextPartTest : public QObject
{
    Q_OBJECT
public:
    explicit TextPartTest(QObject *parent = nullptr);
    ~TextPartTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // TEXTPARTTEST_H
