/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILWEBENGINEACCESSKEYANCHORTEST_H
#define MAILWEBENGINEACCESSKEYANCHORTEST_H

#include <QObject>

class WebEngineAccessKeyAnchorTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineAccessKeyAnchorTest(QObject *parent = nullptr);
    ~WebEngineAccessKeyAnchorTest();
private Q_SLOTS:
    void shouldReturnEmptyAccessKeyAnchor();
};

#endif // MAILWEBENGINEACCESSKEYANCHORTEST_H
