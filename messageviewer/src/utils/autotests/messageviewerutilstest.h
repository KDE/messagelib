/*
  SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class MessageViewerUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerUtilsTest(QObject *parent = nullptr);
    ~MessageViewerUtilsTest() override = default;
private Q_SLOTS:
    void shouldExcludeHeader_data();
    void shouldExcludeHeader();

    void shouldContainsExternalReferences_data();
    void shouldContainsExternalReferences();

    void shouldExtractHtmlBenchmark();
    void shouldExtractHtmlBenchmark_data();

    void shouldExtractHtml();
    void shouldExtractHtml_data();

    void shouldExtractBodyStyle();
    void shouldExtractBodyStyle_data();
};
