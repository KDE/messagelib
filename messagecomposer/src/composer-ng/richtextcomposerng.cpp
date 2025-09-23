/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "richtextcomposerng.h"
#include "config-messagecomposer.h"
#include "richtextcomposersignatures.h"
#include "settings/messagecomposersettings.h"
#include <KPIMTextEdit/MarkupDirector>
#include <KPIMTextEdit/PlainTextMarkupBuilder>
#include <KPIMTextEdit/RichTextComposerControler>
#include <KPIMTextEdit/RichTextComposerImages>
#include <KPIMTextEdit/TextHTMLBuilder>

#include <TextAutoCorrectionCore/AutoCorrection>

#include "part/textpart.h"

#include <KMessageBox>

#include <QRegularExpression>
#if HAVE_TEXTADDONSWIDGET_RICHTEXTQUICKTEXTFORMAT
#include <TextAddonsWidgets/RichTextQuickTextFormat>
#endif
using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

class MessageComposer::RichTextComposerNgPrivate
{
public:
    explicit RichTextComposerNgPrivate(RichTextComposerNg *q)
        : richtextComposer(q)
        , richTextComposerSignatures(new MessageComposer::RichTextComposerSignatures(richtextComposer, richtextComposer))
    {
    }

    void fixHtmlFontSize(QString &cleanHtml) const;
    [[nodiscard]] QString toCleanHtml() const;
    TextAutoCorrectionCore::AutoCorrection *autoCorrection = nullptr;
    RichTextComposerNg *const richtextComposer;
    MessageComposer::RichTextComposerSignatures *const richTextComposerSignatures;
#if HAVE_TEXTADDONSWIDGET_RICHTEXTQUICKTEXTFORMAT
    TextAddonsWidgets::RichTextQuickTextFormat *richTextQuickTextFormat = nullptr;
#endif
};

RichTextComposerNg::RichTextComposerNg(QWidget *parent)
    : KPIMTextEdit::RichTextComposer(parent)
    , d(new MessageComposer::RichTextComposerNgPrivate(this))
{
#if HAVE_TEXTADDONSWIDGET_RICHTEXTQUICKTEXTFORMAT
    auto quicktextformatmessage = new TextAddonsWidgets::RichTextQuickTextFormat(this, this);
    TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatTypes formatTypes = TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Unknown;
    formatTypes |= TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Bold;
    formatTypes |= TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Italic;
    formatTypes |= TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::StrikeThrough;
    formatTypes |= TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::InsertLink;
    formatTypes |= TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::UnderLine;
    quicktextformatmessage->setFormatTypes(formatTypes);
    connect(quicktextformatmessage,
            &TextAddonsWidgets::RichTextQuickTextFormat::quickTextFormatRequested,
            this,
            [this](TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType type) {
                switch (type) {
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::UnderLine: {
                    applyUnderlineFormat();
                    break;
                }
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Bold: {
                    applyBoldFormat();
                    break;
                }
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Italic: {
                    applyItalicFormat();
                    break;
                }
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::StrikeThrough: {
                    applyStrikeOutFormat();
                    break;
                }
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::InsertLink:
                    applyInsertLink();
                    break;
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::CodeBlock:
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::BlockQuote:
                case TextAddonsWidgets::RichTextQuickTextFormat::QuickTextFormatType::Unknown:
                    break;
                }
            });
    quicktextformatmessage->setEnabled(MessageComposer::MessageComposerSettings::activateQuickTextFormat());

#endif
}

RichTextComposerNg::~RichTextComposerNg() = default;

void RichTextComposerNg::applyInsertLink()
{
    if (textMode() == KPIMTextEdit::RichTextComposer::Plain) {
        // TODO
    } else {
        composerControler()->manageLink();
    }
}

void RichTextComposerNg::applyUnderlineFormat()
{
    if (textMode() == KPIMTextEdit::RichTextComposer::Plain) {
        insertFormat(u'~');
    } else {
        composerControler()->setTextUnderline(true);
    }
}

void RichTextComposerNg::applyBoldFormat()
{
    if (textMode() == KPIMTextEdit::RichTextComposer::Plain) {
        insertFormat(u'*');
    } else {
        composerControler()->setTextBold(true);
    }
}

void RichTextComposerNg::applyItalicFormat()
{
    if (textMode() == KPIMTextEdit::RichTextComposer::Plain) {
        insertFormat(u'/');
    } else {
        composerControler()->setTextItalic(true);
    }
}

void RichTextComposerNg::applyStrikeOutFormat()
{
    if (textMode() == KPIMTextEdit::RichTextComposer::Plain) {
        insertFormat(u'_');
    } else {
        composerControler()->setTextStrikeOut(true);
    }
}

void RichTextComposerNg::insertFormat(QChar formatChar)
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        const QString newText = formatChar + cursor.selectedText() + formatChar;
        cursor.insertText(newText);
    } else {
        cursor.insertText(QString(formatChar) + QString(formatChar));
    }
    cursor.setPosition(cursor.position());
    setTextCursor(cursor);
}

MessageComposer::RichTextComposerSignatures *RichTextComposerNg::composerSignature() const
{
    return d->richTextComposerSignatures;
}

TextAutoCorrectionCore::AutoCorrection *RichTextComposerNg::autocorrection() const
{
    return d->autoCorrection;
}

void RichTextComposerNg::setAutocorrection(TextAutoCorrectionCore::AutoCorrection *autocorrect)
{
    d->autoCorrection = autocorrect;
}

void RichTextComposerNg::setAutocorrectionLanguage(const QString &lang)
{
    if (d->autoCorrection) {
        TextAutoCorrectionCore::AutoCorrectionSettings *settings = d->autoCorrection->autoCorrectionSettings();
        settings->setLanguage(lang);
        d->autoCorrection->setAutoCorrectionSettings(settings);
    }
}

static bool isSpecial(const QTextCharFormat &charFormat)
{
    return charFormat.isFrameFormat() || charFormat.isImageFormat() || charFormat.isListFormat() || charFormat.isTableFormat()
        || charFormat.isTableCellFormat();
}

bool RichTextComposerNg::processModifyText(QKeyEvent *e)
{
    if (d->autoCorrection && d->autoCorrection->autoCorrectionSettings()->isEnabledAutoCorrection()) {
        if ((e->key() == Qt::Key_Space) || (e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)) {
            if (!isLineQuoted(textCursor().block().text()) && !textCursor().hasSelection()) {
                const QTextCharFormat initialTextFormat = textCursor().charFormat();
                const bool richText = (textMode() == RichTextComposer::Rich);
                int position = textCursor().position();
                const bool addSpace = d->autoCorrection->autocorrect(richText, *document(), position);
                QTextCursor cur = textCursor();
                cur.setPosition(position);
                const bool spacePressed = (e->key() == Qt::Key_Space);
                if (overwriteMode() && spacePressed) {
                    if (addSpace) {
                        const QChar insertChar = u' ';
                        if (!cur.atBlockEnd()) {
                            cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
                        }
                        if (richText && !isSpecial(initialTextFormat)) {
                            cur.insertText(insertChar, initialTextFormat);
                        } else {
                            cur.insertText(insertChar);
                        }
                        setTextCursor(cur);
                    }
                } else {
                    const QChar insertChar = spacePressed ? u' ' : u'\n';
                    if (richText && !isSpecial(initialTextFormat)) {
                        if ((spacePressed && addSpace) || !spacePressed) {
                            cur.insertText(insertChar, initialTextFormat);
                        }
                    } else {
                        if ((spacePressed && addSpace) || !spacePressed) {
                            cur.insertText(insertChar);
                        }
                    }
                    setTextCursor(cur);
                }
                return true;
            }
        }
    }
    return false;
}

void RichTextComposerNgPrivate::fixHtmlFontSize(QString &cleanHtml) const
{
    // non-greedy matching
    static const QRegularExpression styleRegex(u"<span style=\".*?font-size:(.*?)pt;.*?</span>"_s);

    QRegularExpressionMatch rmatch;
    int offset = 0;
    while (cleanHtml.indexOf(styleRegex, offset, &rmatch) != -1) {
        bool ok = false;
        const double ptValue = rmatch.captured(1).toDouble(&ok);
        if (ok) {
            const double emValue = ptValue / 12;
            const QString replacement = QString::number(emValue, 'g', 2);
            const int capLen = rmatch.capturedLength(1);
            cleanHtml.replace(rmatch.capturedStart(1), capLen + 2 /* QLatin1StringView("pt").size() */, replacement + QLatin1StringView("em"));
            // advance the offset to just after the last replace
            offset = rmatch.capturedEnd(0) - capLen + replacement.size();
        } else {
            // a match was found but the toDouble call failed, advance the offset to just after
            // the entire match
            offset = rmatch.capturedEnd(0);
        }
    }
}

MessageComposer::PluginEditorConvertTextInterface::ConvertTextStatus RichTextComposerNg::convertPlainText(MessageComposer::TextPart *textPart)
{
    Q_UNUSED(textPart)
    return MessageComposer::PluginEditorConvertTextInterface::ConvertTextStatus::NotConverted;
}

void RichTextComposerNg::fillComposerTextPart(MessageComposer::TextPart *textPart)
{
    const bool wasConverted = convertPlainText(textPart) == MessageComposer::PluginEditorConvertTextInterface::ConvertTextStatus::Converted;
    if (composerControler()->isFormattingUsed() && !wasConverted) {
        if (MessageComposer::MessageComposerSettings::self()->improvePlainTextOfHtmlMessage()) {
            KPIMTextEdit::PlainTextMarkupBuilder pb;

            KPIMTextEdit::MarkupDirector pmd(&pb);
            pmd.processDocument(document());
            const QString plainText = pb.getResult();
            textPart->setCleanPlainText(composerControler()->toCleanPlainText(plainText));
            QTextDocument doc(plainText);
            doc.adjustSize();

            textPart->setWrappedPlainText(composerControler()->toWrappedPlainText(&doc));
        } else {
            textPart->setCleanPlainText(composerControler()->toCleanPlainText());
            textPart->setWrappedPlainText(composerControler()->toWrappedPlainText());
        }
    } else if (!wasConverted) {
        textPart->setCleanPlainText(composerControler()->toCleanPlainText());
        textPart->setWrappedPlainText(composerControler()->toWrappedPlainText());
    }

    textPart->setWordWrappingEnabled(lineWrapMode() == QTextEdit::FixedColumnWidth);
    if (composerControler()->isFormattingUsed() && !wasConverted) {
        KPIMTextEdit::TextHTMLBuilder pb;

        KPIMTextEdit::MarkupDirector pmd(&pb);
        pmd.processDocument(document());
        QString cleanHtml =
            u"<html>\n<head>\n<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n</head>\n<body>%1</body>\n</html>"_s.arg(pb.getResult());
        d->fixHtmlFontSize(cleanHtml);
        textPart->setCleanHtml(cleanHtml);
        // qDebug() << " cleanHtml  grantlee builder" << cleanHtml;
        // qDebug() << " d->toCleanHtml() " << d->toCleanHtml();
        textPart->setEmbeddedImages(composerControler()->composerImages()->embeddedImages());
    }
}

QString RichTextComposerNgPrivate::toCleanHtml() const
{
    QString result = richtextComposer->toHtml();

    static const QString EMPTYLINEHTML = QStringLiteral(
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; "
        "margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; \">&nbsp;</p>");

    // Qt inserts various style properties based on the current mode of the editor (underline,
    // bold, etc), but only empty paragraphs *also* have qt-paragraph-type set to 'empty'.
    // Minimal/non-greedy matching
    static const QString EMPTYLINEREGEX = u"<p style=\"-qt-paragraph-type:empty;(?:.*?)</p>"_s;

    static const QString OLLISTPATTERNQT = u"<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px;"_s;

    static const QString ULLISTPATTERNQT = u"<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px;"_s;

    static const QString ORDEREDLISTHTML = u"<ol style=\"margin-top: 0px; margin-bottom: 0px;"_s;

    static const QString UNORDEREDLISTHTML = u"<ul style=\"margin-top: 0px; margin-bottom: 0px;"_s;

    // fix 1 - empty lines should show as empty lines - MS Outlook treats margin-top:0px; as
    // a non-existing line.
    // Although we can simply remove the margin-top style property, we still get unwanted results
    // if you have three or more empty lines. It's best to replace empty <p> elements with <p>&nbsp;</p>.

    // Replace all the matching text with the new line text
    result.replace(QRegularExpression(EMPTYLINEREGEX), EMPTYLINEHTML);

    // fix 2a - ordered lists - MS Outlook treats margin-left:0px; as
    // a non-existing number; e.g: "1. First item" turns into "First Item"
    result.replace(OLLISTPATTERNQT, ORDEREDLISTHTML);

    // fix 2b - unordered lists - MS Outlook treats margin-left:0px; as
    // a non-existing bullet; e.g: "* First bullet" turns into "First Bullet"
    result.replace(ULLISTPATTERNQT, UNORDEREDLISTHTML);

    return result;
}

static bool isCursorAtEndOfLine(const QTextCursor &cursor)
{
    QTextCursor testCursor = cursor;
    testCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    return !testCursor.hasSelection();
}

static void insertSignatureHelper(const QString &signature,
                                  RichTextComposerNg *textEdit,
                                  KIdentityManagementCore::Signature::Placement placement,
                                  bool isHtml,
                                  bool addNewlines)
{
    if (!signature.isEmpty()) {
        // Save the modified state of the document, as inserting a signature
        // shouldn't change this. Restore it at the end of this function.
        bool isModified = textEdit->document()->isModified();

        // Move to the desired position, where the signature should be inserted
        QTextCursor cursor = textEdit->textCursor();
        QTextCursor oldCursor = cursor;
        cursor.beginEditBlock();

        if (placement == KIdentityManagementCore::Signature::End) {
            cursor.movePosition(QTextCursor::End);
        } else if (placement == KIdentityManagementCore::Signature::Start) {
            cursor.movePosition(QTextCursor::Start);
        } else if (placement == KIdentityManagementCore::Signature::AtCursor) {
            cursor.movePosition(QTextCursor::StartOfLine);
        }
        textEdit->setTextCursor(cursor);

        QString lineSep;
        if (addNewlines) {
            if (isHtml) {
                lineSep = u"<br>"_s;
            } else {
                lineSep = u'\n';
            }
        }

        // Insert the signature and newlines depending on where it was inserted.
        int newCursorPos = -1;
        QString headSep;
        QString tailSep;

        if (placement == KIdentityManagementCore::Signature::End) {
            // There is one special case when re-setting the old cursor: The cursor
            // was at the end. In this case, QTextEdit has no way to know
            // if the signature was added before or after the cursor, and just
            // decides that it was added before (and the cursor moves to the end,
            // but it should not when appending a signature). See bug 167961
            if (oldCursor.position() == textEdit->toPlainText().length()) {
                newCursorPos = oldCursor.position();
            }
            headSep = lineSep;
        } else if (placement == KIdentityManagementCore::Signature::Start) {
            // When prepending signatures, add a couple of new lines before
            // the signature, and move the cursor to the beginning of the QTextEdit.
            // People tends to insert new text there.
            newCursorPos = 0;
            headSep = lineSep + lineSep;
            if (!isCursorAtEndOfLine(cursor)) {
                tailSep = lineSep;
            }
        } else if (placement == KIdentityManagementCore::Signature::AtCursor) {
            if (!isCursorAtEndOfLine(cursor)) {
                tailSep = lineSep;
            }
        }

        const QString full_signature = headSep + signature + tailSep;
        if (isHtml) {
            textEdit->insertHtml(full_signature);
        } else {
            textEdit->insertPlainText(full_signature);
        }

        cursor.endEditBlock();
        if (newCursorPos != -1) {
            oldCursor.setPosition(newCursorPos);
        }

        textEdit->setTextCursor(oldCursor);
        textEdit->ensureCursorVisible();

        textEdit->document()->setModified(isModified);

        if (isHtml) {
            textEdit->activateRichText();
        }
    }
}

void RichTextComposerNg::insertSignature(const KIdentityManagementCore::Signature &signature,
                                         KIdentityManagementCore::Signature::Placement placement,
                                         KIdentityManagementCore::Signature::AddedText addedText)
{
    if (signature.isEnabledSignature()) {
        QString signatureStr;
        bool ok = false;
        QString errorMessage;
        if (addedText & KIdentityManagementCore::Signature::AddSeparator) {
            signatureStr = signature.withSeparator(&ok, &errorMessage);
        } else {
            signatureStr = signature.rawText(&ok, &errorMessage);
        }

        if (!ok && !errorMessage.isEmpty()) {
            KMessageBox::error(nullptr, errorMessage);
        }

        insertSignatureHelper(signatureStr,
                              this,
                              placement,
                              (signature.isInlinedHtml() && signature.type() == KIdentityManagementCore::Signature::Inlined),
                              (addedText & KIdentityManagementCore::Signature::AddNewLines));

        // We added the text of the signature above, now it is time to add the images as well.
        if (signature.isInlinedHtml()) {
            const QList<KIdentityManagementCore::Signature::EmbeddedImagePtr> embeddedImages = signature.embeddedImages();
            for (const KIdentityManagementCore::Signature::EmbeddedImagePtr &image : embeddedImages) {
                composerControler()->composerImages()->loadImage(image->image, image->name, image->name);
            }
        }
    }
}

QString RichTextComposerNg::toCleanHtml() const
{
    return d->toCleanHtml();
}

void RichTextComposerNg::fixHtmlFontSize(QString &cleanHtml) const
{
    d->fixHtmlFontSize(cleanHtml);
}

void RichTextComposerNg::forceAutoCorrection(bool selectedText)
{
    if (document()->isEmpty()) {
        return;
    }
    if (d->autoCorrection && d->autoCorrection->autoCorrectionSettings()->isEnabledAutoCorrection()) {
        const bool richText = (textMode() == RichTextComposer::Rich);
        const int initialPosition = textCursor().position();
        QTextCursor cur = textCursor();
        cur.beginEditBlock();
        if (selectedText && cur.hasSelection()) {
            const int positionStart = qMin(cur.selectionEnd(), cur.selectionStart());
            const int positionEnd = qMax(cur.selectionEnd(), cur.selectionStart());
            cur.setPosition(positionStart);
            int cursorPosition = positionStart;
            while (cursorPosition < positionEnd) {
                if (isLineQuoted(cur.block().text())) {
                    cur.movePosition(QTextCursor::NextBlock);
                } else {
                    cur.movePosition(QTextCursor::NextWord);
                }
                cursorPosition = cur.position();
                (void)d->autoCorrection->autocorrect(richText, *document(), cursorPosition);
            }
        } else {
            cur.movePosition(QTextCursor::Start);
            while (!cur.atEnd()) {
                if (isLineQuoted(cur.block().text())) {
                    cur.movePosition(QTextCursor::NextBlock);
                } else {
                    cur.movePosition(QTextCursor::NextWord);
                }
                int cursorPosition = cur.position();
                (void)d->autoCorrection->autocorrect(richText, *document(), cursorPosition);
            }
        }
        cur.endEditBlock();
        if (cur.position() != initialPosition) {
            cur.setPosition(initialPosition);
            setTextCursor(cur);
        }
    }
}

#include "moc_richtextcomposerng.cpp"
