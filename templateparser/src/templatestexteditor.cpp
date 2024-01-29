/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templatestexteditor.h"
#include "templatesutil_p.h"

#include <TextCustomEditor/PlainTextSyntaxSpellCheckingHighlighter>
#include <TextCustomEditor/TextEditorCompleter>

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Theme>

#include <QAbstractItemView>
#include <QCompleter>
#include <QFontDatabase>
#include <QKeyEvent>
using namespace TemplateParser;

TemplatesTextEditor::TemplatesTextEditor(QWidget *parent)
    : TextCustomEditor::PlainTextEditor(parent)
{
    setFocus();
    const QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFont(f);
    QStringList excludeKeyWord;
    const QStringList lst = TemplateParser::Util::keywords();
    excludeKeyWord.reserve(lst.count() * 2);
    for (QString str : lst) {
        excludeKeyWord << str.remove(QLatin1Char('%'));
        excludeKeyWord << str.replace(QLatin1StringView("\\("), QLatin1StringView("("));
    }
    addIgnoreWords(excludeKeyWord);
    setWordWrapMode(QTextOption::NoWrap);
    initCompleter();
    createHighlighter();
}

TemplatesTextEditor::~TemplatesTextEditor() = default;

void TemplatesTextEditor::updateHighLighter()
{
    auto hlighter = dynamic_cast<TextCustomEditor::PlainTextSyntaxSpellCheckingHighlighter *>(highlighter());
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
    auto highlighter = new TextCustomEditor::PlainTextSyntaxSpellCheckingHighlighter(this);
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
        excludeKeyWord << str.replace(QLatin1StringView("\\("), QLatin1StringView("("));
    }
    listWord << excludeKeyWord;
    listWord << Util::keywordsWithArgs();

    mTextEditorCompleter = new TextCustomEditor::TextEditorCompleter(this, this);
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
    TextCustomEditor::PlainTextEditor::keyPressEvent(e);
    mTextEditorCompleter->completeText();
}

#include "moc_templatestexteditor.cpp"
