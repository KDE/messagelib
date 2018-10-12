/*
   Copyright (C) 2015-2018 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef RICHTEXTCOMPOSERNG_H
#define RICHTEXTCOMPOSERNG_H

#include "messagecomposer_export.h"
#include <kpimtextedit/richtextcomposer.h>
#include <MessageComposer/PluginEditorConvertTextInterface>
#include <KIdentityManagement/Signature>

namespace PimCommon {
class AutoCorrection;
}

namespace MessageComposer {
class TextPart;
class RichTextComposerSignatures;
class RichTextComposerNgPrivate;
class MESSAGECOMPOSER_EXPORT RichTextComposerNg : public KPIMTextEdit::RichTextComposer
{
    Q_OBJECT
public:
    explicit RichTextComposerNg(QWidget *parent = nullptr);
    ~RichTextComposerNg() override;

    Q_REQUIRED_RESULT PimCommon::AutoCorrection *autocorrection() const;
    void setAutocorrection(PimCommon::AutoCorrection *autocorrect);
    void setAutocorrectionLanguage(const QString &lang);

    void fillComposerTextPart(MessageComposer::TextPart *textPart);
    Q_REQUIRED_RESULT MessageComposer::RichTextComposerSignatures *composerSignature() const;

    void insertSignature(const KIdentityManagement::Signature &signature, KIdentityManagement::Signature::Placement placement, KIdentityManagement::Signature::AddedText addedText);
    Q_REQUIRED_RESULT QString toCleanHtml() const;

    void forceAutoCorrection(bool selectedText = false) override;

    Q_REQUIRED_RESULT virtual MessageComposer::PluginEditorConvertTextInterface::ConvertTextStatus convertPlainText(MessageComposer::TextPart *textPart);

private:
    bool processAutoCorrection(QKeyEvent *event) override;
    RichTextComposerNgPrivate *const d;
};
}
#endif // RICHTEXTCOMPOSERNG_H
