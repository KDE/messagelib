/*
  Copyright (c) 2013-2018 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TEMPLATESTEXTEDITOR_H
#define TEMPLATESTEXTEDITOR_H

#include "kpimtextedit/plaintexteditor.h"

#include <KSyntaxHighlighting/Repository>

class QKeyEvent;
namespace KPIMTextEdit {
class TextEditorCompleter;
}
namespace TemplateParser {
class TemplatesTextEditor : public KPIMTextEdit::PlainTextEditor
{
    Q_OBJECT
public:
    explicit TemplatesTextEditor(QWidget *parent = nullptr);
    ~TemplatesTextEditor() override;

protected:
    void initCompleter();
    void keyPressEvent(QKeyEvent *e) override;

    void updateHighLighter() override;

    void clearDecorator() override;
    void createHighlighter() override;
private:
    KPIMTextEdit::TextEditorCompleter *mTextEditorCompleter = nullptr;
    KSyntaxHighlighting::Repository mSyntaxRepo;
};
}
#endif // TEMPLATESTEXTEDITOR_H
