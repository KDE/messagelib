/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LOCALDATABASEFILETEST_H
#define LOCALDATABASEFILETEST_H

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

#endif // LOCALDATABASEFILETEST_H
