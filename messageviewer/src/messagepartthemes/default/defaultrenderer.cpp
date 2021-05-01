/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "defaultrenderer.h"

#include "defaultrenderer_p.h"

#include "utils/messageviewerutil.h"

#include "messageviewer_debug.h"

#include "converthtmltoplaintext.h"
#include "htmlblock.h"
#include "messagepartrendererbase.h"
#include "messagepartrendererfactory.h"
#include "messagepartrenderermanager.h"
#include "utils/iconnamecache.h"
#include "utils/mimetype.h"
#include "viewer/attachmentstrategy.h"
#include "viewer/csshelperbase.h"

#include "htmlwriter/bufferedhtmlwriter.h"
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeParser>

#include <QGpgME/Protocol>

#include <MessageCore/StringUtil>

#include <KEmailAddress>
#include <KIconLoader>
#include <KLocalizedString>

#include <QUrl>

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/template.h>
#include <grantlee/templateloader.h>

using namespace MimeTreeParser;
using namespace MessageViewer;

Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(GpgME::Key)
Q_DECLARE_METATYPE(const QGpgME::Protocol *)

static const int SIG_FRAME_COL_UNDEF = 99;
#define SIG_FRAME_COL_RED -1
#define SIG_FRAME_COL_YELLOW 0
#define SIG_FRAME_COL_GREEN 1
QString sigStatusToString(const QGpgME::Protocol *cryptProto, int status_code, GpgME::Signature::Summary summary, int &frameColor, bool &showKeyInfos)
{
    // note: At the moment frameColor and showKeyInfos are
    //       used for CMS only but not for PGP signatures
    // pending(khz): Implement usage of these for PGP sigs as well.
    showKeyInfos = true;
    QString result;
    if (cryptProto) {
        if (cryptProto == QGpgME::openpgp()) {
            // process enum according to it's definition to be read in
            // GNU Privacy Guard CVS repository /gpgme/gpgme/gpgme.h
            switch (status_code) {
            case 0: // GPGME_SIG_STAT_NONE
                result = i18n("Error: Signature not verified");
                frameColor = SIG_FRAME_COL_YELLOW;
                break;
            case 1: // GPGME_SIG_STAT_GOOD
                result = i18n("Good signature");
                frameColor = SIG_FRAME_COL_GREEN;
                break;
            case 2: // GPGME_SIG_STAT_BAD
                result = i18n("Bad signature");
                frameColor = SIG_FRAME_COL_RED;
                break;
            case 3: // GPGME_SIG_STAT_NOKEY
                result = i18n("No public key to verify the signature");
                frameColor = SIG_FRAME_COL_RED;
                break;
            case 4: // GPGME_SIG_STAT_NOSIG
                result = i18n("No signature found");
                frameColor = SIG_FRAME_COL_RED;
                break;
            case 5: // GPGME_SIG_STAT_ERROR
                result = i18n("Error verifying the signature");
                frameColor = SIG_FRAME_COL_RED;
                break;
            case 6: // GPGME_SIG_STAT_DIFF
                result = i18n("Different results for signatures");
                frameColor = SIG_FRAME_COL_RED;
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
                result.clear(); // do *not* return a default text here !
                break;
            }
        } else if (cryptProto == QGpgME::smime()) {
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
                result2 = i18n("One key has expired.");
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
                if (result2.isEmpty()) {
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
                    showKeyInfos = false;
                }
                frameColor = SIG_FRAME_COL_RED;
            } else {
                result.clear();
            }

            if (SIG_FRAME_COL_GREEN == frameColor) {
                result = i18n("Good signature.");
            } else if (SIG_FRAME_COL_RED == frameColor) {
                result = i18n("Bad signature.");
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

DefaultRendererPrivate::DefaultRendererPrivate(CSSHelperBase *cssHelper, const MessagePartRendererFactory *rendererFactory)
    : mCSSHelper(cssHelper)
    , mRendererFactory(rendererFactory)
{
}

DefaultRendererPrivate::~DefaultRendererPrivate()
{
}

CSSHelperBase *DefaultRendererPrivate::cssHelper() const
{
    return mCSSHelper;
}

Interface::ObjectTreeSource *DefaultRendererPrivate::source() const
{
    return mMsgPart->source();
}

void DefaultRendererPrivate::renderSubParts(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    for (const auto &m : msgPart->subParts()) {
        renderFactory(m, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const MessagePartList::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr rBlock;
    HTMLBlock::Ptr aBlock;

    if (mp->isRoot()) {
        rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
    }

    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    renderSubParts(mp, htmlWriter);
}

void DefaultRendererPrivate::render(const MimeMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr aBlock;
    HTMLBlock::Ptr rBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    if (mp->isRoot()) {
        rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
    }

    renderSubParts(mp, htmlWriter);
}

void DefaultRendererPrivate::render(const EncapsulatedRfc822MessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    if (!mp->hasSubParts()) {
        return;
    }
    Grantlee::Template t = MessagePartRendererManager::self()->loadByName(QStringLiteral("encapsulatedrfc822messagepart.html"));
    Grantlee::Context c = MessagePartRendererManager::self()->createContext();
    QObject block;

    c.insert(QStringLiteral("block"), &block);
    block.setProperty("link", mp->nodeHelper()->asHREF(mp->message().data(), QStringLiteral("body")));

    c.insert(QStringLiteral("msgHeader"), mCreateMessageHeader(mp->message().data()));
    c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([this, mp, htmlWriter](Grantlee::OutputStream *) {
                 renderSubParts(mp, htmlWriter);
             }));

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::render(const HtmlMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("htmlmessagepart.html"));
    Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    c.insert(QStringLiteral("block"), &block);

    auto preferredMode = mp->source()->preferredMode();
    const bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);
    block.setProperty("htmlMail", isHtmlPreferred);
    block.setProperty("loadExternal", htmlLoadExternal());
    block.setProperty("isPrinting", isPrinting());
    {
        // laurent: FIXME port to async method webengine
        Util::HtmlMessageInfo messageInfo = Util::processHtml(mp->bodyHtml());

        if (isHtmlPreferred) {
            mp->nodeHelper()->setNodeDisplayedEmbedded(mp->content(), true);
            htmlWriter->setExtraHead(messageInfo.extraHead);
            htmlWriter->setStyleBody(Util::parseBodyStyle(messageInfo.bodyStyle));
        }

        block.setProperty("containsExternalReferences", Util::containsExternalReferences(messageInfo.htmlSource, messageInfo.extraHead));
        c.insert(QStringLiteral("content"), messageInfo.htmlSource);
    }

    {
        ConvertHtmlToPlainText convert;
        convert.setHtmlString(mp->bodyHtml());
        QString plaintext = convert.generatePlainText();
        plaintext.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
        c.insert(QStringLiteral("plaintext"), plaintext);
    }
    mp->source()->setHtmlMode(MimeTreeParser::Util::Html,
                              QList<MimeTreeParser::Util::HtmlMode>() << MimeTreeParser::Util::Html << MimeTreeParser::Util::Normal);

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::renderEncrypted(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    KMime::Content *node = mp->content();
    const auto metaData = *mp->partMetaData();

    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("encryptedmessagepart.html"));
    Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    if (node || mp->hasSubParts()) {
        c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([this, mp, htmlWriter](Grantlee::OutputStream *) {
                     HTMLBlock::Ptr rBlock;
                     if (mp->content() && mp->isRoot()) {
                         rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
                     }
                     renderSubParts(mp, htmlWriter);
                 }));
    } else if (!metaData.inProgress) {
        c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([this, mp, htmlWriter](Grantlee::OutputStream *) {
                     renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
                 }));
    }

    c.insert(QStringLiteral("cryptoProto"), QVariant::fromValue(mp->cryptoProto()));
    if (!mp->decryptRecipients().empty()) {
        c.insert(QStringLiteral("decryptedRecipients"), QVariant::fromValue(mp->decryptRecipients()));
    }
    c.insert(QStringLiteral("block"), &block);

    block.setProperty("isPrinting", isPrinting());
    block.setProperty("detailHeader", showEncryptionDetails());
    block.setProperty("inProgress", metaData.inProgress);
    block.setProperty("isDecrypted", mp->decryptMessage());
    block.setProperty("isDecryptable", metaData.isDecryptable);
    block.setProperty("decryptIcon", QUrl::fromLocalFile(IconNameCache::instance()->iconPath(QStringLiteral("document-decrypt"), KIconLoader::Small)).url());
    block.setProperty("errorText", metaData.errorText);
    block.setProperty("noSecKey", mp->isNoSecKey());

    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::renderSigned(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    KMime::Content *node = mp->content();
    const auto metaData = *mp->partMetaData();
    auto cryptoProto = mp->cryptoProto();

    const bool isSMIME = cryptoProto && (cryptoProto == QGpgME::smime());

    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("signedmessagepart.html"));
    Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;

    if (node) {
        c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([this, mp, htmlWriter](Grantlee::OutputStream *) {
                     HTMLBlock::Ptr rBlock;
                     if (mp->isRoot()) {
                         rBlock = HTMLBlock::Ptr(new RootBlock(htmlWriter));
                     }
                     renderSubParts(mp, htmlWriter);
                 }));
    } else if (!metaData.inProgress) {
        c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([this, mp, htmlWriter](Grantlee::OutputStream *) {
                     renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
                 }));
    }

    c.insert(QStringLiteral("cryptoProto"), QVariant::fromValue(cryptoProto));
    c.insert(QStringLiteral("block"), &block);

    block.setProperty("inProgress", metaData.inProgress);
    block.setProperty("errorText", metaData.errorText);

    block.setProperty("detailHeader", showSignatureDetails());
    block.setProperty("isPrinting", isPrinting());
    block.setProperty("addr", metaData.signerMailAddresses.join(QLatin1Char(',')));
    block.setProperty("technicalProblem", metaData.technicalProblem);
    block.setProperty("keyId", metaData.keyId);
    if (metaData.creationTime.isValid()) { // should be handled inside grantlee but currently not possible see: https://bugs.kde.org/363475
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
            startKeyHREF = QStringLiteral("<a href=\"kmail:showCertificate#%1 ### %2 ### %3\">")
                               .arg(cryptoProto->displayName(), cryptoProto->name(), QString::fromLatin1(metaData.keyId));

            keyWithWithoutURL = QStringLiteral("%1%2</a>").arg(startKeyHREF, QString::fromLatin1(QByteArray(QByteArrayLiteral("0x") + metaData.keyId)));
        } else {
            keyWithWithoutURL = QLatin1String("0x") + QString::fromUtf8(metaData.keyId);
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
        statusStr = sigStatusToString(cryptoProto, metaData.status_code, metaData.sigSummary, frameColor, showKeyInfos);
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
                QString msgFrom(KEmailAddress::extractEmailAddress(mp->fromAddress()));
                QString certificate;
                if (metaData.keyId.isEmpty()) {
                    certificate = i18n("certificate");
                } else {
                    certificate = startKeyHREF + i18n("certificate") + QStringLiteral("</a>");
                }

                if (!blockAddrs.empty()) {
                    if (!blockAddrs.contains(msgFrom, Qt::CaseInsensitive)) {
                        greenCaseWarning = QStringLiteral("<u>") + i18nc("Start of warning message.", "Warning:") + QStringLiteral("</u> ")
                            + i18n("Sender's mail address is not stored in the %1 used for signing.", certificate) + QStringLiteral("<br />") + i18n("sender: ")
                            + msgFrom + QStringLiteral("<br />") + i18n("stored: ");
                        // We cannot use Qt's join() function here but
                        // have to join the addresses manually to
                        // extract the mail addresses (without '<''>')
                        // before including it into our string:
                        bool bStart = true;
                        QStringList::ConstIterator end(blockAddrs.constEnd());
                        for (QStringList::ConstIterator it = blockAddrs.constBegin(); it != end; ++it) {
                            if (!bStart) {
                                greenCaseWarning.append(QLatin1String(", <br />&nbsp; &nbsp;"));
                            }

                            bStart = false;
                            greenCaseWarning.append(KEmailAddress::extractEmailAddress(*it));
                        }
                    }
                } else {
                    greenCaseWarning = QStringLiteral("<u>") + i18nc("Start of warning message.", "Warning:") + QStringLiteral("</u> ")
                        + i18n("No mail address is stored in the %1 used for signing, "
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
                        signer = QStringLiteral("<a href=\"mailto:%1\">%2</a>").arg(QLatin1String(QUrl ::toPercentEncoding(address.path())), signer);
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

    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

void DefaultRendererPrivate::render(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const auto metaData = *mp->partMetaData();
    if (metaData.isSigned || metaData.inProgress) {
        HTMLBlock::Ptr aBlock;
        if (mp->isAttachment()) {
            aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
        }
        renderSigned(mp, htmlWriter);
        return;
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    if (mp->hasSubParts()) {
        renderSubParts(mp, htmlWriter);
    } else if (!metaData.inProgress) {
        renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const auto metaData = *mp->partMetaData();

    if (metaData.isEncrypted || metaData.inProgress) {
        HTMLBlock::Ptr aBlock;
        if (mp->isAttachment()) {
            aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
        }
        renderEncrypted(mp, htmlWriter);
        return;
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    if (mp->hasSubParts()) {
        renderSubParts(mp, htmlWriter);
    } else if (!metaData.inProgress) {
        renderWithFactory<MimeTreeParser::MessagePart>(mp, htmlWriter);
    }
}

void DefaultRendererPrivate::render(const AlternativeMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }

    auto mode = mp->preferredMode();
    if (mode == MimeTreeParser::Util::MultipartPlain && mp->text().trimmed().isEmpty()) {
        const auto availableModes = mp->availableModes();
        for (const auto m : availableModes) {
            if (m != MimeTreeParser::Util::MultipartPlain) {
                mode = m;
                break;
            }
        }
    }
    MimeMessagePart::Ptr part(mp->childParts().first());
    if (mp->childParts().contains(mode)) {
        part = mp->childParts()[mode];
    }

    render(part, htmlWriter);
}

void DefaultRendererPrivate::render(const CertMessagePart::Ptr &mp, HtmlWriter *htmlWriter)
{
    const GpgME::ImportResult &importResult(mp->importResult());
    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("certmessagepart.html"));
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
        auto key(new QObject(mp.data()));
        key->setProperty("error", QString::fromLocal8Bit((*it).error().asString()));
        key->setProperty("status", (*it).status());
        key->setProperty("fingerprint", QLatin1String((*it).fingerprint()));
        keylist << QVariant::fromValue(key);
    }

    HTMLBlock::Ptr aBlock;
    if (mp->isAttachment()) {
        aBlock = HTMLBlock::Ptr(new AttachmentMarkBlock(htmlWriter, mp->attachmentContent()));
    }
    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
}

bool DefaultRendererPrivate::renderWithFactory(const QMetaObject *mo, const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    if (!mRendererFactory) {
        return false;
    }
    for (auto r : mRendererFactory->renderersForPart(mo, msgPart)) {
        if (r->render(msgPart, htmlWriter, this)) {
            return true;
        }
    }
    return false;
}

void DefaultRendererPrivate::renderFactory(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter)
{
    const QString className = QString::fromUtf8(msgPart->metaObject()->className());

    if (isHiddenHint(msgPart)) {
        const QByteArray cid = msgPart->content()->contentID()->identifier();
        auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
        if (!cid.isEmpty() && mp) {
            QString fileName = mp->temporaryFilePath();
            QString href = QUrl::fromLocalFile(fileName).url();
            htmlWriter->embedPart(cid, href);
        }
    }

    if (renderWithFactory(msgPart, htmlWriter)) {
        return;
    }

    if (className == QLatin1String("MimeTreeParser::MessagePartList")) {
        auto mp = msgPart.dynamicCast<MessagePartList>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::MimeMessagePart")) {
        auto mp = msgPart.dynamicCast<MimeMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::EncapsulatedRfc822MessagePart")) {
        auto mp = msgPart.dynamicCast<EncapsulatedRfc822MessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::HtmlMessagePart")) {
        auto mp = msgPart.dynamicCast<HtmlMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::SignedMessagePart")) {
        auto mp = msgPart.dynamicCast<SignedMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::EncryptedMessagePart")) {
        auto mp = msgPart.dynamicCast<EncryptedMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::AlternativeMessagePart")) {
        auto mp = msgPart.dynamicCast<AlternativeMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else if (className == QLatin1String("MimeTreeParser::CertMessagePart")) {
        auto mp = msgPart.dynamicCast<CertMessagePart>();
        if (mp) {
            render(mp, htmlWriter);
        }
    } else {
        qCWarning(MESSAGEVIEWER_LOG) << "We got a unknown classname, using default behaviour for " << className;
    }
}

bool DefaultRendererPrivate::isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart)
{
    auto mp = msgPart.dynamicCast<MimeTreeParser::MessagePart>();
    auto content = msgPart->content();

    if (!mp || !content) {
        return false;
    }

    if (mShowOnlyOneMimePart && mMsgPart.data() == msgPart->parentPart()) {
        if (mMsgPart->subParts().at(0) == msgPart.data()) {
            return false;
        }
    }

    if (msgPart->nodeHelper()->isNodeDisplayedHidden(content)) {
        return true;
    }

    const AttachmentStrategy *const as = mAttachmentStrategy;
    const bool defaultHidden(as && as->defaultDisplay(content) == AttachmentStrategy::None);
    auto preferredMode = source()->preferredMode();
    bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);

    QByteArray mediaType("text");
    if (content->contentType(false) && !content->contentType(false)->mediaType().isEmpty() && !content->contentType(false)->subType().isEmpty()) {
        mediaType = content->contentType(false)->mediaType();
    }
    const bool isTextPart = (mediaType == QByteArrayLiteral("text"));

    bool defaultAsIcon = true;
    if (!mp->neverDisplayInline()) {
        if (as) {
            defaultAsIcon = as->defaultDisplay(content) == AttachmentStrategy::AsIcon;
        }
    }

    // neither image nor text -> show as icon
    if (!mp->isImage() && !isTextPart) {
        defaultAsIcon = true;
    }

    bool hidden(false);
    if (isTextPart) {
        hidden = defaultHidden;
    } else {
        if (mp->isImage() && isHtmlPreferred && content->parent() && content->parent()->contentType(false)->subType() == "related") {
            hidden = true;
        } else {
            hidden = defaultHidden && content->parent();
            hidden |= defaultAsIcon && defaultHidden;
        }
    }
    msgPart->nodeHelper()->setNodeDisplayedHidden(content, hidden);
    return hidden;
}

MimeTreeParser::IconType DefaultRendererPrivate::displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart)
{
    auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
    auto content = msgPart->content();

    if (!content || !mp) {
        return MimeTreeParser::IconType::NoIcon;
    }

    const AttachmentStrategy *const as = mAttachmentStrategy;
    const bool defaultDisplayHidden(as && as->defaultDisplay(content) == AttachmentStrategy::None);
    const bool defaultDisplayInline(as && as->defaultDisplay(content) == AttachmentStrategy::Inline);
    const bool defaultDisplayAsIcon(as && as->defaultDisplay(content) == AttachmentStrategy::AsIcon);
    const bool showOnlyOneMimePart(mShowOnlyOneMimePart);
    auto preferredMode = source()->preferredMode();
    bool isHtmlPreferred = (preferredMode == MimeTreeParser::Util::Html) || (preferredMode == MimeTreeParser::Util::MultipartHtml);

    QByteArray mediaType("text");
    if (content->contentType(false) && !content->contentType(false)->mediaType().isEmpty() && !content->contentType(false)->subType().isEmpty()) {
        mediaType = content->contentType(false)->mediaType();
    }
    const bool isTextPart = (mediaType == QByteArrayLiteral("text"));

    bool defaultAsIcon = true;
    if (!mp->neverDisplayInline()) {
        if (as) {
            defaultAsIcon = defaultDisplayAsIcon;
        }
    }
    if (mp->isImage() && showOnlyOneMimePart && !mp->neverDisplayInline()) {
        defaultAsIcon = false;
    }

    // neither image nor text -> show as icon
    if (!mp->isImage() && !isTextPart) {
        defaultAsIcon = true;
    }

    if (isTextPart) {
        if (as && !defaultDisplayInline) {
            return MimeTreeParser::IconExternal;
        }
        return MimeTreeParser::NoIcon;
    } else {
        if (mp->isImage() && isHtmlPreferred && content->parent() && content->parent()->contentType(false)->subType() == "related") {
            return MimeTreeParser::IconInline;
        }

        if (defaultDisplayHidden && !showOnlyOneMimePart && content->parent()) {
            return MimeTreeParser::IconInline;
        }

        if (defaultAsIcon) {
            return MimeTreeParser::IconExternal;
        } else if (mp->isImage()) {
            return MimeTreeParser::IconInline;
        }
    }

    return MimeTreeParser::NoIcon;
}

bool DefaultRendererPrivate::showEmoticons() const
{
    return mShowEmoticons;
}

bool DefaultRendererPrivate::isPrinting() const
{
    return mIsPrinting;
}

bool DefaultRendererPrivate::htmlLoadExternal() const
{
    return mHtmlLoadExternal;
}

bool DefaultRendererPrivate::showExpandQuotesMark() const
{
    return mShowExpandQuotesMark;
}

bool DefaultRendererPrivate::showOnlyOneMimePart() const
{
    return mShowOnlyOneMimePart;
}

bool DefaultRendererPrivate::showSignatureDetails() const
{
    return mShowSignatureDetails;
}

bool DefaultRendererPrivate::showEncryptionDetails() const
{
    return mShowEncryptionDetails;
}

int DefaultRendererPrivate::levelQuote() const
{
    return mLevelQuote;
}

DefaultRenderer::DefaultRenderer(CSSHelperBase *cssHelper)
    : d(new DefaultRendererPrivate(cssHelper, MessagePartRendererFactory::instance()))
{
}

DefaultRenderer::~DefaultRenderer()
{
    delete d;
}

void DefaultRenderer::setShowOnlyOneMimePart(bool onlyOneMimePart)
{
    d->mShowOnlyOneMimePart = onlyOneMimePart;
}

void DefaultRenderer::setAttachmentStrategy(const AttachmentStrategy *strategy)
{
    d->mAttachmentStrategy = strategy;
}

void DefaultRenderer::setShowEmoticons(bool showEmoticons)
{
    d->mShowEmoticons = showEmoticons;
}

void DefaultRenderer::setIsPrinting(bool isPrinting)
{
    d->mIsPrinting = isPrinting;
}

void DefaultRenderer::setShowExpandQuotesMark(bool showExpandQuotesMark)
{
    d->mShowExpandQuotesMark = showExpandQuotesMark;
}

void DefaultRenderer::setShowEncryptionDetails(bool showEncryptionDetails)
{
    d->mShowEncryptionDetails = showEncryptionDetails;
}

void DefaultRenderer::setShowSignatureDetails(bool showSignatureDetails)
{
    d->mShowSignatureDetails = showSignatureDetails;
}

void DefaultRenderer::setLevelQuote(int levelQuote)
{
    d->mLevelQuote = levelQuote;
}

void DefaultRenderer::setHtmlLoadExternal(bool htmlLoadExternal)
{
    d->mHtmlLoadExternal = htmlLoadExternal;
}

void DefaultRenderer::setCreateMessageHeader(const std::function<QString(KMime::Message *)> &createMessageHeader)
{
    d->mCreateMessageHeader = createMessageHeader;
}

QString renderTreeHelper(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    QString ret = QStringLiteral("%1 * %3\n").arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    indent += QLatin1Char(' ');
    for (const auto &subPart : messagePart->subParts()) {
        ret += renderTreeHelper(subPart, indent);
    }
    return ret;
}

void DefaultRenderer::render(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer)
{
    qCDebug(MESSAGEVIEWER_LOG) << "MimeTreeParser structure:";
    qCDebug(MESSAGEVIEWER_LOG) << qPrintable(renderTreeHelper(msgPart, QString()));
    d->mMsgPart = msgPart;
    d->renderFactory(d->mMsgPart, writer);
}
