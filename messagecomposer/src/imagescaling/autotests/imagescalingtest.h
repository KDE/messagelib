/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMAGESCALINGTEST_H
#define IMAGESCALINGTEST_H

#include <QObject>

class ImageScalingTest : public QObject
{
    Q_OBJECT
public:
    explicit ImageScalingTest(QObject *parent = nullptr);
    ~ImageScalingTest();
private Q_SLOTS:
    void initTestCase();
    void shouldHaveDefaultValue();
    void shouldHaveRenameFile_data();
    void shouldHaveRenameFile();

    void shouldHaveChangeMimetype_data();
    void shouldHaveChangeMimetype();
};

#endif // IMAGESCALINGTEST_H
