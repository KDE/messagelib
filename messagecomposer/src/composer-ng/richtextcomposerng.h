/*
   SPDX-FileCopyrightText: 2015-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "config-messagecomposer.h"
#include "messagecomposer_export.h"
#include <KIdentityManagement/Signature>
#include <KPIMTextEdit/RichTextComposer>
#include <MessageComposer/PluginEditorConvertTextInterface>

class RichTextComposerNgTest;
#if HAVE_TEXT_AUTOCORRECTION_WIDGETS
namespace TextAutoCorrectionCore
{
class AutoCorrection;
}
#else
namespace TextAutoCorrection
{
class AutoCorrection;
}
#endif
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
#if HAVE_TEXT_AUTOCORRECTION_WIDGETS
    Q_REQUIRED_RESULT TextAutoCorrectionCore::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrectionCore::AutoCorrection *autocorrect);
#else
    Q_REQUIRED_RESULT TextAutoCorrection::AutoCorrection *autocorrection() const;
    void setAutocorrection(TextAutoCorrection::AutoCorrection *autocorrect);
#endif

    void setAutocorrectionLanguage(const QString &lang);

    void fillComposerTextPart(MessageComposer::TextPart *textPart);
    Q_REQUIRED_RESULT MessageComposer::RichTextComposerSignatures *composerSignature() const;

    void insertSignature(const KIdentityManagement::Signature &signature,
                         KIdentityManagement::Signature::Placement placement,
                         KIdentityManagement::Signature::AddedText addedText);
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
