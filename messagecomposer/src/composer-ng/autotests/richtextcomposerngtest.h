/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class RichTextComposerNgTest : public QObject
{
    Q_OBJECT
public:
    explicit RichTextComposerNgTest(QObject *parent = nullptr);
    ~RichTextComposerNgTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldForceAutoCorrection_data();
    void shouldForceAutoCorrection();

    void shouldForceAutoCorrectionWithSelection_data();
    void shouldForceAutoCorrectionWithSelection();

    void shouldNotChangeSignatureWhenOriginalAndNewSignatureAreSame();

    void shouldAddSignature_data();
    void shouldAddSignature();

    void shouldAddSpecificSignature_data();
    void shouldAddSpecificSignature();

    void shouldReplaceSignature_data();
    void shouldReplaceSignature();

    void shouldLoadSignatureFromFile_data();
    void shouldLoadSignatureFromFile();

    void shouldLoadSignatureFromCommand_data();
    void shouldLoadSignatureFromCommand();

    void fixHtmlFontSizeTest();

    void toCleanHtmlRegexTest();
};
