/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CreateDatabaseFileJobTest : public QObject
{
    Q_OBJECT
public:
    explicit CreateDatabaseFileJobTest(QObject *parent = nullptr);
    ~CreateDatabaseFileJobTest() override;
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
