/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class PluginEditorTest : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorTest(QObject *parent = nullptr);
    ~PluginEditorTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
