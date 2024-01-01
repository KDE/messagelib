/*
  SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplateParserUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserUtilTest(QObject *parent = nullptr);
    ~TemplateParserUtilTest() override = default;

private Q_SLOTS:
    void shouldRemoveSpaceAtBegin_data();
    void shouldRemoveSpaceAtBegin();
    void shouldHasNotEmptyKeywordsWithArgs();
    void shouldHasNotEmptyKeywords();
    void shouldGetFirstNameFromEmail_data();
    void shouldGetFirstNameFromEmail();
    void shouldGetLastNameFromEmail();
    void shouldGetLastNameFromEmail_data();
};
