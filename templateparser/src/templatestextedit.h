/* SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <KPIMTextEdit/PlainTextEditorWidget>

namespace TemplateParser
{
/**
 * @brief The TemplatesTextEdit class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesTextEdit : public KPIMTextEdit::PlainTextEditorWidget
{
    Q_OBJECT
public:
    explicit TemplatesTextEdit(QWidget *parent = nullptr);
    ~TemplatesTextEdit() override;
};
}

