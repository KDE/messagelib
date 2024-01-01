/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplateParserExtractHtmlInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserExtractHtmlInfoTest(QObject *parent = nullptr);
    ~TemplateParserExtractHtmlInfoTest() override = default;
private Q_SLOTS:
    void initTestCase();
    void shouldReturnNullResult();
    void shouldReturnNullButWithTemplate();
};
