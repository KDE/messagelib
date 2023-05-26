/*
   SPDX-FileCopyrightText: 2015-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <KIdentityManagementCore/Signature>
#include <KPIMTextEdit/RichTextComposer>
#include <MessageComposer/PluginEditorConvertTextInterface>

class RichTextComposerNgTest;
namespace TextAutoCorrectionCore
{
class AutoCorrection;
}
namespace MessageComposer
{
class TextPart;
class RichTextComposerSignatures;
class RichTextComposerNgPrivate;
/**
 * @brief The RichTextComposerNg class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT RichTextComposerNg : public KPIMTextEdit::RichTextComposer
{
    Q_OBJECT
public:
    explicit RichTextComposerNg(QWidget *parent = nullptr);
    ~RichTextComposerNg() override;
    Q_REQUIRED_RESULT TextAutoCorrectionCore::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrectionCore::AutoCorrection *autocorrect);

    void setAutocorrectionLanguage(const QString &lang);

    void fillComposerTextPart(MessageComposer::TextPart *textPart);
    Q_REQUIRED_RESULT MessageComposer::RichTextComposerSignatures *composerSignature() const;

    void insertSignature(const KIdentityManagementCore::Signature &signature,
                         KIdentityManagementCore::Signature::Placement placement,
                         KIdentityManagementCore::Signature::AddedText addedText);
    Q_REQUIRED_RESULT QString toCleanHtml() const;

    void forceAutoCorrection(bool selectedText = false) override;

    Q_REQUIRED_RESULT virtual MessageComposer::PluginEditorConvertTextInterface::ConvertTextStatus convertPlainText(MessageComposer::TextPart *textPart);

protected:
    bool processModifyText(QKeyEvent *event) override;

private:
    std::unique_ptr<RichTextComposerNgPrivate> const d;

    friend class ::RichTextComposerNgTest; // for fixHtmlFontSize
    void fixHtmlFontSize(QString &cleanHtml) const;
};
}
