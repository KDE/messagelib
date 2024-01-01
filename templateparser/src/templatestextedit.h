/* SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <TextCustomEditor/PlainTextEditorWidget>

namespace TemplateParser
{
/**
 * @brief The TemplatesTextEdit class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesTextEdit : public TextCustomEditor::PlainTextEditorWidget
{
    Q_OBJECT
public:
    explicit TemplatesTextEdit(QWidget *parent = nullptr);
    ~TemplatesTextEdit() override;
};
}
