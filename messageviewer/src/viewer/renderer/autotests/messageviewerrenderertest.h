/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class MessageViewerRendererTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerRendererTest(QObject *parent = nullptr);
    ~MessageViewerRendererTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
