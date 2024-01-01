/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SendLaterInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit SendLaterInfoTest(QObject *parent = nullptr);

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldNotValidIfIdIsNotValid();
    void shouldNotValidIfDateIsNotValid();
    void shouldCopyInfo();
};
