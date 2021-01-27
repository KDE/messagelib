/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtextcomposersignatures.h"
#include "richtextcomposerng.h"

#include "messagecomposer_debug.h"
#include <KIdentityManagement/Signature>
#include <QTextBlock>
using namespace MessageComposer;

class Q_DECL_HIDDEN RichTextComposerSignatures::RichTextComposerSignaturesPrivate
{
public:
    RichTextComposerSignaturesPrivate(RichTextComposerNg *composer)
        : richTextComposer(composer)
    {
    }

    void cleanWhitespaceHelper(const QRegExp &regExp, const QString &newText, const KIdentityManagement::Signature &sig);
    Q_REQUIRED_RESULT QVector<QPair<int, int>> signaturePositions(const KIdentityManagement::Signature &sig) const;
    RichTextComposerNg *const richTextComposer;
};

RichTextComposerSignatures::RichTextComposerSignatures(MessageComposer::RichTextComposerNg *composer, QObject *parent)
    : QObject(parent)
    , d(new RichTextComposerSignaturesPrivate(composer))
{
}

RichTextComposerSignatures::~RichTextComposerSignatures()
{
    delete d;
}

void RichTextComposerSignatures::RichTextComposerSignaturesPrivate::cleanWhitespaceHelper(const QRegExp &regExp,
                                                                                          const QString &newText,
                                                                                          const KIdentityManagement::Signature &sig)
{
    int currentSearchPosition = 0;

    forever {
        // Find the text
        const QString text = richTextComposer->document()->toPlainText();
        const int currentMatch = regExp.indexIn(text, currentSearchPosition);
        currentSearchPosition = currentMatch;
        if (currentMatch == -1) {
            break;
        }

        // Select the text
        QTextCursor cursor(richTextComposer->document());
        cursor.setPosition(currentMatch);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, regExp.matchedLength());
        // Skip quoted text
        if (richTextComposer->isLineQuoted(cursor.block().text())) {
            currentSearchPosition += regExp.matchedLength();
            continue;
        }
        // Skip text inside signatures
        bool insideSignature = false;
        const QVector<QPair<int, int>> sigPositions = signaturePositions(sig);
        for (const QPair<int, int> &position : sigPositions) {
            if (cursor.position() >= position.first && cursor.position() <= position.second) {
                insideSignature = true;
            }
        }
        if (insideSignature) {
            currentSearchPosition += regExp.matchedLength();
            continue;
        }

        // Replace the text
        cursor.removeSelectedText();
        cursor.insertText(newText);
        currentSearchPosition += newText.length();
    }
}

void RichTextComposerSignatures::cleanWhitespace(const KIdentityManagement::Signature &sig)
{
    QTextCursor cursor(d->richTextComposer->document());
    cursor.beginEditBlock();

    // Squeeze tabs and spaces
    d->cleanWhitespaceHelper(QRegExp(QLatin1String("[\t ]+")), QStringLiteral(" "), sig);

    // Remove trailing whitespace
    d->cleanWhitespaceHelper(QRegExp(QLatin1String("[\t ][\n]")), QStringLiteral("\n"), sig);

    // Single space lines
    d->cleanWhitespaceHelper(QRegExp(QLatin1String("[\n]{3,}")), QStringLiteral("\n\n"), sig);

    if (!d->richTextComposer->textCursor().hasSelection()) {
        d->richTextComposer->textCursor().clearSelection();
    }

    cursor.endEditBlock();
}

QVector<QPair<int, int>> RichTextComposerSignatures::RichTextComposerSignaturesPrivate::signaturePositions(const KIdentityManagement::Signature &sig) const
{
    QVector<QPair<int, int>> signaturePositions;
    if (!sig.rawText().isEmpty()) {
        QString sigText = sig.toPlainText();

        int currentSearchPosition = 0;
        forever {
            // Find the next occurrence of the signature text
            const QString text = richTextComposer->document()->toPlainText();
            const int currentMatch = text.indexOf(sigText, currentSearchPosition);
            currentSearchPosition = currentMatch + sigText.length();
            if (currentMatch == -1) {
                break;
            }

            signaturePositions.append(QPair<int, int>(currentMatch, currentMatch + sigText.length()));
        }
    }
    return signaturePositions;
}

bool RichTextComposerSignatures::replaceSignature(const KIdentityManagement::Signature &oldSig, const KIdentityManagement::Signature &newSig)
{
    bool found = false;
    if (oldSig == newSig) {
        return false;
    }
    QString oldSigText = oldSig.toPlainText();
    if (oldSigText.isEmpty()) {
        return false;
    }
    QTextCursor cursor(d->richTextComposer->document());
    cursor.beginEditBlock();
    int currentSearchPosition = 0;
    forever {
        // Find the next occurrence of the signature text
        const QString text = d->richTextComposer->document()->toPlainText();
        const int currentMatch = text.indexOf(oldSigText, currentSearchPosition);
        currentSearchPosition = currentMatch;
        if (currentMatch == -1) {
            break;
        }

        // Select the signature
        QTextCursor cursor(d->richTextComposer->document());
        cursor.setPosition(currentMatch);

        // If the new signature is completely empty, we also want to remove the
        // signature separator, so include it in the selection
        int additionalMove = 0;
        if (newSig.rawText().isEmpty() && text.mid(currentMatch - 4, 4) == QLatin1String("-- \n")) {
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 4);
            additionalMove = 4;
        } else if (newSig.rawText().isEmpty() && text.mid(currentMatch - 1, 1) == QLatin1Char('\n')) {
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 1);
            additionalMove = 1;
        }
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, oldSigText.length() + additionalMove);
        // Skip quoted signatures
        if (d->richTextComposer->isLineQuoted(cursor.block().text())) {
            currentSearchPosition += oldSig.toPlainText().length();
            continue;
        }
        // Remove the old and insert the new signature
        cursor.removeSelectedText();
        d->richTextComposer->setTextCursor(cursor);
        d->richTextComposer->insertSignature(newSig, KIdentityManagement::Signature::AtCursor, KIdentityManagement::Signature::AddNothing);
        found = true;

        currentSearchPosition += newSig.toPlainText().length();
    }

    cursor.endEditBlock();
    return found;
}
