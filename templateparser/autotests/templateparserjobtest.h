/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TemplateParserJobTest : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserJobTest(QObject *parent = nullptr);
    ~TemplateParserJobTest() override;
private Q_SLOTS:
    /**
     * checks whether text/plain only mails are converted to a valid HTML
     */

    void test_convertedHtml();
    void test_convertedHtml_data();

    void test_replyHtml();
    void test_replyHtml_data();

    void test_replyPlain();
    void test_replyPlain_data();

    void test_forwardPlain();
    void test_forwardPlain_data();

    void test_forwardHtml();
    void test_forwardHtml_data();

    void test_forwardedAttachments();
    void test_forwardedAttachments_data();

    /**
     * Tests whether templates are returning required body or not
     */
    void test_processWithTemplatesForBody();
    void test_processWithTemplatesForBody_data();

    void test_processWithTemplatesForContent();
    void test_processWithTemplatesForContent_data();

    void test_processWithTemplatesForContentOtherTimeZone();
    void test_processWithTemplatesForContentOtherTimeZone_data();

    void test_makeValidHtml_data();
    void test_makeValidHtml();
};
