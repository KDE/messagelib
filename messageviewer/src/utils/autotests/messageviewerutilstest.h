/*
  SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef MESSAGEVIEWERUTILSTEST_H
#define MESSAGEVIEWERUTILSTEST_H

#include <QObject>

class MessageViewerUtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerUtilsTest(QObject *parent = nullptr);
    ~MessageViewerUtilsTest() = default;
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

    void shouldUseCorrectCodec();
    void shouldUseCorrectCodec_data();
};

#endif // MESSAGEVIEWERUTILSTEST_H
