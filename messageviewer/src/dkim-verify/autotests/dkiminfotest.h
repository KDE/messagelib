/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMInfoTest(QObject *parent = nullptr);
    ~DKIMInfoTest() override = default;

private Q_SLOTS:
    void shouldTestExtractDkimInfo();
    void shouldTestExtractDkimInfo_data();
    void shouldHaveDefaultValue();
};
