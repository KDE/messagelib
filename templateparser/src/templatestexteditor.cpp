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

#include "templatestexteditor.h"
#include "templatesutil_p.h"

#include <KPIMTextEdit/TextEditorCompleter>
#include <KPIMTextEdit/PlainTextSyntaxSpellCheckingHighlighter>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <QKeyEvent>
#include <QAbstractItemView>
#include <QFontDatabase>
#include <QCompleter>
using namespace TemplateParser;

TemplatesTextEditor::TemplatesTextEditor(QWidget *parent)
    : KPIMTextEdit::PlainTextEditor(parent)
{
    setFocus();
    const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    QStringList excludeKeyWord;
    const QStringList lst = TemplateParser::Util::keywords();
    for (QString str : lst) {
        excludeKeyWord << str.remove(QLatin1Char('%'));
        excludeKeyWord << str.replace(QLatin1String("\\("), QLatin1String("("));
    }
    addIgnoreWords(excludeKeyWord);
    setWordWrapMode(QTextOption::NoWrap);
    initCompleter();
    createHighlighter();
}

TemplatesTextEditor::~TemplatesTextEditor()
{
}

void TemplatesTextEditor::updateHighLighter()
{
    KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter *hlighter = dynamic_cast<KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter *>(highlighter());
    if (hlighter) {
        hlighter->toggleSpellHighlighting(checkSpellingEnabled());
    }
}

void TemplatesTextEditor::clearDecorator()
{
    //Nothing
}

void TemplatesTextEditor::createHighlighter()
{
    KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter *highlighter = new KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter(this);
    highlighter->toggleSpellHighlighting(checkSpellingEnabled());
    highlighter->setCurrentLanguage(spellCheckingLanguage());
    highlighter->setDefinition(mSyntaxRepo.definitionForName(QStringLiteral("KMail Template")));
    highlighter->setTheme((palette().color(QPalette::Base).lightness() < 128)
                          ? mSyntaxRepo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                          : mSyntaxRepo.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
    setHighlighter(highlighter);
}

void TemplatesTextEditor::initCompleter()
{
    QStringList listWord;
    QStringList excludeKeyWord;
    const QStringList lst = TemplateParser::Util::keywords();
    excludeKeyWord.reserve(lst.count());
    for (QString str : lst) {
        excludeKeyWord << str.replace(QLatin1String("\\("), QLatin1String("("));
    }
    listWord << excludeKeyWord;
    listWord << Util::keywordsWithArgs();

    mTextEditorCompleter = new KPIMTextEdit::TextEditorCompleter(this, this);
    mTextEditorCompleter->setCompleterStringList(listWord);
    mTextEditorCompleter->setExcludeOfCharacters(QStringLiteral("~!@#$^&*()+{}|\"<>,./;'[]\\-= "));
}

void TemplatesTextEditor::keyPressEvent(QKeyEvent *e)
{
    if (mTextEditorCompleter->completer()->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }
    KPIMTextEdit::PlainTextEditor::keyPressEvent(e);
    mTextEditorCompleter->completeText();
}
