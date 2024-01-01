/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplatesInsertCommandPushButtonTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandPushButtonTest(QObject *parent = nullptr);
    ~TemplatesInsertCommandPushButtonTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
