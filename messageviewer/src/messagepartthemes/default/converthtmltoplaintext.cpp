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
    KPIMTextEdit::PlainTextMarkupBuilder pb;
    KPIMTextEdit::MarkupDirector pmd(&pb);
    QTextDocument doc;

    doc.setHtml(mHtmlString);
    pmd.processDocument(&doc);
    QString plainText = pb.getResult();

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

    text = text.trimmed();

    // In plaintext mode, each space is non-breaking.
    text.replace(QChar::Nbsp, QChar::fromLatin1(' '));
}
