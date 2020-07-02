/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHECKPHISHINGURLJOBTEST_H
#define CHECKPHISHINGURLJOBTEST_H

#include <QObject>

class CheckPhishingUrlJobTest : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlJobTest(QObject *parent = nullptr);
    ~CheckPhishingUrlJobTest();
private Q_SLOTS:
    void shouldNotBeAbleToStartWithEmptyUrl();
    void shouldCreateRequest_data();
    void shouldCreateRequest();

    void shouldParseResult_data();
    void shouldParseResult();
};

#endif // CHECKPHISHINGURLJOBTEST_H
