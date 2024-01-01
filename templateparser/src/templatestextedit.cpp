/* SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "templatestextedit.h"
#include "templatestexteditor.h"

using namespace TemplateParser;

TemplatesTextEdit::TemplatesTextEdit(QWidget *parent)
    : TextCustomEditor::PlainTextEditorWidget(new TemplatesTextEditor, parent)
{
}

TemplatesTextEdit::~TemplatesTextEdit() = default;

#include "moc_templatestextedit.cpp"
