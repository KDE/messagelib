/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

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

#include "defaultrenderer.h"

#include "messageviewer_debug.h"

#include "converthtmltoplaintext.h"
#include "htmlblock.h"
#include "utils/iconnamecache.h"
#include "utils/mimetype.h"
#include "viewer/csshelperbase.h"
#include "messagepartrenderermanager.h"

#include <MimeTreeParser/HtmlWriter>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeParser>
#include <GrantleeTheme/qtresourcetemplateloader.h>

#include <Libkleo/CryptoBackend>
#include <Libkleo/CryptoBackendFactory>

#include <MessageCore/StringUtil>

#include <KEmailAddress>
#include <KIconLoader>
#include <KLocalizedString>
#include <KTextToHTML>

#include <QApplication>
#include <QFile>
#include <QStandardPaths>
#include <QTextCodec>
#include <QUrl>
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>
#include <grantlee/template.h>

using namespace MimeTreeParser;
using namespace MessageViewer;

Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(const Kleo::CryptoBackend::Protocol *)


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

static const int SIG_FRAME_COL_UNDEF = 99;
#define SIG_FRAME_COL_RED    -1
#define SIG_FRAME_COL_YELLOW  0
#define SIG_FRAME_COL_GREEN   1
QString sigStatusToString(const Kleo::CryptoBackend::Protocol *cryptProto,
                          int status_code,
                          GpgME::Signature::Summary summary,
                          int &frameColor,
                          bool &showKeyInfos)
{
    // note: At the moment frameColor and showKeyInfos are
    //       used for CMS only but not for PGP signatures
    // pending(khz): Implement usage of these for PGP sigs as well.
    showKeyInfos = true;
    QString result;
    if (cryptProto) {
        if (cryptProto == Kleo::CryptoBackendFactory::instance()->openpgp()) {
            // process enum according to it's definition to be read in
            // GNU Privacy Guard CVS repository /gpgme/gpgme/gpgme.h
            switch (status_code) {
            case 0: // GPGME_SIG_STAT_NONE
                result = i18n("Error: Signature not verified");
                break;
            case 1: // GPGME_SIG_STAT_GOOD
                result = i18n("Good signature");
                break;
            case 2: // GPGME_SIG_STAT_BAD
                result = i18n("<b>Bad</b> signature");
                break;
            case 3: // GPGME_SIG_STAT_NOKEY
                result = i18n("No public key to verify the signature");
                break;
            case 4: // GPGME_SIG_STAT_NOSIG
                result = i18n("No signature found");
                break;
            case 5: // GPGME_SIG_STAT_ERROR
                result = i18n("Error verifying the signature");
                break;
            case 6: // GPGME_SIG_STAT_DIFF
                result = i18n("Different results for signatures");
                break;
            /* PENDING(khz) Verify exact meaning of the following values:
            case 7: // GPGME_SIG_STAT_GOOD_EXP
            return i18n("Signature certificate is expired");
            break;
            case 8: // GPGME_SIG_STAT_GOOD_EXPKEY
            return i18n("One of the certificate's keys is expired");
            break;
            */
            default:
                result.clear();   // do *not* return a default text here !
                break;
            }
        } else if (cryptProto == Kleo::CryptoBackendFactory::instance()->smime()) {
            // process status bits according to SigStatus_...
            // definitions in kdenetwork/libkdenetwork/cryptplug.h

            if (summary == GpgME::Signature::None) {
                result = i18n("No status information available.");
                frameColor = SIG_FRAME_COL_YELLOW;
                showKeyInfos = false;
                return result;
            }

            if (summary & GpgME::Signature::Valid) {
                result = i18n("Good signature.");
                // Note:
                // Here we are work differently than KMail did before!
                //
                // The GOOD case ( == sig matching and the complete
                // certificate chain was verified and is valid today )
                // by definition does *not* show any key
                // information but just states that things are OK.
                //           (khz, according to LinuxTag 2002 meeting)
                frameColor = SIG_FRAME_COL_GREEN;
                showKeyInfos = false;
                return result;
            }

            // we are still there?  OK, let's test the different cases:

            // we assume green, test for yellow or red (in this order!)
            frameColor = SIG_FRAME_COL_GREEN;
            QString result2;
            if (summary & GpgME::Signature::KeyExpired) {
                // still is green!
                result2 += i18n("One key has expired.");
            }
            if (summary & GpgME::Signature::SigExpired) {
                // and still is green!
                result2 += i18n("The signature has expired.");
            }

            // test for yellow:
            if (summary & GpgME::Signature::KeyMissing) {
                result2 += i18n("Unable to verify: key missing.");
                // if the signature certificate is missing
                // we cannot show information on it
                showKeyInfos = false;
                frameColor = SIG_FRAME_COL_YELLOW;
            }
            if (summary & GpgME::Signature::CrlMissing) {
                result2 += i18n("CRL not available.");
                frameColor = SIG_FRAME_COL_YELLOW;
            }
            if (summary & GpgME::Signature::CrlTooOld) {
                result2 += i18n("Available CRL is too old.");
                frameColor = SIG_FRAME_COL_YELLOW;
            }
            if (summary & GpgME::Signature::BadPolicy) {
                result2 += i18n("A policy was not met.");
                frameColor = SIG_FRAME_COL_YELLOW;
            }
            if (summary & GpgME::Signature::SysError) {
                result2 += i18n("A system error occurred.");
                // if a system error occurred
                // we cannot trust any information
                // that was given back by the plug-in
                showKeyInfos = false;
                frameColor = SIG_FRAME_COL_YELLOW;
            }

            // test for red:
            if (summary & GpgME::Signature::KeyRevoked) {
                // this is red!
                result2 += i18n("One key has been revoked.");
                frameColor = SIG_FRAME_COL_RED;
            }
            if (summary & GpgME::Signature::Red) {
                if (result2.isEmpty())
                    // Note:
                    // Here we are work differently than KMail did before!
                    //
                    // The BAD case ( == sig *not* matching )
                    // by definition does *not* show any key
                    // information but just states that things are BAD.
                    //
                    // The reason for this: In this case ALL information
                    // might be falsificated, we can NOT trust the data
                    // in the body NOT the signature - so we don't show
                    // any key/signature information at all!
                    //         (khz, according to LinuxTag 2002 meeting)
                {
                    showKeyInfos = false;
                }
                frameColor = SIG_FRAME_COL_RED;
            } else {
                result.clear();
            }

            if (SIG_FRAME_COL_GREEN == frameColor) {
                result = i18n("Good signature.");
            } else if (SIG_FRAME_COL_RED == frameColor) {
                result = i18n("<b>Bad</b> signature.");
            } else {
                result.clear();
            }

            if (!result2.isEmpty()) {
                if (!result.isEmpty()) {
                    result.append(QLatin1String("<br />"));
                }
                result.append(result2);
            }
        }
        /*
        // add i18n support for 3rd party plug-ins here:
        else if ( cryptPlug->libName().contains( "yetanotherpluginname", Qt::CaseInsensitive )) {

        }
        */
    }
    return result;
}

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

QString processHtml(const QString &htmlSource, QString &extraHead)
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

class CacheHtmlWriter : public MimeTreeParser::HtmlWriter
{
public:
    explicit CacheHtmlWriter(MimeTreeParser::HtmlWriter *baseWriter)
        : mBaseWriter(baseWriter)
    {}
    virtual ~CacheHtmlWriter() {}

    void begin(const QString &text) Q_DECL_OVERRIDE {mBaseWriter->begin(text);}
    void write(const QString &str) Q_DECL_OVERRIDE {
        html.append(str);
    }
    void end() Q_DECL_OVERRIDE {mBaseWriter->end();}
    void reset() Q_DECL_OVERRIDE {mBaseWriter->reset();}
    void queue(const QString &str) Q_DECL_OVERRIDE {
        html.append(str);
    }
    void flush() Q_DECL_OVERRIDE {mBaseWriter->flush();}
    void embedPart(const QByteArray &contentId, const QString &url) Q_DECL_OVERRIDE {mBaseWriter->embedPart(contentId, url);}
    void extraHead(const QString &extra) Q_DECL_OVERRIDE {mBaseWriter->extraHead(extra);}

    QString html;
    MimeTreeParser::HtmlWriter *mBaseWriter;
};

class MimeTreeParser::DefaultRendererPrivate
{
public:
    DefaultRendererPrivate(DefaultRenderer *qPtr, const Interface::MessagePart::Ptr &msgPart, CSSHelperBase *cssHelper)
        : mMsgPart(msgPart)
        , q(qPtr)
        , mOldWriter(msgPart->htmlWriter())
        , mCSSHelper(cssHelper)
    {
        mHtml = renderFactory(mMsgPart, QSharedPointer<CacheHtmlWriter>());
    }
    ~DefaultRendererPrivate()
    {
    }

    inline QString alignText()
    {
        return QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
    }
    CSSHelperBase *cssHelper() const
    {
        auto mp = mMsgPart.dynamicCast<MessagePart>();
        if (mp) {
            return mCSSHelper;
        }
        return Q_NULLPTR;
    }

    Interface::ObjectTreeSource *source() const
    {
        auto mp = mMsgPart.dynamicCast<MessagePart>();
        if (mp) {
            return mp->source();
        }
        return Q_NULLPTR;
    }

    void renderSubParts(const MessagePart::Ptr &msgPart, const QSharedPointer<CacheHtmlWriter> &htmlWriter)
    {
        foreach (const auto &_m, msgPart->subParts()) {
            const auto m = _m.dynamicCast<MessagePart>();
            if (m) {
                htmlWriter->queue(renderFactory(m, htmlWriter));
            }
        }
    }

    QString render(const MessagePartList::Ptr &mp)
    {
        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr rBlock;
            HTMLBlock::Ptr aBlock;

            if (mp->isRoot()) {
                rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter.data()));
            }

            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }

            mp->setHtmlWriter(htmlWriter.data());
            renderSubParts(mp, htmlWriter);
            mp->setHtmlWriter(mOldWriter);
        }

        return htmlWriter->html;
    }

    QString render(const MimeMessagePart::Ptr &mp)
    {
        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            HTMLBlock::Ptr rBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }

            if (mp->isRoot()) {
                rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter.data()));
            }

            renderSubParts(mp, htmlWriter);
        }

        return htmlWriter->html;
    }

    QString render(const EncapsulatedRfc822MessagePart::Ptr &mp)
    {
        if (!mp->hasSubParts()) {
            return QString();
        }
        Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/encapsulatedrfc822messagepart.html"));
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;

        c.insert(QStringLiteral("block"), &block);
        block.setProperty("dir", alignText());
        block.setProperty("link", mp->mOtp->nodeHelper()->asHREF(mp->mMessage.data(), QStringLiteral("body")));

        c.insert(QStringLiteral("msgHeader"), mp->source()->createMessageHeader(mp->mMessage.data()));
        {
            auto _htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
            renderSubParts(mp, _htmlWriter);
            c.insert(QStringLiteral("content"), _htmlWriter->html);
        }

        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }
            const auto html = t->render(&c);
            htmlWriter->queue(html);
        }
        return htmlWriter->html;
    }

    QString render(const TextMessagePart::Ptr &mp)
    {
        KMime::Content *node = mp->mNode;
        NodeHelper *nodeHelper = mp->mOtp->nodeHelper();

        if (mp->isHidden()) {
            return QString();
        }

        Grantlee::Template t;
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;
        c.insert(QStringLiteral("block"), &block);

        block.setProperty("showTextFrame", mp->mDrawFrame);
        block.setProperty("label", MessageCore::StringUtil::quoteHtmlChars(NodeHelper::fileName(node), true));
        block.setProperty("comment", MessageCore::StringUtil::quoteHtmlChars(node->contentDescription()->asUnicodeString(), true));
        block.setProperty("link", nodeHelper->asHREF(node, QStringLiteral("body")));
        block.setProperty("showLink", mp->mShowLink);
        block.setProperty("dir", alignText());

        if (mp->mAsIcon != MimeTreeParser::NoIcon) {
            t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/asiconpart.html"));
            block.setProperty("iconSize", KIconLoader::global()->currentSize(KIconLoader::Desktop));
            block.setProperty("inline", (mp->mAsIcon == MimeTreeParser::IconInline));

            QString iconPath;
            if (mp->mAsIcon == MimeTreeParser::IconInline) {
                iconPath = nodeHelper->writeNodeToTempFile(node);
            } else {
                iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
                if (iconPath.right(14) == QLatin1String("mime_empty.png")) {
                    nodeHelper->magicSetType(node);
                    iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
                }
            }
            block.setProperty("iconPath", QUrl::fromLocalFile(iconPath).url());

            const QString name = node->contentType()->name();
            QString label = name.isEmpty() ? NodeHelper::fileName(node) : name;
            QString comment = node->contentDescription()->asUnicodeString();

            if (label.isEmpty()) {
                label = i18nc("display name for an unnamed attachment", "Unnamed");
            }
            label = MessageCore::StringUtil::quoteHtmlChars(label, true);

            comment = MessageCore::StringUtil::quoteHtmlChars(comment, true);
            if (label == comment) {
                comment.clear();
            }

            block.setProperty("label", label);
            block.setProperty("comment", comment);

        } else {
            t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/textmessagepart.html"));
            auto _htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
            renderSubParts(mp, _htmlWriter);
            c.insert(QStringLiteral("content"), _htmlWriter->html);
        }

        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }
            const auto html = t->render(&c);
            htmlWriter->queue(html);
        }
        return htmlWriter->html;
    }

    QString quotedHTML(const QString &s, bool decorate)
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

        // qCDebug(MESSAGEVIEWER_LOG) << "========================================\n"
        //                            << htmlStr
        //                            << "\n======================================\n";
        return htmlStr;
    }

    QString render(const MessagePart::Ptr mp)
    {
        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }

            htmlWriter->queue(quotedHTML(mp->text(), false));
        }
        return htmlWriter->html;
    }

    QString render(const HtmlMessagePart::Ptr &mp)
    {
        Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/htmlmessagepart.html"));
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;

        c.insert(QStringLiteral("block"), &block);

        block.setProperty("htmlMail", mp->source()->htmlMail());
        block.setProperty("loadExternal", mp->source()->htmlLoadExternal());

        {
            QString extraHead;
            QString bodyText = processHtml(mp->mBodyHTML, extraHead);

            if (mp->source()->htmlMail()) {
                mp->mOtp->nodeHelper()->setNodeDisplayedEmbedded(mp->mNode, true);
                mOldWriter->extraHead(extraHead);
            }

            block.setProperty("containsExternalReferences", containsExternalReferences(bodyText, extraHead));
            c.insert(QStringLiteral("content"), bodyText);
        }

        {
            ConvertHtmlToPlainText convert;
            convert.setHtmlString(mp->mBodyHTML);
            QString plaintext = convert.generatePlainText();
            plaintext.replace(QLatin1String("\n"), QStringLiteral("<br>"));
            c.insert(QStringLiteral("plaintext"), plaintext);
        }
        mp->source()->setHtmlMode(Util::Html);

        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }
            const auto html = t->render(&c);
            htmlWriter->queue(html);
        }
        return htmlWriter->html;

    }

    QString renderEncrypted(const CryptoMessagePart::Ptr &mp)
    {
        KMime::Content *node = mp->mNode;
        const auto metaData = mp->mMetaData;

        Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/encryptedmessagepart.html"));
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;

        if (metaData.isSigned) {
            c.insert(QStringLiteral("content"), renderSigned(mp));
        } else if (node) {
            auto _htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
            {
                HTMLBlock::Ptr rBlock;
                if (mp->isRoot()) {
                    rBlock = HTMLBlock::Ptr(new RootBlock(_htmlWriter.data()));
                }
                renderSubParts(mp, _htmlWriter);
            }
            c.insert(QStringLiteral("content"), _htmlWriter->html);
        } else {
            c.insert(QStringLiteral("content"), render(mp.dynamicCast<MessagePart>()));
        }

        c.insert(QStringLiteral("cryptoProto"), QVariant::fromValue(mp->mCryptoProto));
        if (mp->mDecryptRecipients.size() > 0) {
            c.insert(QStringLiteral("decryptedRecipients"), QVariant::fromValue(mp->mDecryptRecipients));
        }
        c.insert(QStringLiteral("block"), &block);

        block.setProperty("dir", alignText());
        block.setProperty("inProgress", metaData.inProgress);
        block.setProperty("isDecrypted", mp->decryptMessage());
        block.setProperty("isDecryptable", metaData.isDecryptable);
        block.setProperty("decryptIcon", QUrl::fromLocalFile(IconNameCache::instance()->iconPath(QStringLiteral("document-decrypt"), KIconLoader::Small)).url());
        block.setProperty("errorText", metaData.errorText);
        block.setProperty("noSecKey", mp->mNoSecKey);

        const auto html = t->render(&c);

        return html;
    }

    QString renderSigned(const CryptoMessagePart::Ptr &mp)
    {
        KMime::Content *node = mp->mNode;
        const auto metaData = mp->mMetaData;
        auto cryptoProto = mp->mCryptoProto;

        const bool isSMIME = cryptoProto && (cryptoProto == Kleo::CryptoBackendFactory::instance()->smime());

        Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/signedmessagepart.html"));
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;

        if (node) {
            auto _htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
            {
                HTMLBlock::Ptr rBlock;
                if (mp->isRoot()) {
                    rBlock = HTMLBlock::Ptr(new RootBlock(_htmlWriter.data()));
                }
                renderSubParts(mp, _htmlWriter);
            }
            c.insert(QStringLiteral("content"), _htmlWriter->html);
        } else {
            c.insert(QStringLiteral("content"), render(mp.dynamicCast<MessagePart>()));
        }

        c.insert(QStringLiteral("cryptoProto"), QVariant::fromValue(cryptoProto));
        c.insert(QStringLiteral("block"), &block);

        block.setProperty("dir", alignText());
        block.setProperty("inProgress", metaData.inProgress);
        block.setProperty("errorText", metaData.errorText);

        block.setProperty("detailHeader", mp->source()->showSignatureDetails());
        block.setProperty("printing", false);
        block.setProperty("addr", metaData.signerMailAddresses.join(QStringLiteral(",")));
        block.setProperty("technicalProblem", metaData.technicalProblem);
        block.setProperty("keyId", metaData.keyId);
        if (metaData.creationTime.isValid()) {      //should be handled inside grantlee but currently not possible see: https://bugs.kde.org/363475
            block.setProperty("creationTime", QLocale().toString(metaData.creationTime, QLocale::ShortFormat));
        }
        block.setProperty("isGoodSignature", metaData.isGoodSignature);
        block.setProperty("isSMIME", isSMIME);

        if (metaData.keyTrust == GpgME::Signature::Unknown) {
            block.setProperty("keyTrust", QStringLiteral("unknown"));
        } else if (metaData.keyTrust == GpgME::Signature::Marginal) {
            block.setProperty("keyTrust", QStringLiteral("marginal"));
        } else if (metaData.keyTrust == GpgME::Signature::Full) {
            block.setProperty("keyTrust", QStringLiteral("full"));
        } else if (metaData.keyTrust == GpgME::Signature::Ultimate) {
            block.setProperty("keyTrust", QStringLiteral("ultimate"));
        } else {
            block.setProperty("keyTrust", QStringLiteral("untrusted"));
        }

        QString startKeyHREF;
        {
            QString keyWithWithoutURL;
            if (cryptoProto) {
                startKeyHREF =
                    QStringLiteral("<a href=\"kmail:showCertificate#%1 ### %2 ### %3\">")
                    .arg(cryptoProto->displayName(),
                         cryptoProto->name(),
                         QString::fromLatin1(metaData.keyId));

                keyWithWithoutURL = QStringLiteral("%1%2</a>").arg(startKeyHREF, QString::fromLatin1(QByteArray(QByteArray("0x") + metaData.keyId)));
            } else {
                keyWithWithoutURL = QStringLiteral("0x") + QString::fromUtf8(metaData.keyId);
            }
            block.setProperty("keyWithWithoutURL", keyWithWithoutURL);
        }

        bool onlyShowKeyURL = false;
        bool showKeyInfos = false;
        bool cannotCheckSignature = true;
        QString signer = metaData.signer;
        QString statusStr;
        QString mClass;
        QString greenCaseWarning;

        if (metaData.inProgress) {
            mClass = QStringLiteral("signInProgress");
        } else {
            const QStringList &blockAddrs(metaData.signerMailAddresses);
            // note: At the moment frameColor and showKeyInfos are
            //       used for CMS only but not for PGP signatures
            // pending(khz): Implement usage of these for PGP sigs as well.
            int frameColor = SIG_FRAME_COL_UNDEF;
            statusStr = sigStatusToString(cryptoProto,
                                          metaData.status_code,
                                          metaData.sigSummary,
                                          frameColor,
                                          showKeyInfos);
            // if needed fallback to english status text
            // that was reported by the plugin
            if (statusStr.isEmpty()) {
                statusStr = metaData.status;
            }
            if (metaData.technicalProblem) {
                frameColor = SIG_FRAME_COL_YELLOW;
            }

            switch (frameColor) {
            case SIG_FRAME_COL_RED:
                cannotCheckSignature = false;
                break;
            case SIG_FRAME_COL_YELLOW:
                cannotCheckSignature = true;
                break;
            case SIG_FRAME_COL_GREEN:
                cannotCheckSignature = false;
                break;
            }

            // temporary hack: always show key information!
            showKeyInfos = true;

            if (isSMIME && (SIG_FRAME_COL_UNDEF != frameColor)) {

                switch (frameColor) {
                case SIG_FRAME_COL_RED:
                    mClass = QStringLiteral("signErr");
                    onlyShowKeyURL = true;
                    break;
                case SIG_FRAME_COL_YELLOW:
                    if (metaData.technicalProblem) {
                        mClass = QStringLiteral("signWarn");
                    } else {
                        mClass = QStringLiteral("signOkKeyBad");
                    }
                    break;
                case SIG_FRAME_COL_GREEN:
                    mClass = QStringLiteral("signOkKeyOk");
                    // extra hint for green case
                    // that email addresses in DN do not match fromAddress
                    QString msgFrom(KEmailAddress::extractEmailAddress(mp->mFromAddress));
                    QString certificate;
                    if (metaData.keyId.isEmpty()) {
                        certificate = i18n("certificate");
                    } else {
                        certificate = startKeyHREF + i18n("certificate") + QStringLiteral("</a>");
                    }

                    if (!blockAddrs.empty()) {
                        if (!blockAddrs.contains(msgFrom, Qt::CaseInsensitive)) {
                            greenCaseWarning =
                                QStringLiteral("<u>") +
                                i18nc("Start of warning message.", "Warning:") +
                                QStringLiteral("</u> ") +
                                i18n("Sender's mail address is not stored in the %1 used for signing.",
                                     certificate) +
                                QStringLiteral("<br />") +
                                i18n("sender: ") +
                                msgFrom +
                                QStringLiteral("<br />") +
                                i18n("stored: ");
                            // We cannot use Qt's join() function here but
                            // have to join the addresses manually to
                            // extract the mail addresses (without '<''>')
                            // before including it into our string:
                            bool bStart = true;
                            QStringList::ConstIterator end(blockAddrs.constEnd());
                            for (QStringList::ConstIterator it = blockAddrs.constBegin();
                                    it != end; ++it) {
                                if (!bStart) {
                                    greenCaseWarning.append(QStringLiteral(", <br />&nbsp; &nbsp;"));
                                }

                                bStart = false;
                                greenCaseWarning.append(KEmailAddress::extractEmailAddress(*it));
                            }
                        }
                    } else {
                        greenCaseWarning =
                            QStringLiteral("<u>") +
                            i18nc("Start of warning message.", "Warning:") +
                            QStringLiteral("</u> ") +
                            i18n("No mail address is stored in the %1 used for signing, "
                                 "so we cannot compare it to the sender's address %2.",
                                 certificate,
                                 msgFrom);
                    }
                    break;
                }

                if (showKeyInfos && !cannotCheckSignature) {
                    if (metaData.signer.isEmpty()) {
                        signer.clear();
                    } else {
                        if (!blockAddrs.empty()) {
                            const QUrl address = KEmailAddress::encodeMailtoUrl(blockAddrs.first());
                            signer = QStringLiteral("<a href=\"mailto:%1\">%2</a>").arg(QLatin1String(QUrl::toPercentEncoding(address.path())), signer);
                        }
                    }
                }
            } else {
                if (metaData.signer.isEmpty() || metaData.technicalProblem) {
                    mClass = QStringLiteral("signWarn");
                } else {
                    // HTMLize the signer's user id and create mailto: link
                    signer = MessageCore::StringUtil::quoteHtmlChars(signer, true);
                    signer = QStringLiteral("<a href=\"mailto:%1\">%1</a>").arg(signer);

                    if (metaData.isGoodSignature) {
                        if (metaData.keyTrust < GpgME::Signature::Marginal) {
                            mClass = QStringLiteral("signOkKeyBad");
                        } else {
                            mClass = QStringLiteral("signOkKeyOk");
                        }
                    } else {
                        mClass = QStringLiteral("signErr");
                    }
                }
            }
        }

        block.setProperty("onlyShowKeyURL", onlyShowKeyURL);
        block.setProperty("showKeyInfos", showKeyInfos);
        block.setProperty("cannotCheckSignature", cannotCheckSignature);
        block.setProperty("signer", signer);
        block.setProperty("statusStr", statusStr);
        block.setProperty("signClass", mClass);
        block.setProperty("greenCaseWarning", greenCaseWarning);

        const auto html = t->render(&c);

        return html;
    }

    QString render(const CryptoMessagePart::Ptr &mp)
    {
        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        const auto metaData = mp->mMetaData;

        if (metaData.isEncrypted) {
            {
                HTMLBlock::Ptr aBlock;
                if (mp->isAttachment()) {
                    aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
                }
                htmlWriter->queue(renderEncrypted(mp));
            }
            return htmlWriter->html;
        }

        if (metaData.isSigned) {
            {
                HTMLBlock::Ptr aBlock;
                if (mp->isAttachment()) {
                    aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
                }
                htmlWriter->queue(renderSigned(mp));
            }
            return htmlWriter->html;
        }

        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }

            if (mp->mNode) {
                renderSubParts(mp, htmlWriter);
            } else {
                htmlWriter->queue(render(mp.dynamicCast<MessagePart>()));
            }
        }
        return htmlWriter->html;
    }

    QString render(const AlternativeMessagePart::Ptr &mp)
    {
        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }

            MimeMessagePart::Ptr part(mp->mTextPart);
            if (mp->viewHtml() && mp->mHTMLPart) {
                part = mp->mHTMLPart;
            } else if (mp->text().trimmed().isEmpty()) {
                part = mp->mHTMLPart;
            }

            htmlWriter->queue(render(part));
        }
        return htmlWriter->html;
    }

    QString render(const CertMessagePart::Ptr &mp)
    {
        const GpgME::ImportResult &importResult(mp->mImportResult);
        Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/certmessagepart.html"));
        Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
        QObject block;

        c.insert(QStringLiteral("block"), &block);
        block.setProperty("importError", QString::fromLocal8Bit(importResult.error().asString()));
        block.setProperty("nImp", importResult.numImported());
        block.setProperty("nUnc", importResult.numUnchanged());
        block.setProperty("nSKImp", importResult.numSecretKeysImported());
        block.setProperty("nSKUnc", importResult.numSecretKeysUnchanged());

        QVariantList keylist;
        const auto imports = importResult.imports();

        auto end(imports.end());
        for (auto it = imports.begin(); it != end; ++it) {
            QObject *key(new QObject(mp.data()));
            key->setProperty("error", QString::fromLocal8Bit((*it).error().asString()));
            key->setProperty("status", (*it).status());
            key->setProperty("fingerprint", QLatin1String((*it).fingerprint()));
            keylist << QVariant::fromValue(key);
        }

        auto htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        {
            HTMLBlock::Ptr aBlock;
            if (mp->isAttachment()) {
                aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter.data(), mp->attachmentNode()));
            }
            const auto html = t->render(&c);
            htmlWriter->queue(html);
        }
        return htmlWriter->html;
    }

    QString renderFactory(const Interface::MessagePart::Ptr &msgPart, const QSharedPointer<CacheHtmlWriter> &htmlWriter)
    {
        const QString className = QString::fromUtf8(msgPart->metaObject()->className());

        if (className == QStringLiteral("MimeTreeParser::MessagePartList")) {
            auto mp = msgPart.dynamicCast<MessagePartList>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::MimeMessagePart")) {
            auto mp = msgPart.dynamicCast<MimeMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::EncapsulatedRfc822MessagePart")) {
            auto mp = msgPart.dynamicCast<EncapsulatedRfc822MessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::TextMessagePart")) {
            auto mp = msgPart.dynamicCast<TextMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::HtmlMessagePart")) {
            auto mp = msgPart.dynamicCast<HtmlMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::CryptoMessagePart")) {
            auto mp = msgPart.dynamicCast<CryptoMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::AlternativeMessagePart")) {
            auto mp = msgPart.dynamicCast<AlternativeMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::CertMessagePart")) {
            auto mp = msgPart.dynamicCast<CertMessagePart>();
            if (mp) {
                return render(mp);
            }
        } else if (className == QStringLiteral("MimeTreeParser::MessagePart")) {
            auto mp = msgPart.dynamicCast<MessagePart>();
            if (mp) {
                return render(mp);
            }
        }

        qCDebug(MESSAGEVIEWER_LOG) << "We got a unkonwn classname, using default behaviour for " << className;

        auto _htmlWriter = htmlWriter;
        if (!_htmlWriter) {
            _htmlWriter = QSharedPointer<CacheHtmlWriter>(new CacheHtmlWriter(mOldWriter));
        }
        msgPart->setHtmlWriter(_htmlWriter.data());
        msgPart->html(false);
        msgPart->setHtmlWriter(mOldWriter);
        if (!htmlWriter) {
            return _htmlWriter->html;
        }

        return QString();
    }


    QString mHtml;
    Interface::MessagePart::Ptr mMsgPart;
private:
    DefaultRenderer *q;
    HtmlWriter *mOldWriter;

    CSSHelperBase *mCSSHelper;
    QString mCollapseIcon;
    QString mExpandIcon;
};

DefaultRenderer::DefaultRenderer(const MimeTreeParser::Interface::MessagePart::Ptr &msgPart, CSSHelperBase *cssHelper)
    : d(new MimeTreeParser::DefaultRendererPrivate(this, msgPart, cssHelper))
{
}

DefaultRenderer::~DefaultRenderer()
{
    delete d;
}

QString DefaultRenderer::html() const
{
    return d->mHtml;
}
