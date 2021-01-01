/* SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "templatestextedit.h"
#include "templatestexteditor.h"

using namespace TemplateParser;

TemplatesTextEdit::TemplatesTextEdit(QWidget *parent)
    : KPIMTextEdit::PlainTextEditorWidget(new TemplatesTextEditor, parent)
{
}

TemplatesTextEdit::~TemplatesTextEdit()
{
}
