/* SPDX-FileCopyrightText: 2011-2020 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEMPLATEPARSER_TEMPLATESTEXTEDIT_H
#define TEMPLATEPARSER_TEMPLATESTEXTEDIT_H

#include "templateparser_export.h"

#include <KPIMTextEdit/PlainTextEditorWidget>

namespace TemplateParser {
/**
 * @brief The TemplatesTextEdit class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesTextEdit : public KPIMTextEdit::PlainTextEditorWidget
{
    Q_OBJECT
public:
    explicit TemplatesTextEdit(QWidget *parent = nullptr);
    ~TemplatesTextEdit();
};
}

#endif
