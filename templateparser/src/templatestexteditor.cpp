/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templatestexteditor.h"
#include "templatesutil_p.h"

#include <KPIMTextEdit/PlainTextSyntaxSpellCheckingHighlighter>
#include <KPIMTextEdit/TextEditorCompleter>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <QAbstractItemView>
#include <QCompleter>
#include <QFontDatabase>
#include <QKeyEvent>
using namespace TemplateParser;

TemplatesTextEditor::TemplatesTextEditor(QWidget *parent)
    : KPIMTextEdit::PlainTextEditor(parent)
{
    setFocus();
    const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    QStringList excludeKeyWord;
    const QStringList lst = TemplateParser::Util::keywords();
    excludeKeyWord.reserve(lst.count() * 2);
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
    auto hlighter = dynamic_cast<KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter *>(highlighter());
    if (hlighter) {
        hlighter->toggleSpellHighlighting(checkSpellingEnabled());
    }
}

void TemplatesTextEditor::clearDecorator()
{
    // Nothing
}

void TemplatesTextEditor::createHighlighter()
{
    auto highlighter = new KPIMTextEdit::PlainTextSyntaxSpellCheckingHighlighter(this);
    highlighter->toggleSpellHighlighting(checkSpellingEnabled());
    highlighter->setCurrentLanguage(spellCheckingLanguage());
    highlighter->setDefinition(mSyntaxRepo.definitionForName(QStringLiteral("KMail Template")));
    highlighter->setTheme((palette().color(QPalette::Base).lightness() < 128) ? mSyntaxRepo.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
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
