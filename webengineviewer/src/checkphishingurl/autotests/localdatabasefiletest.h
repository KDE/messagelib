/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class LocalDataBaseFileTest : public QObject
{
    Q_OBJECT
public:
    explicit LocalDataBaseFileTest(QObject *parent = nullptr);
    ~LocalDataBaseFileTest();
private Q_SLOTS:
    void shouldBeInvalidWithUnExistingFile();

    void shouldCheckHashBinaryFile_data();
    void shouldCheckHashBinaryFile();
};

