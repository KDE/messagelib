/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CREATEDATABASEFILEJOBTEST_H
#define CREATEDATABASEFILEJOBTEST_H

#include <QObject>

class CreateDatabaseFileJobTest : public QObject
{
    Q_OBJECT
public:
    explicit CreateDatabaseFileJobTest(QObject *parent = nullptr);
    ~CreateDatabaseFileJobTest();
private Q_SLOTS:
    void initTestCase();
    void shouldHaveDefaultValue();

    void shouldCreateFile();
    void shouldCreateFile_data();

    void shouldCreateCorrectBinaryFile();

    void shouldUpdateDataBase();

    void shouldRemoveElementInDataBase();
    void shouldRemoveElementInDataBase_data();
};

#endif // CREATEDATABASEFILEJOBTEST_H
