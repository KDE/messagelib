/*
   Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#include "messagepart.h"
#include "mimetreeparser_debug.h"
#include "converthtmltoplaintext.h"
#include "csshelperbase.h"
#include "cryptohelper.h"
#include "htmlblock.h"
#include "objecttreeparser.h"
#include "interfaces/htmlwriter.h"
#include "job/kleojobexecutor.h"
#include "memento/decryptverifybodypartmemento.h"
#include "memento/verifydetachedbodypartmemento.h"
#include "memento/verifyopaquebodypartmemento.h"
#include "utils/iconnamecache.h"

#include <KMime/Content>
#include <MessageCore/StringUtil>

#include <Libkleo/Dn>
#include <Libkleo/ImportJob>
#include <Libkleo/CryptoBackendFactory>
#include <Libkleo/KeyListJob>
#include <Libkleo/VerifyDetachedJob>
#include <Libkleo/VerifyOpaqueJob>

#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme.h>

#include <QFile>
#include <QTextCodec>
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>

#include <KLocalizedString>
#include <KTextToHTML>

#include <sstream>

using namespace MimeTreeParser;

/** Checks whether @p str contains external references. To be precise,
    we only check whether @p str contains 'xxx="http[s]:' where xxx is
    not href. Obfuscated external references are ignored on purpose.
*/

bool containsExternalReferences(const QString &str, const QString &extraHead)
{
    const bool hasBaseInHeader = extraHead.contains(QStringLiteral("<base href=\""), Qt::CaseInsensitive);
    if (hasBaseInHeader && (str.contains(QStringLiteral("href=\"/"), Qt::CaseInsensitive) ||
                            str.contains(QStringLiteral("<img src=\"/"), Qt::CaseInsensitive))) {
        return true;
    }
    /*
    //Laurent: workaround for local ref cid
    if(str.contains(QStringLiteral("<img src=\"cid:"),Qt::CaseInsensitive)) {
    return true;
    }
    */
    int httpPos = str.indexOf(QLatin1String("\"http:"), Qt::CaseInsensitive);
    int httpsPos = str.indexOf(QLatin1String("\"https:"), Qt::CaseInsensitive);

    while (httpPos >= 0 || httpsPos >= 0) {
        // pos = index of next occurrence of "http: or "https: whichever comes first
        int pos = (httpPos < httpsPos)
                  ? ((httpPos >= 0) ? httpPos : httpsPos)
                  : ((httpsPos >= 0) ? httpsPos : httpPos);
        // look backwards for "href"
        if (pos > 5) {
            int hrefPos = str.lastIndexOf(QLatin1String("href"), pos - 5, Qt::CaseInsensitive);
            // if no 'href' is found or the distance between 'href' and '"http[s]:'
            // is larger than 7 (7 is the distance in 'href = "http[s]:') then
            // we assume that we have found an external reference
            if ((hrefPos == -1) || (pos - hrefPos > 7)) {

                // HTML messages created by KMail itself for now contain the following:
                // <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
                // Make sure not to show an external references warning for this string
                int dtdPos = str.indexOf(QLatin1String("http://www.w3.org/TR/html4/loose.dtd"), pos + 1);
                if (dtdPos != (pos + 1)) {
                    return true;
                }
            }
        }
        // find next occurrence of "http: or "https:
        if (pos == httpPos) {
            httpPos = str.indexOf(QLatin1String("\"http:"), httpPos + 6, Qt::CaseInsensitive);
        } else {
            httpsPos = str.indexOf(QLatin1String("\"https:"), httpsPos + 7, Qt::CaseInsensitive);
        }
    }
    return false;
}

//------MessagePart-----------------------
MessagePart::MessagePart(ObjectTreeParser *otp,
                         const QString &text)
    : mText(text)
    , mOtp(otp)
    , mSubOtp(Q_NULLPTR)
    , mAttachmentNode(Q_NULLPTR)
    , mRoot(false)
    , mInternalAttachmentNode(Q_NULLPTR)
    , mIsInternalRoot(false)
{

}

MessagePart::~MessagePart()
{
    if (mSubOtp) {
        delete mSubOtp;
        mSubOtp = Q_NULLPTR;
    }
}

PartMetaData *MessagePart::partMetaData()
{
    return &mMetaData;
}

void MessagePart::setAttachmentFlag(KMime::Content *node)
{
    mAttachmentNode = node;
}

bool MessagePart::isAttachment() const
{
    return mAttachmentNode;
}

KMime::Content *MessagePart::attachmentNode() const
{
    return mAttachmentNode;
}

HTMLBlock::Ptr MessagePart::attachmentBlock() const
{
    if (mOtp->htmlWriter() && isAttachment()) {
        return HTMLBlock::Ptr(new AttachmentMarkBlock(mOtp->htmlWriter(), mAttachmentNode));
    }
    return HTMLBlock::Ptr();
}

void MessagePart::setIsRoot(bool root)
{
    mRoot = root;
}

bool MessagePart::isRoot() const
{
    return mRoot;
}

HTMLBlock::Ptr MessagePart::rootBlock() const
{
    if (mOtp->htmlWriter() && isRoot()) {
        return HTMLBlock::Ptr(new RootBlock(mOtp->htmlWriter()));
    }
    return HTMLBlock::Ptr();
}

QString MessagePart::text() const
{
    return mText;
}

void MessagePart::setText(const QString &text)
{
    mText = text;
}

CSSHelperBase *MessagePart::cssHelper() const
{
    Q_ASSERT(mOtp);
    return mOtp->cssHelper();
}

Interface::ObjectTreeSource *MessagePart::source() const
{
    Q_ASSERT(mOtp);
    return mOtp->mSource;
}

static QString iconToDataUrl(const QString &iconPath)
{
    QFile f(iconPath);
    if (!f.open(QIODevice::ReadOnly)) {
        return QString();
    }

    const QByteArray ba = f.readAll();
    return QStringLiteral("data:image/png;base64,%1").arg(QLatin1String(ba.toBase64().constData()));
}

/** Check if the newline at position @p newLinePos in string @p s
    seems to separate two paragraphs (important for correct BiDi
    behavior, but is heuristic because paragraphs are not
    well-defined) */
// Guesstimate if the newline at newLinePos actually separates paragraphs in the text s
// We use several heuristics:
// 1. If newLinePos points after or before (=at the very beginning of) text, it is not between paragraphs
// 2. If the previous line was longer than the wrap size, we want to consider it a paragraph on its own
//    (some clients, notably Outlook, send each para as a line in the plain-text version).
// 3. Otherwise, we check if the newline could have been inserted for wrapping around; if this
//    was the case, then the previous line will be shorter than the wrap size (which we already
//    know because of item 2 above), but adding the first word from the next line will make it
//    longer than the wrap size.
bool looksLikeParaBreak(const QString &s, unsigned int newLinePos)
{
    const unsigned int WRAP_COL = 78;

    unsigned int length = s.length();
    // 1. Is newLinePos at an end of the text?
    if (newLinePos >= length - 1 || newLinePos == 0) {
        return false;
    }

    // 2. Is the previous line really a paragraph -- longer than the wrap size?

    // First char of prev line -- works also for first line
    unsigned prevStart = s.lastIndexOf(QLatin1Char('\n'), newLinePos - 1) + 1;
    unsigned prevLineLength = newLinePos - prevStart;
    if (prevLineLength > WRAP_COL) {
        return true;
    }

    // find next line to delimit search for first word
    unsigned int nextStart = newLinePos + 1;
    int nextEnd = s.indexOf(QLatin1Char('\n'), nextStart);
    if (nextEnd == -1) {
        nextEnd = length;
    }
    QString nextLine = s.mid(nextStart, nextEnd - nextStart);
    length = nextLine.length();
    // search for first word in next line
    unsigned int wordStart;
    bool found = false;
    for (wordStart = 0; !found && wordStart < length; wordStart++) {
        switch (nextLine[wordStart].toLatin1()) {
        case '>':
        case '|':
        case ' ':  // spaces, tabs and quote markers don't count
        case '\t':
        case '\r':
            break;
        default:
            found = true;
            break;
        }
    } /* for() */

    if (!found) {
        // next line is essentially empty, it seems -- empty lines are
        // para separators
        return true;
    }
    //Find end of first word.
    //Note: flowText (in kmmessage.cpp) separates words for wrap by
    //spaces only. This should be consistent, which calls for some
    //refactoring.
    int wordEnd = nextLine.indexOf(QLatin1Char(' '), wordStart);
    if (wordEnd == (-1)) {
        wordEnd = length;
    }
    int wordLength = wordEnd - wordStart;

    // 3. If adding a space and the first word to the prev line don't
    //    make it reach the wrap column, then the break was probably
    //    meaningful
    return prevLineLength + wordLength + 1 < WRAP_COL;
}

QString MessagePart::quotedHTML(const QString &s, bool decorate)
{
    assert(cssHelper());

    KTextToHTML::Options convertFlags = KTextToHTML::PreserveSpaces | KTextToHTML::HighlightText;
    if (decorate && source()->showEmoticons()) {
        convertFlags |= KTextToHTML::ReplaceSmileys;
    }
    QString htmlStr;
    const QString normalStartTag = cssHelper()->nonQuotedFontTag();
    QString quoteFontTag[3];
    QString deepQuoteFontTag[3];
    for (int i = 0; i < 3; ++i) {
        quoteFontTag[i] = cssHelper()->quoteFontTag(i);
        deepQuoteFontTag[i] = cssHelper()->quoteFontTag(i + 3);
    }
    const QString normalEndTag = QStringLiteral("</div>");
    const QString quoteEnd = QStringLiteral("</div>");

    const unsigned int length = s.length();
    bool paraIsRTL = false;
    bool startNewPara = true;
    unsigned int pos, beg;

    // skip leading empty lines
    for (pos = 0; pos < length && s[pos] <= QLatin1Char(' '); ++pos)
        ;
    while (pos > 0 && (s[pos - 1] == QLatin1Char(' ') || s[pos - 1] == QLatin1Char('\t'))) {
        pos--;
    }
    beg = pos;

    int currQuoteLevel = -2; // -2 == no previous lines
    bool curHidden = false; // no hide any block

    if (source()->showExpandQuotesMark()) {
        // Cache Icons
        if (mCollapseIcon.isEmpty()) {
            mCollapseIcon = iconToDataUrl(IconNameCache::instance()->iconPath(QStringLiteral("quotecollapse"), 0));
        }
        if (mExpandIcon.isEmpty()) {
            mExpandIcon = iconToDataUrl(IconNameCache::instance()->iconPath(QStringLiteral("quoteexpand"), 0));
        }
    }

    int previousQuoteDepth = -1;
    while (beg < length) {
        /* search next occurrence of '\n' */
        pos = s.indexOf(QLatin1Char('\n'), beg, Qt::CaseInsensitive);
        if (pos == (unsigned int)(-1)) {
            pos = length;
        }

        QString line(s.mid(beg, pos - beg));
        beg = pos + 1;

        bool foundQuote = false;
        /* calculate line's current quoting depth */
        int actQuoteLevel = -1;
        const int numberOfCaracters(line.length());
        int quoteLength = 0;
        for (int p = 0; p < numberOfCaracters; ++p) {
            switch (line[p].toLatin1()) {
            case '>':
            case '|':
                actQuoteLevel++;
                quoteLength = p;
                foundQuote = true;
                break;
            case ' ':  // spaces and tabs are allowed between the quote markers
            case '\t':
            case '\r':
                quoteLength = p;
                break;
            default:  // stop quoting depth calculation
                p = numberOfCaracters;
                break;
            }
        } /* for() */
        if (!foundQuote) {
            quoteLength = 0;
        }
        bool actHidden = false;

        // This quoted line needs be hidden
        if (source()->showExpandQuotesMark() && source()->levelQuote() >= 0
                && source()->levelQuote() <= (actQuoteLevel)) {
            actHidden = true;
        }

        if (actQuoteLevel != currQuoteLevel) {
            /* finish last quotelevel */
            if (currQuoteLevel == -1) {
                htmlStr.append(normalEndTag);
            } else if (currQuoteLevel >= 0 && !curHidden) {
                htmlStr.append(quoteEnd);
            }
            //Close blockquote
            if (previousQuoteDepth > actQuoteLevel) {
                htmlStr += cssHelper()->addEndBlockQuote((previousQuoteDepth - actQuoteLevel));
            }

            /* start new quotelevel */
            if (actQuoteLevel == -1) {
                htmlStr += normalStartTag;
            } else {
                if (source()->showExpandQuotesMark()) {
                    if (actHidden) {
                        //only show the QuoteMark when is the first line of the level hidden
                        if (!curHidden) {
                            //Expand all quotes
                            htmlStr += QLatin1String("<div class=\"quotelevelmark\" >");
                            htmlStr += QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                      "<img src=\"%2\"/></a>")
                                       .arg(-1)
                                       .arg(mExpandIcon);
                            htmlStr += QLatin1String("</div><br/>");
                            htmlStr += quoteEnd;
                        }
                    } else {
                        htmlStr += QLatin1String("<div class=\"quotelevelmark\" >");
                        htmlStr += QStringLiteral("<a href=\"kmail:levelquote?%1 \">"
                                                  "<img src=\"%2\"/></a>")
                                   .arg(actQuoteLevel)
                                   .arg(mCollapseIcon);
                        htmlStr += QLatin1String("</div>");
                        if (actQuoteLevel < 3) {
                            htmlStr += quoteFontTag[actQuoteLevel];
                        } else {
                            htmlStr += deepQuoteFontTag[actQuoteLevel % 3];
                        }
                    }
                } else {
                    // Add blockquote
                    if (previousQuoteDepth < actQuoteLevel) {
                        htmlStr += cssHelper()->addStartBlockQuote(actQuoteLevel - previousQuoteDepth);
                    }

                    if (actQuoteLevel < 3) {
                        htmlStr += quoteFontTag[actQuoteLevel];
                    } else {
                        htmlStr += deepQuoteFontTag[actQuoteLevel % 3];
                    }
                }
            }
            currQuoteLevel = actQuoteLevel;
        }
        curHidden = actHidden;

        if (!actHidden) {
            // don't write empty <div ...></div> blocks (they have zero height)
            // ignore ^M DOS linebreaks
            if (!line.remove(QLatin1Char('\015')).isEmpty()) {
                if (startNewPara) {
                    paraIsRTL = line.isRightToLeft();
                }
                htmlStr += QStringLiteral("<div dir=\"%1\">").arg(paraIsRTL ? QStringLiteral("rtl") : QStringLiteral("ltr"));
                // if quoteLengh == 0 && foundQuote => a simple quote
                if (foundQuote) {
                    quoteLength++;
                    htmlStr += QStringLiteral("<span class=\"quotemarks\">%1</span>").arg(line.left(quoteLength));
                    const int rightString = (line.length()) - quoteLength;
                    if (rightString > 0) {
                        htmlStr += QStringLiteral("<font color=\"%1\">").arg(cssHelper()->quoteColorName(actQuoteLevel)) + KTextToHTML::convertToHtml(line.right(rightString), convertFlags) + QStringLiteral("</font>");
                    }
                } else {
                    htmlStr += KTextToHTML::convertToHtml(line, convertFlags);
                }

                htmlStr += QLatin1String("</div>");
                startNewPara = looksLikeParaBreak(s, pos);
            } else {
                htmlStr += QLatin1String("<br/>");
                // after an empty line, always start a new paragraph
                startNewPara = true;
            }
        }
        previousQuoteDepth = actQuoteLevel;
    } /* while() */

    /* really finish the last quotelevel */
    if (currQuoteLevel == -1) {
        htmlStr.append(normalEndTag);
    } else {
        htmlStr += quoteEnd + cssHelper()->addEndBlockQuote(currQuoteLevel + 1);
    }

    // qCDebug(MIMETREEPARSER_LOG) << "========================================\n"
    //                            << htmlStr
    //                            << "\n======================================\n";
    return htmlStr;
}

void MessagePart::html(bool decorate)
{
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());
    writer->queue(quotedHTML(text(), decorate));
}

void MessagePart::parseInternal(KMime::Content *node, bool onlyOneMimePart)
{
    mSubOtp = new ObjectTreeParser(mOtp, onlyOneMimePart);
    mSubOtp->setAllowAsync(mOtp->allowAsync());
    auto subMessagePart = mSubOtp->parseObjectTreeInternal(node);
    mInternalAttachmentNode = subMessagePart->attachmentNode();
    mIsInternalRoot = subMessagePart->isRoot();
    foreach (auto part, subMessagePart->subParts()) {
        appendSubPart(part);
    }
}

HTMLBlock::Ptr MessagePart::internalRootBlock() const
{
    if (mOtp->htmlWriter() && mIsInternalRoot) {
        return HTMLBlock::Ptr(new RootBlock(mOtp->htmlWriter()));
    }
    return HTMLBlock::Ptr();
}

HTMLBlock::Ptr MessagePart::internalAttachmentBlock() const
{
    if (mOtp->htmlWriter() && mInternalAttachmentNode) {
        return HTMLBlock::Ptr(new AttachmentMarkBlock(mOtp->htmlWriter(), mInternalAttachmentNode));
    }
    return HTMLBlock::Ptr();
}

void MessagePart::renderInternalHtml(bool decorate) const
{
    const HTMLBlock::Ptr rBlock(internalRootBlock());
    const HTMLBlock::Ptr aBlock(internalAttachmentBlock());

    foreach (const auto &mp, subParts()) {
        mp->html(decorate);
    }
}

QString MessagePart::renderInternalText() const
{
    QString text;
    foreach (const auto &mp, subParts()) {
        text += mp->text();
    }
    return text;
}

void MessagePart::copyContentFrom() const
{
    foreach (const auto &mp, subParts()) {
        const auto m = mp.dynamicCast<MessagePart>();
        if (m) {
            m->copyContentFrom();
        }
    }
    if (hasSubParts() && mSubOtp) {
        mOtp->copyContentFrom(mSubOtp);
    }
}

void MessagePart::fix() const
{
    foreach (const auto &mp, subParts()) {
        const auto m = mp.dynamicCast<MessagePart>();
        if (m) {
            m->fix();
        }
    }
}

void MessagePart::appendSubPart(const Interface::MessagePart::Ptr &messagePart)
{
    messagePart->setParentPart(this);
    mBlocks.append(messagePart);
}

const QVector<Interface::MessagePart::Ptr> &MessagePart::subParts() const
{
    return mBlocks;
}

bool MessagePart::hasSubParts() const
{
    return !mBlocks.isEmpty();
}

//-----MessagePartList----------------------
MessagePartList::MessagePartList(ObjectTreeParser *otp)
    : MessagePart(otp, QString())
{
}

MessagePartList::~MessagePartList()
{

}

void MessagePartList::html(bool decorate)
{
    const HTMLBlock::Ptr rBlock(rootBlock());
    const HTMLBlock::Ptr aBlock(attachmentBlock());

    renderInternalHtml(decorate);
}

QString MessagePartList::text() const
{
    return renderInternalText();
}

//-----TextMessageBlock----------------------

TextMessagePart::TextMessagePart(ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage, IconType asIcon)
    : MessagePartList(otp)
    , mNode(node)
    , mDrawFrame(drawFrame)
    , mShowLink(showLink)
    , mDecryptMessage(decryptMessage)
    , mAsIcon(asIcon)
{
    if (!mNode) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    parseContent();
}

TextMessagePart::~TextMessagePart()
{

}

bool TextMessagePart::decryptMessage() const
{
    return mDecryptMessage;
}

void TextMessagePart::parseContent()
{
    const auto aCodec = mOtp->codecFor(mNode);
    const QString &fromAddress = NodeHelper::fromAsString(mNode);
    mSignatureState  = KMMsgNotSigned;
    mEncryptionState = KMMsgNotEncrypted;
    const auto blocks = prepareMessageForDecryption(mNode->decodedContent());

    const auto cryptProto = Kleo::CryptoBackendFactory::instance()->openpgp();

    if (!blocks.isEmpty()) {

        /* The (overall) signature/encrypted status is broken
         * if one unencrypted part is at the beginning or in the middle
         * because mailmain adds an unencrypted part at the end this should not break the overall status
         *
         * That's why we first set the tmp status and if one crypted/signed block comes afterwards, than
         * the status is set to unencryped
         */
        bool fullySignedOrEncrypted = true;
        bool fullySignedOrEncryptedTmp = true;

        Q_FOREACH (const auto &block, blocks) {

            if (!fullySignedOrEncryptedTmp) {
                fullySignedOrEncrypted = false;
            }

            if (block.type() == NoPgpBlock && !block.text().trimmed().isEmpty()) {
                fullySignedOrEncryptedTmp = false;
                appendSubPart(MessagePart::Ptr(new MessagePart(mOtp, aCodec->toUnicode(block.text()))));
            } else if (block.type() == PgpMessageBlock) {
                CryptoMessagePart::Ptr mp(new CryptoMessagePart(mOtp, QString(), cryptProto, fromAddress, Q_NULLPTR));
                mp->setDecryptMessage(decryptMessage());
                mp->setIsEncrypted(true);
                appendSubPart(mp);
                if (!decryptMessage()) {
                    continue;
                }
                mp->startDecryption(block.text(), aCodec);
                if (mp->partMetaData()->inProgress) {
                    continue;
                }
            } else if (block.type() == ClearsignedBlock) {
                CryptoMessagePart::Ptr mp(new CryptoMessagePart(mOtp, QString(), cryptProto, fromAddress, Q_NULLPTR));
                appendSubPart(mp);
                mp->startVerification(block.text(), aCodec);
            } else {
                continue;
            }

            const auto mp = subParts().last().staticCast<MessagePart>();
            const PartMetaData *messagePart(mp->partMetaData());

            if (!messagePart->isEncrypted && !messagePart->isSigned && !block.text().trimmed().isEmpty()) {
                mp->setText(aCodec->toUnicode(block.text()));
            }

            if (messagePart->isEncrypted) {
                mEncryptionState = KMMsgPartiallyEncrypted;
            }

            if (messagePart->isSigned) {
                mSignatureState = KMMsgPartiallySigned;
            }
        }

        //Do we have an fully Signed/Encrypted Message?
        if (fullySignedOrEncrypted) {
            if (mSignatureState == KMMsgPartiallySigned) {
                mSignatureState = KMMsgFullySigned;
            }
            if (mEncryptionState == KMMsgPartiallyEncrypted) {
                mEncryptionState = KMMsgFullyEncrypted;
            }
        }
    }
}

void TextMessagePart::writePartIcon()
{
    HtmlWriter *writer = mOtp->htmlWriter();
    NodeHelper *nodeHelper = mOtp->nodeHelper();

    if (!writer || !mNode || mAsIcon == MimeTreeParser::NoIcon) {
        return;
    }

    const QString name = mNode->contentType()->name();
    QString label = name.isEmpty() ? NodeHelper::fileName(mNode) : name;
    if (label.isEmpty()) {
        label = i18nc("display name for an unnamed attachment", "Unnamed");
    }
    label = MessageCore::StringUtil::quoteHtmlChars(label, true);

    QString comment = mNode->contentDescription()->asUnicodeString();
    comment = MessageCore::StringUtil::quoteHtmlChars(comment, true);
    if (label == comment) {
        comment.clear();
    }

    QString href = nodeHelper->asHREF(mNode, QStringLiteral("body"));

    if (mAsIcon == MimeTreeParser::IconInline) {
        const QString fileName = nodeHelper->writeNodeToTempFile(mNode);
        // show the filename of the image below the embedded image
        writer->queue(QLatin1String("<hr/><div><a href=\"") + href + QLatin1String("\">"
                      "<img align=\"center\" src=\"") + QUrl::fromLocalFile(fileName).url() + QLatin1String("\" border=\"0\" style=\"max-width: 100%\"/></a>"
                              "</div>"
                              "<div><a href=\"") + href + QLatin1String("\">") + label + QLatin1String("</a>"
                                      "</div>"
                                      "<div>") + comment + QLatin1String("</div>"));
    } else {
        // show the filename next to the image
        const QString iconName = QUrl::fromLocalFile(nodeHelper->iconName(mNode)).url();
        if (iconName.right(14) == QLatin1String("mime_empty.png")) {
            nodeHelper->magicSetType(mNode);
            //iconName = nodeHelper->iconName( mNode );
        }

        const int iconSize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
        writer->queue(QStringLiteral("<hr/><div><a href=\"%1\">").arg(href) +
                      QStringLiteral("<img align=\"center\" height=\"%1\" width=\"%1\" src=\"%2\" border=\"0\" style=\"max-width: 100%\" alt=\"\"/>").arg(QString::number(iconSize), iconName) +
                      label + QStringLiteral("</a></div>") +
                      QStringLiteral("<div>%1</div>").arg(comment));
    }
}

void TextMessagePart::html(bool decorate)
{
    if (mOtp->nodeHelper()->isNodeDisplayedHidden(mNode)) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());
    HTMLBlock::Ptr block;
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (mDrawFrame) {
        block = HTMLBlock::Ptr(new TextBlock(writer, mOtp->nodeHelper(), mNode, mShowLink));
    }

    if (mAsIcon != MimeTreeParser::NoIcon) {
        writePartIcon();
    } else {
        renderInternalHtml(decorate);
    }
}

KMMsgEncryptionState TextMessagePart::encryptionState() const
{
    return mEncryptionState;
}

KMMsgSignatureState TextMessagePart::signatureState() const
{
    return mSignatureState;
}

//-----HtmlMessageBlock----------------------

HtmlMessagePart::HtmlMessagePart(ObjectTreeParser *otp, KMime::Content *node, Interface::ObjectTreeSource *source)
    : MessagePart(otp, QString())
    , mNode(node)
    , mSource(source)
{
    if (!mNode) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    const QByteArray partBody(mNode->decodedContent());
    mBodyHTML = mOtp->codecFor(mNode)->toUnicode(partBody);
    mCharset = NodeHelper::charset(mNode);
}

HtmlMessagePart::~HtmlMessagePart()
{
}

void HtmlMessagePart::fix() const
{
    mOtp->mHtmlContent += mBodyHTML;
    mOtp->mHtmlContentCharset = mCharset;
}

QString HtmlMessagePart::processHtml(const QString &htmlSource, QString &extraHead)
{
    // Create a DOM Document from the HTML source
    QWebPage page(0);
    page.settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    page.settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page.settings()->setAttribute(QWebSettings::PluginsEnabled, false);

    page.settings()->setAttribute(QWebSettings::AutoLoadImages, false);

    QWebFrame *frame = page.mainFrame();
    frame->setHtml(htmlSource);

    const QWebElement body = frame->documentElement().findFirst(QStringLiteral("body"));
    const QWebElement header = frame->documentElement().findFirst(QStringLiteral("head"));

    extraHead = header.toInnerXml();
    return body.toInnerXml();
}

void HtmlMessagePart::html(bool decorate)
{
    Q_UNUSED(decorate);
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();
    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());
    HTMLBlock::Ptr block;

    if (mSource->htmlMail()) {
        QString bodyText = mBodyHTML;
        QString extraHead;
        bodyText = processHtml(bodyText, extraHead);
        mOtp->mNodeHelper->setNodeDisplayedEmbedded(mNode, true);
        writer->extraHead(extraHead);

        // Show the "external references" warning (with possibility to load
        // external references only if loading external references is disabled
        // and the HTML code contains obvious external references). For
        // messages where the external references are obfuscated the user won't
        // have an easy way to load them but that shouldn't be a problem
        // because only spam contains obfuscated external references.
        if (!mSource->htmlLoadExternal() &&
                containsExternalReferences(bodyText, extraHead)) {
            block = HTMLBlock::Ptr(new HTMLWarnBlock(writer, i18n("<b>Note:</b> This HTML message may contain external "
                                   "references to images etc. For security/privacy reasons "
                                   "external references are not loaded. If you trust the "
                                   "sender of this message then you can load the external "
                                   "references for this message "
                                   "<a href=\"kmail:loadExternal\">by clicking here</a>.")));
        } else {
            block = HTMLBlock::Ptr(new HTMLWarnBlock(writer, QString()));
        }
        // Make sure the body is relative, so that nothing is painted over above "Note: ..."
        // if a malicious message uses absolute positioning. #137643
        writer->queue(bodyText);
    } else {
        block = HTMLBlock::Ptr(new HTMLWarnBlock(writer, i18n("<b>Note:</b> This is an HTML message. For "
                               "security reasons, only the raw HTML code "
                               "is shown. If you trust the sender of this "
                               "message then you can activate formatted "
                               "HTML display for this message "
                               "<a href=\"kmail:showHTML\">by clicking here</a>.")));
        // Make sure the body is relative, so that nothing is painted over above "Note: ..."
        // if a malicious message uses absolute positioning. #137643
        ConvertHtmlToPlainText convert;
        convert.setHtmlString(mBodyHTML);
        QString result = convert.generatePlainText();
        result.replace(QLatin1String("\n"), QStringLiteral("<br>"));
        writer->queue(result);
    }
    mSource->setHtmlMode(Util::Html);
}

QString HtmlMessagePart::text() const
{
    return mBodyHTML;
}

//-----MimeMessageBlock----------------------

MimeMessagePart::MimeMessagePart(ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart)
    : MessagePart(otp, QString())
    , mNode(node)
    , mOnlyOneMimePart(onlyOneMimePart)
{
    if (!mNode) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    parseInternal(mNode, mOnlyOneMimePart);
}

MimeMessagePart::~MimeMessagePart()
{

}

void MimeMessagePart::html(bool decorate)
{
    const HTMLBlock::Ptr aBlock(attachmentBlock());
    renderInternalHtml(decorate);
}

QString MimeMessagePart::text() const
{
    return renderInternalText();
}

//-----AlternativeMessagePart----------------------

AlternativeMessagePart::AlternativeMessagePart(ObjectTreeParser *otp, KMime::Content *textNode, KMime::Content *htmlNode)
    : MessagePart(otp, QString())
    , mTextNode(textNode)
    , mHTMLNode(htmlNode)
    , mViewHtml(false)
{
    if (!mTextNode && !mHTMLNode) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid nodes";
        return;
    }

    if (mTextNode) {
        mTextPart = MimeMessagePart::Ptr(new MimeMessagePart(mOtp, mTextNode, true));
    }

    if (mHTMLNode) {
        mHTMLPart = MimeMessagePart::Ptr(new MimeMessagePart(mOtp, mHTMLNode, true));
    }
}

AlternativeMessagePart::~AlternativeMessagePart()
{

}

void AlternativeMessagePart::setViewHtml(bool html)
{
    mViewHtml = html;
}

bool AlternativeMessagePart::viewHtml() const
{
    return mViewHtml;
}

void AlternativeMessagePart::html(bool decorate)
{
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    if (viewHtml() && mHTMLPart) {
        mHTMLPart->html(decorate);
    } else if (mTextNode) {
        mTextPart->html(decorate);
    }
}

QString AlternativeMessagePart::text() const
{
    if (mTextPart) {
        return mTextPart->text();
    }
    return QString();
}

void AlternativeMessagePart::fix() const
{
    if (mTextPart) {
        mTextPart->fix();
    }

    if (viewHtml() && mHTMLPart) {
        mHTMLPart->fix();
    }
}

void AlternativeMessagePart::copyContentFrom() const
{
    if (mTextPart) {
        mTextPart->copyContentFrom();
    }

    if (viewHtml() && mHTMLPart) {
        mHTMLPart->copyContentFrom();
    }
}

//-----CertMessageBlock----------------------

CertMessagePart::CertMessagePart(ObjectTreeParser *otp, KMime::Content *node, const Kleo::CryptoBackend::Protocol *cryptoProto, bool autoImport)
    : MessagePart(otp, QString())
    , mNode(node)
    , mAutoImport(autoImport)
    , mCryptoProto(cryptoProto)
{
    if (!mNode) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    if (!mAutoImport) {
        return;
    }

    const QByteArray certData = node->decodedContent();

    Kleo::ImportJob *import = mCryptoProto->importJob();
    KleoJobExecutor executor;
    mImportResult = executor.exec(import, certData);
}

CertMessagePart::~CertMessagePart()
{

}

void CertMessagePart::writeCertificateImportResult()
{
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();
    if (mImportResult.error()) {
        writer->queue(i18n("Sorry, certificate could not be imported.<br />"
                           "Reason: %1", QString::fromLocal8Bit(mImportResult.error().asString())));
        return;
    }

    const int nImp = mImportResult.numImported();
    const int nUnc = mImportResult.numUnchanged();
    const int nSKImp = mImportResult.numSecretKeysImported();
    const int nSKUnc = mImportResult.numSecretKeysUnchanged();
    if (!nImp && !nSKImp && !nUnc && !nSKUnc) {
        writer->queue(i18n("Sorry, no certificates were found in this message."));
        return;
    }
    QString comment = QLatin1String("<b>") + i18n("Certificate import status:") + QLatin1String("</b><br/>&nbsp;<br/>");
    if (nImp)
        comment += i18np("1 new certificate was imported.",
                         "%1 new certificates were imported.", nImp) + QLatin1String("<br/>");
    if (nUnc)
        comment += i18np("1 certificate was unchanged.",
                         "%1 certificates were unchanged.", nUnc) + QLatin1String("<br/>");
    if (nSKImp)
        comment += i18np("1 new secret key was imported.",
                         "%1 new secret keys were imported.", nSKImp) + QLatin1String("<br/>");
    if (nSKUnc)
        comment += i18np("1 secret key was unchanged.",
                         "%1 secret keys were unchanged.", nSKUnc) + QLatin1String("<br/>");
    comment += QLatin1String("&nbsp;<br/>");
    writer->queue(comment);
    if (!nImp && !nSKImp) {
        writer->queue(QStringLiteral("<hr/>"));
        return;
    }
    const std::vector<GpgME::Import> imports = mImportResult.imports();
    if (imports.empty()) {
        writer->queue(i18n("Sorry, no details on certificate import available.") + QLatin1String("<hr/>"));
        return;
    }
    writer->queue(QLatin1String("<b>") + i18n("Certificate import details:") + QLatin1String("</b><br/>"));
    std::vector<GpgME::Import>::const_iterator end(imports.end());
    for (std::vector<GpgME::Import>::const_iterator it = imports.begin(); it != end; ++it) {
        if ((*it).error()) {
            writer->queue(i18nc("Certificate import failed.", "Failed: %1 (%2)", QLatin1String((*it).fingerprint()),
                                QString::fromLocal8Bit((*it).error().asString())));
        } else if ((*it).status() & ~GpgME::Import::ContainedSecretKey) {
            if ((*it).status() & GpgME::Import::ContainedSecretKey) {
                writer->queue(i18n("New or changed: %1 (secret key available)", QLatin1String((*it).fingerprint())));
            } else {
                writer->queue(i18n("New or changed: %1", QLatin1String((*it).fingerprint())));
            }
        }
        writer->queue(QStringLiteral("<br/>"));
    }

    writer->queue(QStringLiteral("<hr/>"));
}

void CertMessagePart::html(bool decorate)
{
    Q_UNUSED(decorate);
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    writeCertificateImportResult();
}

QString CertMessagePart::text() const
{
    return QString();
}

//-----CryptMessageBlock---------------------
CryptoMessagePart::CryptoMessagePart(ObjectTreeParser *otp,
                                     const QString &text,
                                     const Kleo::CryptoBackend::Protocol *cryptoProto,
                                     const QString &fromAddress,
                                     KMime::Content *node)
    : MessagePart(otp, text)
    , mPassphraseError(false)
    , mNoSecKey(false)
    , mCryptoProto(cryptoProto)
    , mFromAddress(fromAddress)
    , mNode(node)
    , mDecryptMessage(false)
{
    mMetaData.technicalProblem = (mCryptoProto == 0);
    mMetaData.isSigned = false;
    mMetaData.isGoodSignature = false;
    mMetaData.isEncrypted = false;
    mMetaData.isDecryptable = false;
    mMetaData.keyTrust = GpgME::Signature::Unknown;
    mMetaData.status = i18n("Wrong Crypto Plug-In.");
    mMetaData.status_code = GPGME_SIG_STAT_NONE;
}

CryptoMessagePart::~CryptoMessagePart()
{

}

void CryptoMessagePart::setDecryptMessage(bool decrypt)
{
    mDecryptMessage = decrypt;
}

bool CryptoMessagePart::decryptMessage() const
{
    return mDecryptMessage;
}

void CryptoMessagePart::setIsEncrypted(bool encrypted)
{
    mMetaData.isEncrypted = encrypted;
}

bool CryptoMessagePart::isEncrypted() const
{
    return mMetaData.isEncrypted;
}

void CryptoMessagePart::setIsSigned(bool isSigned)
{
    mMetaData.isSigned = isSigned;
}

bool CryptoMessagePart::isSigned() const
{
    return mMetaData.isSigned;
}

bool CryptoMessagePart::passphraseError() const
{
    return mPassphraseError;
}

void CryptoMessagePart::startDecryption(const QByteArray &text, const QTextCodec *aCodec)
{
    KMime::Content *content = new KMime::Content;
    content->setBody(text);
    content->parse();

    startDecryption(content);

    if (!mMetaData.inProgress && mMetaData.isDecryptable) {
        setText(aCodec->toUnicode(mDecryptedData));
    }
}

bool CryptoMessagePart::okDecryptMIME(KMime::Content &data)
{
    mPassphraseError = false;
    mMetaData.inProgress = false;
    mMetaData.errorText.clear();
    mMetaData.auditLogError = GpgME::Error();
    mMetaData.auditLog.clear();
    bool bDecryptionOk = false;
    bool cannotDecrypt = false;
    Interface::ObjectTreeSource *source = mOtp->mSource;
    NodeHelper *nodeHelper = mOtp->nodeHelper();

    assert(decryptMessage());

    // Check whether the memento contains a result from last time:
    const DecryptVerifyBodyPartMemento *m
        = dynamic_cast<DecryptVerifyBodyPartMemento *>(nodeHelper->bodyPartMemento(&data, "decryptverify"));

    assert(!m || mCryptoProto); //No CryptoPlugin and having a bodyPartMemento -> there is something completly wrong

    if (!m && mCryptoProto) {
        Kleo::DecryptVerifyJob *job = mCryptoProto->decryptVerifyJob();
        if (!job) {
            cannotDecrypt = true;
        } else {
            const QByteArray ciphertext = data.decodedContent();
            DecryptVerifyBodyPartMemento *newM
                = new DecryptVerifyBodyPartMemento(job, ciphertext);
            if (mOtp->allowAsync()) {
                QObject::connect(newM, &CryptoBodyPartMemento::update,
                                 nodeHelper, &NodeHelper::update);
                QObject::connect(newM, SIGNAL(update(MimeTreeParser::UpdateMode)), source->sourceObject(),
                                 SLOT(update(MimeTreeParser::UpdateMode)));
                if (newM->start()) {
                    mMetaData.inProgress = true;
                    mOtp->mHasPendingAsyncJobs = true;
                } else {
                    m = newM;
                }
            } else {
                newM->exec();
                m = newM;
            }
            nodeHelper->setBodyPartMemento(&data, "decryptverify", newM);
        }
    } else if (m->isRunning()) {
        mMetaData.inProgress = true;
        mOtp->mHasPendingAsyncJobs = true;
        m = Q_NULLPTR;
    }

    if (m) {
        const QByteArray &plainText = m->plainText();
        const GpgME::DecryptionResult &decryptResult = m->decryptResult();
        const GpgME::VerificationResult &verifyResult = m->verifyResult();
        mMetaData.isSigned = verifyResult.signatures().size() > 0;
        mSignatures = verifyResult.signatures();
        mDecryptRecipients = decryptResult.recipients();
        bDecryptionOk = !decryptResult.error();
        mMetaData.auditLogError = m->auditLogError();
        mMetaData.auditLog = m->auditLogAsHtml();

//        std::stringstream ss;
//        ss << decryptResult << '\n' << verifyResult;
//        qCDebug(MIMETREEPARSER_LOG) << ss.str().c_str();

        if (!bDecryptionOk && mMetaData.isSigned) {
            //Only a signed part
            mMetaData.isEncrypted = false;
            bDecryptionOk = true;
            mDecryptedData = plainText;
        } else {
            mPassphraseError =  decryptResult.error().isCanceled() || decryptResult.error().code() == GPG_ERR_NO_SECKEY;
            mMetaData.isEncrypted = decryptResult.error().code() != GPG_ERR_NO_DATA;
            mMetaData.errorText = QString::fromLocal8Bit(decryptResult.error().asString());
            if (mMetaData.isEncrypted && decryptResult.numRecipients() > 0) {
                mMetaData.keyId = decryptResult.recipient(0).keyID();
            }

            if (bDecryptionOk) {
                mDecryptedData = plainText;
            } else {
                mNoSecKey = true;
                foreach (const GpgME::DecryptionResult::Recipient &recipient, decryptResult.recipients()) {
                    mNoSecKey &= (recipient.status().code() == GPG_ERR_NO_SECKEY);
                }
            }
        }
    }

    if (!bDecryptionOk) {
        QString cryptPlugLibName;
        if (mCryptoProto) {
            cryptPlugLibName = mCryptoProto->name();
        }

        if (!mCryptoProto) {
            mMetaData.errorText = i18n("No appropriate crypto plug-in was found.");
        } else if (cannotDecrypt) {
            mMetaData.errorText = i18n("Crypto plug-in \"%1\" cannot decrypt messages.",
                                       cryptPlugLibName);
        } else if (!passphraseError()) {
            mMetaData.errorText = i18n("Crypto plug-in \"%1\" could not decrypt the data.", cryptPlugLibName)
                                  + QLatin1String("<br />")
                                  + i18n("Error: %1", mMetaData.errorText);
        }
    }
    return bDecryptionOk;
}

void CryptoMessagePart::startDecryption(KMime::Content *data)
{
    if (!mNode && !data) {
        return;
    }

    if (!data) {
        data = mNode;
    }

    mMetaData.isEncrypted = true;

    bool bOkDecrypt = okDecryptMIME(*data);

    if (mMetaData.inProgress) {
        return;
    }
    mMetaData.isDecryptable = bOkDecrypt;

    if (!mMetaData.isDecryptable) {
        setText(QString::fromUtf8(mDecryptedData.constData()));
    }

    if (mMetaData.isSigned) {
        sigStatusToMetaData();
        mVerifiedText = mDecryptedData;
    }

    if (mMetaData.isEncrypted && !decryptMessage()) {
        mMetaData.isDecryptable = true;
    }

    if (mNode) {
        mOtp->mNodeHelper->setPartMetaData(mNode, mMetaData);

        if (decryptMessage()) {
            auto tempNode = new KMime::Content();
            tempNode->setContent(KMime::CRLFtoLF(mDecryptedData.constData()));
            tempNode->parse();

            if (!tempNode->head().isEmpty()) {
                tempNode->contentDescription()->from7BitString("encrypted data");
            }
            mOtp->mNodeHelper->attachExtraContent(mNode, tempNode);

            parseInternal(tempNode, false);
        }
    }
}

bool CryptoMessagePart::okVerify(const QByteArray &data, const QByteArray &signature)
{
    NodeHelper *nodeHelper = mOtp->nodeHelper();
    Interface::ObjectTreeSource *source = mOtp->mSource;

    mMetaData.isSigned = false;
    mMetaData.technicalProblem = (mCryptoProto == 0);
    mMetaData.keyTrust = GpgME::Signature::Unknown;
    mMetaData.status = i18n("Wrong Crypto Plug-In.");
    mMetaData.status_code = GPGME_SIG_STAT_NONE;

    const QByteArray mementoName = "verification";

    CryptoBodyPartMemento *m = dynamic_cast<CryptoBodyPartMemento *>(nodeHelper->bodyPartMemento(mNode, mementoName));
    assert(!m || mCryptoProto); //No CryptoPlugin and having a bodyPartMemento -> there is something completly wrong

    if (!m && mCryptoProto) {
        if (!signature.isEmpty()) {
            Kleo::VerifyDetachedJob *job = mCryptoProto->verifyDetachedJob();
            if (job) {
                m = new VerifyDetachedBodyPartMemento(job, mCryptoProto->keyListJob(), signature, data);
            }
        } else {
            Kleo::VerifyOpaqueJob *job = mCryptoProto->verifyOpaqueJob();
            if (job) {
                m = new VerifyOpaqueBodyPartMemento(job, mCryptoProto->keyListJob(), data);
            }
        }
        if (m) {
            if (mOtp->allowAsync()) {
                QObject::connect(m, &CryptoBodyPartMemento::update,
                                 nodeHelper, &NodeHelper::update);
                QObject::connect(m, SIGNAL(update(MimeTreeParser::UpdateMode)),
                                 source->sourceObject(), SLOT(update(MimeTreeParser::UpdateMode)));

                if (m->start()) {
                    mMetaData.inProgress = true;
                    mOtp->mHasPendingAsyncJobs = true;
                }
            } else {
                m->exec();
            }
            nodeHelper->setBodyPartMemento(mNode, mementoName, m);
        }
    } else if (m->isRunning()) {
        mMetaData.inProgress = true;
        mOtp->mHasPendingAsyncJobs = true;
    } else {
        mMetaData.inProgress = false;
        mOtp->mHasPendingAsyncJobs = false;
    }

    if (m && !mMetaData.inProgress) {
        if (!signature.isEmpty()) {
            VerifyDetachedBodyPartMemento *vm = dynamic_cast<VerifyDetachedBodyPartMemento *>(m);
            mVerifiedText = data;
            mSignatures = vm->verifyResult().signatures();
        } else {
            VerifyOpaqueBodyPartMemento *vm = dynamic_cast<VerifyOpaqueBodyPartMemento *>(m);
            mVerifiedText = vm->plainText();
            mSignatures = vm->verifyResult().signatures();
        }
        mMetaData.auditLogError = m->auditLogError();
        mMetaData.auditLog = m->auditLogAsHtml();
        mMetaData.isSigned = !mSignatures.empty();
    }

    if (!m && !mMetaData.inProgress) {
        QString errorMsg;
        QString cryptPlugLibName;
        QString cryptPlugDisplayName;
        if (mCryptoProto) {
            cryptPlugLibName = mCryptoProto->name();
            cryptPlugDisplayName = mCryptoProto->displayName();
        }

        if (!mCryptoProto) {
            if (cryptPlugDisplayName.isEmpty()) {
                errorMsg = i18n("No appropriate crypto plug-in was found.");
            } else {
                errorMsg = i18nc("%1 is either 'OpenPGP' or 'S/MIME'",
                                 "No %1 plug-in was found.",
                                 cryptPlugDisplayName);
            }
        } else {
            errorMsg = i18n("Crypto plug-in \"%1\" cannot verify mSignatures.",
                            cryptPlugLibName);
        }
        mMetaData.errorText = i18n("The message is signed, but the "
                                   "validity of the signature cannot be "
                                   "verified.<br />"
                                   "Reason: %1",
                                   errorMsg);
    }

    return mMetaData.isSigned;
}

static int signatureToStatus(const GpgME::Signature &sig)
{
    switch (sig.status().code()) {
    case GPG_ERR_NO_ERROR:
        return GPGME_SIG_STAT_GOOD;
    case GPG_ERR_BAD_SIGNATURE:
        return GPGME_SIG_STAT_BAD;
    case GPG_ERR_NO_PUBKEY:
        return GPGME_SIG_STAT_NOKEY;
    case GPG_ERR_NO_DATA:
        return GPGME_SIG_STAT_NOSIG;
    case GPG_ERR_SIG_EXPIRED:
        return GPGME_SIG_STAT_GOOD_EXP;
    case GPG_ERR_KEY_EXPIRED:
        return GPGME_SIG_STAT_GOOD_EXPKEY;
    default:
        return GPGME_SIG_STAT_ERROR;
    }
}

void CryptoMessagePart::sigStatusToMetaData()
{
    GpgME::Key key;
    if (mMetaData.isSigned) {
        GpgME::Signature signature = mSignatures.front();
        mMetaData.status_code = signatureToStatus(signature);
        mMetaData.isGoodSignature = mMetaData.status_code & GPGME_SIG_STAT_GOOD;
        // save extended signature status flags
        mMetaData.sigSummary = signature.summary();

        if (mMetaData.isGoodSignature && !key.keyID()) {
            // Search for the key by it's fingerprint so that we can check for
            // trust etc.
            Kleo::KeyListJob *job = mCryptoProto->keyListJob(false);    // local, no sigs
            if (!job) {
                qCDebug(MIMETREEPARSER_LOG) << "The Crypto backend does not support listing keys. ";
            } else {
                std::vector<GpgME::Key> found_keys;
                // As we are local it is ok to make this synchronous
                GpgME::KeyListResult res = job->exec(QStringList(QLatin1String(signature.fingerprint())), false, found_keys);
                if (res.error()) {
                    qCDebug(MIMETREEPARSER_LOG) << "Error while searching key for Fingerprint: " << signature.fingerprint();
                }
                if (found_keys.size() > 1) {
                    // Should not Happen
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found more then one Key for Fingerprint: " << signature.fingerprint();
                }
                if (found_keys.size() != 1) {
                    // Should not Happen at this point
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found no Key for Fingerprint: " << signature.fingerprint();
                } else {
                    key = found_keys[0];
                }
            }
        }

        if (key.keyID()) {
            mMetaData.keyId = key.keyID();
        }
        if (mMetaData.keyId.isEmpty()) {
            mMetaData.keyId = signature.fingerprint();
        }
        mMetaData.keyTrust = signature.validity();
        if (key.numUserIDs() > 0 && key.userID(0).id()) {
            mMetaData.signer = Kleo::DN(key.userID(0).id()).prettyDN();
        }
        for (uint iMail = 0; iMail < key.numUserIDs(); ++iMail) {
            // The following if /should/ always result in TRUE but we
            // won't trust implicitely the plugin that gave us these data.
            if (key.userID(iMail).email()) {
                QString email = QString::fromUtf8(key.userID(iMail).email());
                // ### work around gpgme 0.3.x / cryptplug bug where the
                // ### email addresses are specified as angle-addr, not addr-spec:
                if (email.startsWith(QLatin1Char('<')) && email.endsWith(QLatin1Char('>'))) {
                    email = email.mid(1, email.length() - 2);
                }
                if (!email.isEmpty()) {
                    mMetaData.signerMailAddresses.append(email);
                }
            }
        }

        if (signature.creationTime()) {
            mMetaData.creationTime.setTime_t(signature.creationTime());
        } else {
            mMetaData.creationTime = QDateTime();
        }
        if (mMetaData.signer.isEmpty()) {
            if (key.numUserIDs() > 0 && key.userID(0).name()) {
                mMetaData.signer = Kleo::DN(key.userID(0).name()).prettyDN();
            }
            if (!mMetaData.signerMailAddresses.empty()) {
                if (mMetaData.signer.isEmpty()) {
                    mMetaData.signer = mMetaData.signerMailAddresses.front();
                } else {
                    mMetaData.signer += QLatin1String(" <") + mMetaData.signerMailAddresses.front() + QLatin1Char('>');
                }
            }
        }
    }
}

void CryptoMessagePart::startVerification(const QByteArray &text, const QTextCodec *aCodec)
{
    startVerificationDetached(text, Q_NULLPTR, QByteArray());

    if (!mNode && mMetaData.isSigned) {
        setText(aCodec->toUnicode(mVerifiedText));
    }
}

void CryptoMessagePart::startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature)
{
    mMetaData.isEncrypted = false;
    mMetaData.isDecryptable = false;

    okVerify(text, signature);

    if (mMetaData.isSigned) {
        sigStatusToMetaData();
    } else {
        mMetaData.creationTime = QDateTime();
    }

    if (mNode) {
        if (textNode && !signature.isEmpty()) {
            mVerifiedText = text;
        } else if (!mVerifiedText.isEmpty()) {
            textNode = new KMime::Content();
            textNode->setContent(KMime::CRLFtoLF(mVerifiedText.constData()));
            textNode->parse();

            if (!textNode->head().isEmpty()) {
                textNode->contentDescription()->from7BitString("opaque signed data");
            }
            mOtp->mNodeHelper->attachExtraContent(mNode, textNode);
        }

        if (!mVerifiedText.isEmpty() && textNode) {
            parseInternal(textNode, false);
        }
    }

}

void CryptoMessagePart::writeDeferredDecryptionBlock() const
{
    Q_ASSERT(mMetaData.isEncrypted);
    Q_ASSERT(!decryptMessage());

    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();
    if (!writer) {
        return;
    }

    const QString iconName = QUrl::fromLocalFile(IconNameCache::instance()->iconPath(QStringLiteral("document-decrypt"), KIconLoader::Small)).url();
    writer->queue(QLatin1String("<div style=\"font-size:large; text-align:center;"
                                "padding-top:20pt;\">")
                  + i18n("This message is encrypted.")
                  + QLatin1String("</div>"
                                  "<div style=\"text-align:center; padding-bottom:20pt;\">"
                                  "<a href=\"kmail:decryptMessage\">"
                                  "<img src=\"") + iconName + QLatin1String("\"/>")
                  + i18n("Decrypt Message")
                  + QLatin1String("</a></div>"));
}

void CryptoMessagePart::html(bool decorate)
{
    bool hideErrors = false;
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    if (mMetaData.isEncrypted && !decryptMessage()) {
        const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
        writeDeferredDecryptionBlock();
    } else if (mMetaData.inProgress) {
        const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
        // In progress has no special body
    } else if (mMetaData.isEncrypted && !mMetaData.isDecryptable) {
        const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
        const QString errorMsg = i18n("Could not decrypt the data.");
        const QString sNoSecKeyHeader = i18n("No secret key found to encrypt the message. It is encrypted for following keys:");
        QString secKeyList;

        if (mNoSecKey) {
            foreach (const GpgME::DecryptionResult::Recipient &recipient, mDecryptRecipients) {
                if (!secKeyList.isEmpty()) {
                    secKeyList += QStringLiteral("<br />");
                }

                secKeyList += QStringLiteral("<a href=\"kmail:showCertificate#%1 ### %2 ### %3\">%4</a>")
                              .arg(mCryptoProto->displayName(),
                                   mCryptoProto->name(),
                                   QString::fromLatin1(recipient.keyID()),
                                   QString::fromLatin1(QByteArray("0x") + recipient.keyID())
                                  );
            }
        }

        writer->queue(QStringLiteral("<div style=\"font-size:x-large; text-align:center; padding:20pt;\">"));
        if (mNoSecKey) {
            writer->queue(sNoSecKeyHeader + QStringLiteral("<br />") + secKeyList);
        } else {
            writer->queue(errorMsg);
        }
        writer->queue(QStringLiteral("</div>"));
    } else {
        if (mMetaData.isSigned && mVerifiedText.isEmpty() && !hideErrors) {
            const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
            writer->queue(QStringLiteral("<hr/><b><h2>"));
            writer->queue(i18n("The crypto engine returned no cleartext data."));
            writer->queue(QStringLiteral("</h2></b>"));
            writer->queue(QStringLiteral("<br/>&nbsp;<br/>"));
            writer->queue(i18n("Status: "));
            if (!mMetaData.status.isEmpty()) {
                writer->queue(QStringLiteral("<i>"));
                writer->queue(mMetaData.status);
                writer->queue(QStringLiteral("</i>"));
            } else {
                writer->queue(i18nc("Status of message unknown.", "(unknown)"));
            }
        } else if (mNode) {
            const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
            renderInternalHtml(decorate);
        } else {
            const CryptoBlock block(mOtp->htmlWriter(), &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress);
            MessagePart::html(decorate);
        }
    }
}

EncapsulatedRfc822MessagePart::EncapsulatedRfc822MessagePart(ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message)
    : MessagePart(otp, QString())
    , mMessage(message)
    , mNode(node)
{
    mMetaData.isEncrypted = false;
    mMetaData.isSigned = false;
    mMetaData.isEncapsulatedRfc822Message = true;

    mOtp->nodeHelper()->setNodeDisplayedEmbedded(mNode, true);
    mOtp->nodeHelper()->setPartMetaData(mNode, mMetaData);

    if (!mMessage) {
        qCWarning(MIMETREEPARSER_LOG) << "Node is of type message/rfc822 but doesn't have a message!";
        return;
    }

    // The link to "Encapsulated message" is clickable, therefore the temp file needs to exists,
    // since the user can click the link and expect to have normal attachment operations there.
    mOtp->nodeHelper()->writeNodeToTempFile(message.data());

    parseInternal(message.data(), false);
}

EncapsulatedRfc822MessagePart::~EncapsulatedRfc822MessagePart()
{

}

void EncapsulatedRfc822MessagePart::html(bool decorate)
{
    Q_UNUSED(decorate)
    if (!hasSubParts()) {
        return;
    }

    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());
    const EncapsulatedRFC822Block block(mOtp->htmlWriter(), mOtp->nodeHelper(), mMessage.data());
    writer->queue(mOtp->mSource->createMessageHeader(mMessage.data()));
    renderInternalHtml(decorate);

    mOtp->nodeHelper()->setPartMetaData(mNode, mMetaData);
}

QString EncapsulatedRfc822MessagePart::text() const
{
    return renderInternalText();
}

void EncapsulatedRfc822MessagePart::copyContentFrom() const
{
}

void EncapsulatedRfc822MessagePart::fix() const
{
}
