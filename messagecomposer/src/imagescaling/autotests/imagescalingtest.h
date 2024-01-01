/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ImageScalingTest : public QObject
{
    Q_OBJECT
public:
    explicit ImageScalingTest(QObject *parent = nullptr);
    ~ImageScalingTest() override;
private Q_SLOTS:
    void initTestCase();
    void shouldHaveDefaultValue();
    void shouldHaveRenameFile_data();
    void shouldHaveRenameFile();

    void shouldHaveChangeMimetype_data();
    void shouldHaveChangeMimetype();
};
