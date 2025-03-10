/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplateParserEmailAddressRequesterLineEditTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserEmailAddressRequesterLineEditTest(QObject *parent = nullptr);
    ~TemplateParserEmailAddressRequesterLineEditTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAssignValue();
    void shouldClearValue();
    void shouldEmitSignal();
};
