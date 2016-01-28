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
#include "messageviewer_debug.h"
#include "objecttreeparser.h"
#include "converthtmltoplaintext.h"
#include "csshelper.h"
#include "cryptohelper.h"

#include <MessageCore/StringUtil>

#include <libkleo/importjob.h>
#include <libkleo/cryptobackendfactory.h>

#include <interfaces/htmlwriter.h>
#include <job/kleojobexecutor.h>
#include <settings/messageviewersettings.h>
#include <kmime/kmime_content.h>
#include <gpgme++/key.h>
#include <gpgme.h>

#include <QTextCodec>
#include <QApplication>
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>

#include <KLocalizedString>

using namespace MessageViewer;

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

//--------CryptoBlock-------------------
CryptoBlock::CryptoBlock(ObjectTreeParser *otp,
                         PartMetaData *block,
                         const Kleo::CryptoBackend::Protocol *cryptoProto,
                         const QString &fromAddress,
                         KMime::Content *node)
    : HTMLBlock()
    , mOtp(otp)
    , mMetaData(block)
    , mCryptoProto(cryptoProto)
    , mFromAddress(fromAddress)
    , mNode(node)
{
    internalEnter();
}

CryptoBlock::~CryptoBlock()
{
    internalExit();
}

void CryptoBlock::internalEnter()
{
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();
    if (writer && !entered) {
        entered = true;
        if (mMetaData->isEncapsulatedRfc822Message) {
            mInteralBlocks.append(HTMLBlock::Ptr(new EncapsulatedRFC822Block(writer, mOtp->nodeHelper(), mNode)));
        }
        if (mMetaData->isEncrypted) {
            mInteralBlocks.append(HTMLBlock::Ptr(new EncryptedBlock(writer, *mMetaData)));
        }
        writer->queue(mOtp->writeSigstatHeader(*mMetaData, mCryptoProto, mFromAddress, mNode));
    }
}

void CryptoBlock::internalExit()
{
    if (!entered) {
        return;
    }
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();
    writer->queue(mOtp->writeSigstatFooter(*mMetaData));
    while (!mInteralBlocks.isEmpty()) {
        mInteralBlocks.removeLast();
    }
    entered = false;
}

EncapsulatedRFC822Block::EncapsulatedRFC822Block(MessageViewer::HtmlWriter *writer, MessageViewer::NodeHelper *nodeHelper, KMime::Content *node)
    : mWriter(writer)
    , mNodeHelper(nodeHelper)
    , mNode(node)
{
    internalEnter();
}

EncapsulatedRFC822Block::~EncapsulatedRFC822Block()
{
    internalExit();
}

void EncapsulatedRFC822Block::internalEnter()
{
    if (mWriter && !entered) {
        const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
        const QString cellPadding(QStringLiteral("cellpadding=\"1\""));
        const QString cellSpacing(QStringLiteral("cellspacing=\"1\""));
        mWriter->queue(QLatin1String("<table ") + cellSpacing + QLatin1Char(' ') + cellPadding + QLatin1String(" class=\"rfc822\">"
                       "<tr class=\"rfc822H\"><td dir=\"") + dir + QLatin1String("\">"));
        if (mNode) {
            const QString href = mNodeHelper->asHREF(mNode, QStringLiteral("body"));
            mWriter->queue(QLatin1String("<a href=\"") + href + QLatin1String("\">")
                           + i18n("Encapsulated message") + QLatin1String("</a>"));
        } else {
            mWriter->queue(i18n("Encapsulated message"));
        }
        mWriter->queue(QLatin1String("</td></tr><tr class=\"rfc822B\"><td>"));

        entered = true;
    }
}

void EncapsulatedRFC822Block::internalExit()
{
    if (!entered) {
        return;
    }

    const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
    mWriter->queue(QLatin1String("</td></tr><tr class=\"rfc822H\"><td dir=\"") + dir + QLatin1String("\">") +
                   i18n("End of encapsulated message") + QLatin1String("</td></tr></table>"));
    entered = false;
}

EncryptedBlock::EncryptedBlock(MessageViewer::HtmlWriter *writer, const PartMetaData &block)
    : mWriter(writer)
    , mBlock(block)
{
    internalEnter();
}

EncryptedBlock::~EncryptedBlock()
{
    internalExit();
}

void EncryptedBlock::internalEnter()
{
    if (mWriter && !entered) {
        entered = true;
        const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
        const QString cellPadding(QStringLiteral("cellpadding=\"1\""));
        const QString cellSpacing(QStringLiteral("cellspacing=\"1\""));

        mWriter->queue(QLatin1String("<table ") + cellSpacing + QLatin1Char(' ') + cellPadding + QLatin1String(" class=\"encr\">"
                       "<tr class=\"encrH\"><td dir=\"") + dir + QLatin1String("\">"));
        if (mBlock.inProgress) {
            mWriter->queue(i18n("Please wait while the message is being decrypted..."));
        } else if (mBlock.isDecryptable) {
            mWriter->queue(i18n("Encrypted message"));
        } else {
            mWriter->queue(i18n("Encrypted message (decryption not possible)"));
            if (!mBlock.errorText.isEmpty()) {
                mWriter->queue(QLatin1String("<br />") + i18n("Reason: %1", mBlock.errorText));
            }
        }
        mWriter->queue(QLatin1String("</td></tr><tr class=\"encrB\"><td>"));
    }
}

void EncryptedBlock::internalExit()
{
    if (!entered) {
        return;
    }
    const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
    mWriter->queue(QLatin1String("</td></tr><tr class=\"encrH\"><td dir=\"") + dir + QLatin1String("\">") +
                   i18n("End of encrypted message") +
                   QLatin1String("</td></tr></table>"));
    entered = false;
}

AttachmentMarkBlock::AttachmentMarkBlock(MessageViewer::HtmlWriter *writer, KMime::Content *node)
    : mNode(node)
    , mWriter(writer)
{
    internalEnter();
}

AttachmentMarkBlock::~AttachmentMarkBlock()
{
    internalExit();
}

void AttachmentMarkBlock::internalEnter()
{
    if (mWriter && !entered) {
        const QString index = mNode->index().toString();
        mWriter->queue(QStringLiteral("<a name=\"att%1\"></a>").arg(index));
        mWriter->queue(QStringLiteral("<div id=\"attachmentDiv%1\">\n").arg(index));
        entered = true;
    }
}

void AttachmentMarkBlock::internalExit()
{
    if (!entered) {
        return;
    }

    mWriter->queue(QStringLiteral("</div>"));
    entered = false;
}

TextBlock::TextBlock(MessageViewer::HtmlWriter *writer, MessageViewer::NodeHelper *nodeHelper, KMime::Content *node, bool link)
    : mWriter(writer)
    , mNodeHelper(nodeHelper)
    , mNode(node)
    , mLink(link)
{
    internalEnter();
}

TextBlock::~TextBlock()
{
    internalExit();
}

void TextBlock::internalEnter()
{
    if (!mWriter || entered) {
        return;
    }
    entered = true;

    const QString label = MessageCore::StringUtil::quoteHtmlChars(NodeHelper::fileName(mNode), true);

    const QString comment =
        MessageCore::StringUtil::quoteHtmlChars(mNode->contentDescription()->asUnicodeString(), true);

    const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");

    mWriter->queue(QLatin1String("<table cellspacing=\"1\" class=\"textAtm\">"
                                 "<tr class=\"textAtmH\"><td dir=\"") + dir + QLatin1String("\">"));
    if (!mLink)
        mWriter->queue(QLatin1String("<a href=\"") + mNodeHelper->asHREF(mNode, QStringLiteral("body")) + QLatin1String("\">")
                       + label + QLatin1String("</a>"));
    else {
        mWriter->queue(label);
    }
    if (!comment.isEmpty()) {
        mWriter->queue(QLatin1String("<br/>") + comment);
    }
    mWriter->queue(QLatin1String("</td></tr><tr class=\"textAtmB\"><td>"));
}

void TextBlock::internalExit()
{
    if (!entered) {
        return;
    }

    entered = false;

    mWriter->queue(QStringLiteral("</td></tr></table>"));
}

HTMLWarnBlock::HTMLWarnBlock(HtmlWriter *writer, const QString &msg)
    : mWriter(writer)
    , mMsg(msg)
{
    internalEnter();
}

HTMLWarnBlock::~HTMLWarnBlock()
{
    internalExit();
}

void HTMLWarnBlock::internalEnter()
{
    if (!mWriter || entered) {
        return;
    }
    entered = true;

    if (!mMsg.isEmpty()) {
        mWriter->queue(QStringLiteral("<div class=\"htmlWarn\">\n"));
        mWriter->queue(mMsg);
        mWriter->queue(QStringLiteral("</div><br/><br/>"));
    }

    mWriter->queue(QStringLiteral("<div style=\"position: relative\">\n"));
}

void HTMLWarnBlock::internalExit()
{
    if (!entered) {
        return;
    }

    entered = false;

    mWriter->queue(QStringLiteral("</div>\n"));
}

RootBlock::RootBlock(HtmlWriter *writer)
    : HTMLBlock()
    , mWriter(writer)
{
    internalEnter();
}

RootBlock::~RootBlock()
{
    internalExit();
}

void RootBlock::internalEnter()
{
    if (!mWriter || entered) {
        return;
    }
    entered = true;

    mWriter->queue(QStringLiteral("<div style=\"position: relative; word-wrap: break-word\">\n"));
}

void RootBlock::internalExit()
{
    if (!entered) {
        return;
    }

    entered = false;

    mWriter->queue(QStringLiteral("</div>\n"));
}

//------MessagePart-----------------------
MessagePart::MessagePart(ObjectTreeParser *otp,
                         const QString &text)
    : mText(text)
    , mOtp(otp)
    , mSubOtp(Q_NULLPTR)
    , mAttachmentNode(Q_NULLPTR)
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

HTMLBlock::Ptr MessagePart::attachmentBlock() const
{
    if (mOtp->htmlWriter() && isAttachment()) {
        return HTMLBlock::Ptr(new AttachmentMarkBlock(mOtp->htmlWriter(), mAttachmentNode));
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

void MessagePart::html(bool decorate)
{
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    const CryptoBlock block(mOtp, &mMetaData, Q_NULLPTR, QString(), Q_NULLPTR);
    writer->queue(mOtp->quotedHTML(text(), decorate));
}

void MessagePart::parseInternal(KMime::Content *node, bool onlyOneMimePart)
{
    mSubOtp = new ObjectTreeParser(mOtp, onlyOneMimePart);
    mSubOtp->setAllowAsync(mOtp->allowAsync());
    mSubMessagePart = mSubOtp->parseObjectTreeInternal(node);
}

void MessagePart::renderInternalHtml(bool decorate) const
{
    if (mSubMessagePart) {
        mSubMessagePart->html(decorate);
    }
}

void MessagePart::copyContentFrom() const
{
    if (mSubMessagePart) {
        mSubMessagePart->copyContentFrom();
        if (mSubOtp) {
            mOtp->copyContentFrom(mSubOtp);
        }
    }
}

QString MessagePart::renderInternalText() const
{
    if (!mSubMessagePart) {
        return QString();
    }
    return mSubMessagePart->text();
}

void MessagePart::fix() const
{
    if (mSubMessagePart) {
        mSubMessagePart->fix();
    }
}

//-----MessagePartList----------------------
MessagePartList::MessagePartList(ObjectTreeParser *otp)
    : MessagePart(otp, QString())
    , mRoot(false)
{
}

MessagePartList::~MessagePartList()
{

}

void MessagePartList::setIsRoot(bool root)
{
    mRoot = root;
}

bool MessagePartList::isRoot() const
{
    return mRoot;
}

HTMLBlock::Ptr MessagePartList::rootBlock() const
{
    if (mOtp->htmlWriter() && isRoot()) {
        return HTMLBlock::Ptr(new RootBlock(mOtp->htmlWriter()));
    }
    return HTMLBlock::Ptr();
}

void MessagePartList::appendMessagePart(const Interface::MessagePart::Ptr &messagePart)
{
    mBlocks.append(messagePart);
}

const QVector<Interface::MessagePart::Ptr> &MessagePartList::messageParts() const
{
    return mBlocks;
}

void MessagePartList::html(bool decorate)
{
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    const HTMLBlock::Ptr rBlock(rootBlock());
    const HTMLBlock::Ptr aBlock(attachmentBlock());

    htmlInternal(decorate);
}

void MessagePartList::htmlInternal(bool decorate)
{
    foreach (const auto &mp, mBlocks) {
        mp->html(decorate);
    }
}

QString MessagePartList::text() const
{
    QString text;
    foreach (const auto &mp, mBlocks) {
        text += mp->text();
    }
    return text;
}

void MessagePartList::fix() const
{
    foreach (const auto &mp, mBlocks) {
        const auto m = mp.dynamicCast<MessagePart>();
        if (m) {
            m->fix();
        }
    }
}

void MessagePartList::copyContentFrom() const
{
    foreach (const auto &mp, mBlocks) {
        const auto m = mp.dynamicCast<MessagePart>();
        if (m) {
            m->copyContentFrom();
        }
    }
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
        qCWarning(MESSAGEVIEWER_LOG) << "not a valid node";
        return;
    }

    if (mAsIcon == MessageViewer::NoIcon) {
        parseContent();
    }
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

        if (blocks.count() > 1 || blocks.at(0).type() != MessageViewer::NoPgpBlock) {
            mOtp->setCryptoProtocol(cryptProto);
        }

        QString htmlStr;
        QString plainTextStr;

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
                appendMessagePart(MessagePart::Ptr(new MessagePart(mOtp, aCodec->toUnicode(block.text()))));
            } else if (block.type() == PgpMessageBlock) {
                CryptoMessagePart::Ptr mp(new CryptoMessagePart(mOtp, QString(), cryptProto, fromAddress, 0));
                mp->setDecryptMessage(decryptMessage());
                mp->setIsEncrypted(true);
                appendMessagePart(mp);
                if (!decryptMessage()) {
                    continue;
                }
                mp->startDecryption(block.text(), aCodec);
                if (mp->partMetaData()->inProgress) {
                    continue;
                }
            } else if (block.type() == ClearsignedBlock) {
                CryptoMessagePart::Ptr mp(new CryptoMessagePart(mOtp, QString(), cryptProto, fromAddress, 0));
                appendMessagePart(mp);
                mp->startVerification(block.text(), aCodec);
            } else {
                continue;
            }

            const auto mp = messageParts().last().staticCast<MessagePart>();
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

void TextMessagePart::html(bool decorate)
{
    const HTMLBlock::Ptr aBlock(attachmentBlock());
    HTMLBlock::Ptr block;
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    if (mDrawFrame) {
        block = HTMLBlock::Ptr(new TextBlock(writer, mOtp->nodeHelper(), mNode, mShowLink));
    }

    if (mAsIcon != MessageViewer::NoIcon) {
        mOtp->writePartIcon(mNode, (mAsIcon == MessageViewer::IconInline));
    } else {
        MessagePartList::htmlInternal(decorate);
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

HtmlMessagePart::HtmlMessagePart(ObjectTreeParser *otp, KMime::Content *node, ObjectTreeSourceIf *source)
    : MessagePart(otp, QString())
    , mNode(node)
    , mSource(source)
{
    if (!mNode) {
        qCWarning(MESSAGEVIEWER_LOG) << "not a valid node";
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
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();
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
        qCWarning(MESSAGEVIEWER_LOG) << "not a valid node";
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
        qCWarning(MESSAGEVIEWER_LOG) << "not a valid nodes";
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
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

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
    , mAutoImport(autoImport)
    , mCryptoProto(cryptoProto)
{
    if (!mNode) {
        qCWarning(MESSAGEVIEWER_LOG) << "not a valid node";
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

void CertMessagePart::html(bool decorate)
{
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    mOtp->writeCertificateImportResult(mImportResult);
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

void CryptoMessagePart::startDecryption(KMime::Content *data)
{
    if (!mNode && !data) {
        return;
    }

    if (!data) {
        data = mNode;
    }

    bool signatureFound;
    bool actuallyEncrypted = true;
    bool decryptionStarted;

    bool bOkDecrypt = mOtp->okDecryptMIME(*data,
                                          mDecryptedData,
                                          signatureFound,
                                          mSignatures,
                                          true,
                                          mPassphraseError,
                                          actuallyEncrypted,
                                          decryptionStarted,
                                          mMetaData);
    if (decryptionStarted) {
        mMetaData.inProgress = true;
        return;
    }
    mMetaData.isDecryptable = bOkDecrypt;
    mMetaData.isEncrypted = actuallyEncrypted;
    mMetaData.isSigned = signatureFound;

    if (!mMetaData.isDecryptable) {
        setText(QString::fromUtf8(mDecryptedData.constData()));
    }

    if (mMetaData.isSigned) {
        mOtp->sigStatusToMetaData(mSignatures, mCryptoProto, mMetaData, GpgME::Key());
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

void CryptoMessagePart::startVerification(const QByteArray &text, const QTextCodec *aCodec)
{
    startVerificationDetached(text, 0, QByteArray());

    if (!mNode && mMetaData.isSigned) {
        setText(aCodec->toUnicode(mVerifiedText));
    }
}

void CryptoMessagePart::startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature)
{
    mMetaData.isEncrypted = false;
    mMetaData.isDecryptable = false;

    mOtp->okVerify(text, mCryptoProto, mMetaData, mVerifiedText, mSignatures, signature, mNode);

    if (mMetaData.isSigned) {
        mOtp->sigStatusToMetaData(mSignatures, mCryptoProto, mMetaData, GpgME::Key());
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

    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();
    if (!writer) {
        return;
    }

    const QString iconName = QLatin1String("file:///") + KIconLoader::global()->iconPath(QStringLiteral("document-decrypt"),
                             KIconLoader::Small);
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
    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    if (mMetaData.isEncrypted && !decryptMessage()) {
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mFromAddress, mNode);
        writeDeferredDecryptionBlock();
    } else if (mMetaData.inProgress) {
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mFromAddress, mNode);
        // In progress has no special body
    } else if (mMetaData.isEncrypted && !mMetaData.isDecryptable) {
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mFromAddress, mNode);
        writer->queue(text());           //Do not quote ErrorText
    } else {
        if (mMetaData.isSigned && mVerifiedText.isEmpty() && !hideErrors) {
            const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mFromAddress, mNode);
            writer->queue(QStringLiteral("<hr><b><h2>"));
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
            const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mFromAddress, mNode);
            renderInternalHtml(decorate);
        } else {
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
        qCWarning(MESSAGEVIEWER_LOG) << "Node is of type message/rfc822 but doesn't have a message!";
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
    if (!mSubMessagePart) {
        return;
    }

    MessageViewer::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    const CryptoBlock block(mOtp, &mMetaData, Q_NULLPTR, mMessage->from()->asUnicodeString(), mMessage.data());
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
