/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplateExtractTextFromMailTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateExtractTextFromMailTest(QObject *parent = nullptr);
    ~TemplateExtractTextFromMailTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldExtractHtml();
};
