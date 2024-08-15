/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "converthtmltoplaintext.h"

#include <KPIMTextEdit/MarkupDirector>
#include <KPIMTextEdit/PlainTextMarkupBuilder>
#include <QTextDocument>

using namespace MimeTreeParser;
ConvertHtmlToPlainText::ConvertHtmlToPlainText() = default;

ConvertHtmlToPlainText::~ConvertHtmlToPlainText() = default;

void ConvertHtmlToPlainText::setHtmlString(const QString &htmlString)
{
    mHtmlString = htmlString;
}

QString ConvertHtmlToPlainText::generatePlainText()
{
    if (mHtmlString.isEmpty()) {
        return {};
    }
    auto pb = new KPIMTextEdit::PlainTextMarkupBuilder();

    auto pmd = new KPIMTextEdit::MarkupDirector(pb);
    auto doc = new QTextDocument;
    doc->setHtml(mHtmlString);

    pmd->processDocument(doc);
    QString plainText = pb->getResult();

    delete doc;
    delete pmd;
    delete pb;
    toCleanPlainText(plainText);
    return plainText;
}

QString ConvertHtmlToPlainText::htmlString() const
{
    return mHtmlString;
}

// Duplicate from kpimtextedit/textedit.h
void ConvertHtmlToPlainText::toCleanPlainText(QString &text)
{
    // Remove line separators. Normal \n chars are still there, so no linebreaks get lost here
    text.remove(QChar::LineSeparator);

    // Get rid of embedded images, see QTextImageFormat documentation:
    // "Inline images are represented by an object replacement character (0xFFFC in Unicode) "
    text.remove(QChar(0xFFFC));

    // In plaintext mode, each space is non-breaking.
    text.replace(QChar::Nbsp, QLatin1Char(' '));
}
