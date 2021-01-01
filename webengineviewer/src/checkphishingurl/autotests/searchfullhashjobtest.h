/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SEARCHFULLHASHJOBTEST_H
#define SEARCHFULLHASHJOBTEST_H

#include <QObject>

class SearchFullHashJobTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchFullHashJobTest(QObject *parent = nullptr);
    ~SearchFullHashJobTest();

private Q_SLOTS:
    void shouldNotBeAbleToStartWithEmptyUrl();
    void shouldCreateRequest_data();
    void shouldCreateRequest();
};

#endif // SEARCHFULLHASHJOBTEST_H
