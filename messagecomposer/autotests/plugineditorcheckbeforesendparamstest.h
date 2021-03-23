/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class PluginEditorCheckBeforeSendParamsTest : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendParamsTest(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSendParamsTest();

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldBeEqual();
    void shouldAssignValue();
};

