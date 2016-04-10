/*
    objecttreeparser.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003      Marc Mutz <mutz@kde.org>
    Copyright (C) 2002-2004 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>
    Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

// MessageViewer includes

#include "objecttreeparser.h"

#include "attachmentstrategy.h"
#include "bodypartformatterbasefactory.h"
#include "converthtmltoplaintext.h"
#include "csshelperbase.h"
#include "nodehelper.h"
#include "messagepart.h"
#include "partmetadata.h"
#include "partnodebodypart.h"

#include "mimetreeparser_debug.h"

#include "interfaces/bodypartformatter.h"
#include "interfaces/htmlwriter.h"
#include "utils/iconnamecache.h"
#include "utils/mimetype.h"

#include <KMime/Headers>
#include <KMime/Message>

// KDE includes
#include <KLocalizedString>
#include <KTextToHTML>

// Qt includes
#include <QByteArray>
#include <QFile>
#include <QTextCodec>
#include <QUrl>

// other includes
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>

using namespace MimeTreeParser;

ObjectTreeParser::ObjectTreeParser(const ObjectTreeParser *topLevelParser,
                                   bool showOnlyOneMimePart,
                                   const AttachmentStrategy *strategy)
    : mSource(topLevelParser->mSource),
      mNodeHelper(topLevelParser->mNodeHelper),
      mHtmlWriter(topLevelParser->mHtmlWriter),
      mTopLevelContent(topLevelParser->mTopLevelContent),
      mShowOnlyOneMimePart(showOnlyOneMimePart),
      mHasPendingAsyncJobs(false),
      mAllowAsync(topLevelParser->mAllowAsync),
      mAttachmentStrategy(strategy),
      mPrinting(false)
{
    init();
}

ObjectTreeParser::ObjectTreeParser(ObjectTreeSourceIf *source,
                                   MimeTreeParser::NodeHelper *nodeHelper,
                                   bool showOnlyOneMimePart,
                                   const AttachmentStrategy *strategy)
    : mSource(source),
      mNodeHelper(nodeHelper),
      mHtmlWriter(0),
      mTopLevelContent(0),
      mShowOnlyOneMimePart(showOnlyOneMimePart),
      mHasPendingAsyncJobs(false),
      mAllowAsync(false),
      mAttachmentStrategy(strategy),
      mPrinting(false)
{
    init();
}

void ObjectTreeParser::init()
{
    assert(mSource);
    if (!attachmentStrategy()) {
        mAttachmentStrategy = mSource->attachmentStrategy();
    }

    if (!mNodeHelper) {
        mNodeHelper = new NodeHelper();
        mDeleteNodeHelper = true;
    } else {
        mDeleteNodeHelper = false;
    }
}

ObjectTreeParser::ObjectTreeParser(const ObjectTreeParser &other)
    : mSource(other.mSource),
      mNodeHelper(other.nodeHelper()),   //TODO(Andras) hm, review what happens if mDeleteNodeHelper was true in the source
      mHtmlWriter(other.mHtmlWriter),
      mTopLevelContent(other.mTopLevelContent),
      mShowOnlyOneMimePart(other.showOnlyOneMimePart()),
      mHasPendingAsyncJobs(other.hasPendingAsyncJobs()),
      mAllowAsync(other.allowAsync()),
      mAttachmentStrategy(other.attachmentStrategy()),
      mDeleteNodeHelper(false),   // TODO see above
      mPrinting(other.printing())
{

}

ObjectTreeParser::~ObjectTreeParser()
{
    if (mDeleteNodeHelper) {
        delete mNodeHelper;
        mNodeHelper = 0;
    }
}

void ObjectTreeParser::setAllowAsync(bool allow)
{
    assert(!mHasPendingAsyncJobs);
    mAllowAsync = allow;
}

bool ObjectTreeParser::allowAsync() const
{
    return mAllowAsync;
}

bool ObjectTreeParser::hasPendingAsyncJobs() const
{
    return mHasPendingAsyncJobs;
}

QString ObjectTreeParser::plainTextContent() const
{
    return mPlainTextContent;
}

QString ObjectTreeParser::htmlContent() const
{
    return mHtmlContent;
}

void ObjectTreeParser::copyContentFrom(const ObjectTreeParser *other)
{
    mPlainTextContent += other->plainTextContent();
    mHtmlContent += other->htmlContent();
    if (!other->plainTextContentCharset().isEmpty()) {
        mPlainTextContentCharset = other->plainTextContentCharset();
    }
    if (!other->htmlContentCharset().isEmpty()) {
        mHtmlContentCharset = other->htmlContentCharset();
    }
}

bool ObjectTreeParser::printing() const
{
    return mPrinting;
}

//-----------------------------------------------------------------------------

void ObjectTreeParser::parseObjectTree(KMime::Content *node)
{
    mTopLevelContent = node;
    const auto mp = parseObjectTreeInternal(node);

    if (mp) {
        mp->fix();
        mp->copyContentFrom();
        mp->html(false);
    }
}

void ObjectTreeParser::setPrinting(bool printing)
{
    mPrinting = printing;
}

bool ObjectTreeParser::processType(KMime::Content *node, ProcessResult &processResult, const QByteArray &mediaType, const QByteArray &subType, Interface::MessagePartPtr &mpRet)
{
    bool bRendered = false;
    const auto sub = mSource->bodyPartFormatterFactory()->subtypeRegistry(mediaType);
    auto range =  sub.equal_range(subType);
    for (auto it = range.first; it != range.second; ++it) {
        const auto formatter = (*it).second;
        if (!formatter) {
            continue;
        }
        PartNodeBodyPart part(this, &processResult, mTopLevelContent, node, mNodeHelper, codecFor(node));
        // Set the default display strategy for this body part relying on the
        // identity of Interface::BodyPart::Display and AttachmentStrategy::Display
        part.setDefaultDisplay((Interface::BodyPart::Display) attachmentStrategy()->defaultDisplay(node));

        mNodeHelper->setNodeDisplayedEmbedded(node, true);

        const Interface::MessagePart::Ptr result = formatter->process(part);
        if (!result) {
            continue;
        }

        if (const auto mp = result.dynamicCast<MessagePart>()) {
            mp->setAttachmentFlag(node);
            mpRet = result;
            bRendered = true;
            break;
        } else if (dynamic_cast<MimeTreeParser::Interface::MessagePart *>(result.data())) {
            QObject *asyncResultObserver = allowAsync() ? mSource->sourceObject() : 0;
            const auto r = formatter->format(&part, htmlWriter(), asyncResultObserver);
            if (r == Interface::BodyPartFormatter::AsIcon) {
                processResult.setNeverDisplayInline(true);
                formatter->adaptProcessResult(processResult);
                mNodeHelper->setNodeDisplayedEmbedded(node, false);
                const Interface::MessagePart::Ptr mp = defaultHandling(node, processResult);
                if (mp) {
                    if (auto _mp = mp.dynamicCast<MessagePart>()) {
                        _mp->setAttachmentFlag(node);
                    }
                    mpRet = mp;
                }
                bRendered = true;
                break;
            }
            continue;
        } else {
            continue;
        }
    }
    return bRendered;
}

MessagePart::Ptr ObjectTreeParser::parseObjectTreeInternal(KMime::Content *node)
{
    if (!node) {
        return MessagePart::Ptr();
    }

    // reset pending async jobs state (we'll rediscover pending jobs as we go)
    mHasPendingAsyncJobs = false;

    // reset "processed" flags for...
    if (showOnlyOneMimePart()) {
        // ... this node and all descendants
        mNodeHelper->setNodeUnprocessed(node, false);
        if (!node->contents().isEmpty()) {
            mNodeHelper->setNodeUnprocessed(node, true);
        }
    } else if (!node->parent()) {
        // ...this node and all it's siblings and descendants
        mNodeHelper->setNodeUnprocessed(node, true);
    }

    const bool isRoot = node->isTopLevel();
    MessagePartList::Ptr mpl(new MessagePartList(this));
    mpl->setIsRoot(isRoot);
    KMime::Content *parent = node->parent();
    auto contents = parent ? parent->contents() : KMime::Content::List();
    if (contents.isEmpty()) {
        contents.append(node);
    }
    int i = contents.indexOf(const_cast<KMime::Content *>(node));
    for (; i < contents.size(); ++i) {
        node = contents.at(i);
        if (mNodeHelper->nodeProcessed(node)) {
            continue;
        }

        ProcessResult processResult(mNodeHelper);

        QByteArray mediaType("text");
        QByteArray subType("plain");
        if (node->contentType(false) && !node->contentType()->mediaType().isEmpty() &&
                !node->contentType()->subType().isEmpty()) {
            mediaType = node->contentType()->mediaType();
            subType = node->contentType()->subType();
        }

        Interface::MessagePartPtr mp;
        if (processType(node, processResult, mediaType, subType, mp)) {
            if (mp) {
                mpl->appendMessagePart(mp);
            }
        } else if (processType(node, processResult, mediaType, "*", mp)) {
            if (mp) {
                mpl->appendMessagePart(mp);
            }
        } else {
            qCWarning(MIMETREEPARSER_LOG) << "THIS SHOULD NO LONGER HAPPEN:" << mediaType << '/' << subType;
            const auto mp = defaultHandling(node, processResult);
            if (mp) {
                if (auto _mp = mp.dynamicCast<MessagePart>()) {
                    _mp->setAttachmentFlag(node);
                }
                mpl->appendMessagePart(mp);
            }
        }
        mNodeHelper->setNodeProcessed(node, false);

        // adjust signed/encrypted flags if inline PGP was found
        processResult.adjustCryptoStatesOfNode(node);

        if (showOnlyOneMimePart()) {
            break;
        }
    }

    return mpl;
}

Interface::MessagePart::Ptr ObjectTreeParser::defaultHandling(KMime::Content *node, ProcessResult &result)
{
    // ### (mmutz) default handling should go into the respective
    // ### bodypartformatters.
    if (!htmlWriter()) {
        qCWarning(MIMETREEPARSER_LOG) << "no htmlWriter()";
        return MessagePart::Ptr();
    }

    // always show images in multipart/related when showing in html, not with an additional icon
    if (result.isImage() && node->parent() &&
            node->parent()->contentType()->subType() == "related" && mSource->htmlMail() && !showOnlyOneMimePart()) {
        QString fileName = mNodeHelper->writeNodeToTempFile(node);
        QString href = QUrl::fromLocalFile(fileName).url();
        QByteArray cid = node->contentID()->identifier();
        htmlWriter()->embedPart(cid, href);
        nodeHelper()->setNodeDisplayedEmbedded(node, true);
        return MessagePart::Ptr();
    }
    Interface::MessagePart::Ptr mp;
    ProcessResult processResult(mNodeHelper);

    if (node->contentType()->mimeType() == QByteArray("application/octet-stream") &&
            (node->contentType()->name().endsWith(QLatin1String("p7m")) ||
             node->contentType()->name().endsWith(QLatin1String("p7s")) ||
             node->contentType()->name().endsWith(QLatin1String("p7c"))
            ) &&
            processType(node, processResult, "application", "pkcs7-mime", mp)) {
        return mp;
    }

    const AttachmentStrategy *const as = attachmentStrategy();
    if (as && as->defaultDisplay(node) == AttachmentStrategy::None &&
            !showOnlyOneMimePart() &&
            node->parent() /* message is not an attachment */) {
        mNodeHelper->setNodeDisplayedHidden(node, true);
        return MessagePart::Ptr();
    }

    bool asIcon = true;
    if (!result.neverDisplayInline()) {
        if (as) {
            asIcon = as->defaultDisplay(node) == AttachmentStrategy::AsIcon;
        }
    }

    // Show it inline if showOnlyOneMimePart(), which means the user clicked the image
    // in the message structure viewer manually, and therefore wants to see the full image
    if (result.isImage() && showOnlyOneMimePart() && !result.neverDisplayInline()) {
        asIcon = false;
    }

    // neither image nor text -> show as icon
    if (!result.isImage()
            && !node->contentType()->isText()) {
        asIcon = true;
    }

    /*FIXME(Andras) port it
    // if the image is not complete do not try to show it inline
    if ( result.isImage() && !node->msgPart().isComplete() )
    asIcon = true;
    */

    if (asIcon) {
        if (!(as && as->defaultDisplay(node) == AttachmentStrategy::None) ||
                showOnlyOneMimePart()) {
            // Write the node as icon only
            return TextMessagePart::Ptr(new TextMessagePart(this, node, false, false, mSource->decryptMessage(), MimeTreeParser::IconExternal));
        } else {
            mNodeHelper->setNodeDisplayedHidden(node, true);
        }
    } else if (result.isImage()) {
        // Embed the image
        mNodeHelper->setNodeDisplayedEmbedded(node, true);
        return TextMessagePart::Ptr(new TextMessagePart(this, node, false, false, mSource->decryptMessage(), MimeTreeParser::IconInline));
    } else {
        mNodeHelper->setNodeDisplayedEmbedded(node, true);
        const auto mp = TextMessagePart::Ptr(new TextMessagePart(this, node, false, false, mSource->decryptMessage(), MimeTreeParser::NoIcon));
        result.setInlineSignatureState(mp->signatureState());
        result.setInlineEncryptionState(mp->encryptionState());
        return mp;
    }
    return MessagePart::Ptr();
}

KMMsgSignatureState ProcessResult::inlineSignatureState() const
{
    return mInlineSignatureState;
}

void ProcessResult::setInlineSignatureState(KMMsgSignatureState state)
{
    mInlineSignatureState = state;
}

KMMsgEncryptionState ProcessResult::inlineEncryptionState() const
{
    return mInlineEncryptionState;
}

void ProcessResult::setInlineEncryptionState(KMMsgEncryptionState state)
{
    mInlineEncryptionState = state;
}

bool ProcessResult::neverDisplayInline() const
{
    return mNeverDisplayInline;
}

void ProcessResult::setNeverDisplayInline(bool display)
{
    mNeverDisplayInline = display;
}

bool ProcessResult::isImage() const
{
    return mIsImage;
}

void ProcessResult::setIsImage(bool image)
{
    mIsImage = image;
}

void ProcessResult::adjustCryptoStatesOfNode(const KMime::Content *node) const
{
    if ((inlineSignatureState()  != KMMsgNotSigned) ||
            (inlineEncryptionState() != KMMsgNotEncrypted)) {
        mNodeHelper->setSignatureState(node, inlineSignatureState());
        mNodeHelper->setEncryptionState(node, inlineEncryptionState());
    }
}

void ObjectTreeParser::extractNodeInfos(KMime::Content *curNode, bool isFirstTextPart)
{
    if (isFirstTextPart) {
        mPlainTextContent += curNode->decodedText();
        mPlainTextContentCharset += NodeHelper::charset(curNode);
    }
}

void ObjectTreeParser::setPlainTextContent(QString plainTextContent)
{
    mPlainTextContent = plainTextContent;
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

QString ObjectTreeParser::quotedHTML(const QString &s, bool decorate)
{
    assert(cssHelper());

    KTextToHTML::Options convertFlags = KTextToHTML::PreserveSpaces | KTextToHTML::HighlightText;
    if (decorate && mSource->showEmoticons()) {
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

    if (mSource->showExpandQuotesMark()) {
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
        if (mSource->showExpandQuotesMark() && mSource->levelQuote() >= 0
                && mSource->levelQuote() <= (actQuoteLevel)) {
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
                if (mSource->showExpandQuotesMark()) {
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

const QTextCodec *ObjectTreeParser::codecFor(KMime::Content *node) const
{
    assert(node);
    if (mSource->overrideCodec()) {
        return mSource->overrideCodec();
    }
    return mNodeHelper->codec(node);
}

// Guesstimate if the newline at newLinePos actually separates paragraphs in the text s
// We use several heuristics:
// 1. If newLinePos points after or before (=at the very beginning of) text, it is not between paragraphs
// 2. If the previous line was longer than the wrap size, we want to consider it a paragraph on its own
//    (some clients, notably Outlook, send each para as a line in the plain-text version).
// 3. Otherwise, we check if the newline could have been inserted for wrapping around; if this
//    was the case, then the previous line will be shorter than the wrap size (which we already
//    know because of item 2 above), but adding the first word from the next line will make it
//    longer than the wrap size.
bool ObjectTreeParser::looksLikeParaBreak(const QString &s, unsigned int newLinePos) const
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

QByteArray ObjectTreeParser::plainTextContentCharset() const
{
    return mPlainTextContentCharset;
}

QByteArray ObjectTreeParser::htmlContentCharset() const
{
    return mHtmlContentCharset;
}

bool ObjectTreeParser::showOnlyOneMimePart() const
{
    return mShowOnlyOneMimePart;
}

void ObjectTreeParser::setShowOnlyOneMimePart(bool show)
{
    mShowOnlyOneMimePart = show;
}

const AttachmentStrategy *ObjectTreeParser::attachmentStrategy() const
{
    return mAttachmentStrategy;
}

HtmlWriter *ObjectTreeParser::htmlWriter() const
{
    if (mHtmlWriter) {
        return mHtmlWriter;
    }
    return mSource->htmlWriter();
}

CSSHelperBase *ObjectTreeParser::cssHelper() const
{
    return mSource->cssHelper();
}

MimeTreeParser::NodeHelper *ObjectTreeParser::nodeHelper() const
{
    return mNodeHelper;
}
