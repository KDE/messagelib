/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CreatePhishingUrlDataBaseJobTest : public QObject
{
    Q_OBJECT
public:
    explicit CreatePhishingUrlDataBaseJobTest(QObject *parent = nullptr);
    ~CreatePhishingUrlDataBaseJobTest() override;
private Q_SLOTS:
    void shouldCreateRequest_data();
    void shouldCreateRequest();

    void checkAdditionElements();
    void checkAdditionElements_data();

    void checkRemovalElements();
    void checkRemovalElements_data();

    void shouldParseResult_data();
    void shouldParseResult();
    void initTestCase();

    void shouldClearUpdateDataBaseInfo();

    void checkRiceDeltaEncoding_data();
    void checkRiceDeltaEncoding();
};
