/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMKeyRecordTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMKeyRecordTest(QObject *parent = nullptr);
    ~DKIMKeyRecordTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestExtractDkimKeyRecord();
    void shouldTestExtractDkimKeyRecord_data();
};
