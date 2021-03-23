/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KPIMTextEdit/PlainTextEditor>

#include <KSyntaxHighlighting/Repository>

class QKeyEvent;
namespace KPIMTextEdit
{
class TextEditorCompleter;
}
namespace TemplateParser
{
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
