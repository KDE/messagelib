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
#include "objecttreeparser.h"
#include "converthtmltoplaintext.h"
#include "csshelperbase.h"
#include "cryptohelper.h"
#include "interfaces/htmlwriter.h"
#include "job/kleojobexecutor.h"
#include "utils/iconnamecache.h"

#include <MessageCore/StringUtil>

#include <libkleo/importjob.h>
#include <libkleo/cryptobackendfactory.h>

#include <KMime/Content>

#include <gpgme++/key.h>
#include <gpgme.h>

#include <QTextCodec>
#include <QApplication>
#include <QWebPage>
#include <QWebElement>
#include <QWebFrame>

#include <KLocalizedString>
#include <KEmailAddress>

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

QString HTMLBlock::dir() const
{
    return QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
}

//--------CryptoBlock-------------------
CryptoBlock::CryptoBlock(ObjectTreeParser *otp,
                         PartMetaData *block,
                         const Kleo::CryptoBackend::Protocol *cryptoProto,
                         ObjectTreeSourceIf *source,
                         const QString &fromAddress,
                         KMime::Content *node)
    : HTMLBlock()
    , mOtp(otp)
    , mMetaData(block)
    , mCryptoProto(cryptoProto)
    , mSource(source)
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
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();
    if (writer && !entered) {
        entered = true;
        if (mMetaData->isEncapsulatedRfc822Message) {
            mInteralBlocks.append(HTMLBlock::Ptr(new EncapsulatedRFC822Block(writer, mOtp->nodeHelper(), mNode)));
        }
        if (mMetaData->isEncrypted) {
            mInteralBlocks.append(HTMLBlock::Ptr(new EncryptedBlock(writer, *mMetaData)));
        }
        if (mMetaData->isSigned) {
            mInteralBlocks.append(HTMLBlock::Ptr(new SignedBlock(writer, *mMetaData, mCryptoProto, mSource, mFromAddress, false)));
        }
    }
}

void CryptoBlock::internalExit()
{
    if (!entered) {
        return;
    }

    while (!mInteralBlocks.isEmpty()) {
        mInteralBlocks.removeLast();
    }
    entered = false;
}

EncapsulatedRFC822Block::EncapsulatedRFC822Block(MimeTreeParser::HtmlWriter *writer, MimeTreeParser::NodeHelper *nodeHelper, KMime::Content *node)
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
        QString text;
        if (mNode) {
            const QString href = mNodeHelper->asHREF(mNode, QStringLiteral("body"));
            text = QStringLiteral("<a href=\"%1\">%2</a>").arg(href, i18n("Encapsulated message"));
        } else {
            text = i18n("Encapsulated message");
        }
        mWriter->queue(QStringLiteral("<table cellspacing=\"1\" cellpadding=\"1\" class=\"rfc822\">") +
                       QStringLiteral("<tr class=\"rfc822H\"><td dir=\"%1\">").arg(dir()) +
                       text +
                       QStringLiteral("</td></tr><tr class=\"rfc822B\"><td>"));

        entered = true;
    }
}

void EncapsulatedRFC822Block::internalExit()
{
    if (!entered) {
        return;
    }

    mWriter->queue(QStringLiteral("</td></tr>"
                                  "<tr class=\"rfc822H\"><td dir=\"%1\">%2</td></tr>"
                                  "</table>").arg(dir(), i18n("End of encapsulated message")));
    entered = false;
}

EncryptedBlock::EncryptedBlock(MimeTreeParser::HtmlWriter *writer, const PartMetaData &block)
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
        QString text;
        if (mBlock.inProgress) {
            text = i18n("Please wait while the message is being decrypted...");
        } else if (mBlock.isDecryptable) {
            text = i18n("Encrypted message");
        } else {
            text = i18n("Encrypted message (decryption not possible)");
            if (!mBlock.errorText.isEmpty()) {
                text += QStringLiteral("<br />") + i18n("Reason: %1", mBlock.errorText);
            }
        }
        mWriter->queue(QStringLiteral("<table cellspacing=\"1\" cellpadding=\"1\" class=\"encr\">") +
                       QStringLiteral("<tr class=\"encrH\"><td dir=\"%1\">").arg(dir()) +
                       text +
                       QStringLiteral("</td></tr><tr class=\"encrB\"><td>"));
    }
}

void EncryptedBlock::internalExit()
{
    if (!entered) {
        return;
    }
    mWriter->queue(QStringLiteral("</td></tr>"
                                  "<tr class=\"encrH\"><td dir=\"%1\">%2</td></tr>"
                                  "</table>").arg(dir(), i18n("End of encrypted message")));
    entered = false;
}

SignedBlock::SignedBlock(MimeTreeParser::HtmlWriter *writer, const PartMetaData &block,
                         const Kleo::CryptoBackend::Protocol *cryptoProto,
                         ObjectTreeSourceIf *source,
                         QString fromAddress, bool printing)
    : HTMLBlock()
    , mBlock(block)
    , mWriter(writer)
    , mCryptoProto(cryptoProto)
    , mSource(source)
    , mFromAddress(fromAddress)
    , mPrinting(printing)
{
    internalEnter();
}

SignedBlock::~SignedBlock()
{
    internalExit();
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

QString SignedBlock::simpleHeader()
{
    QString text;
    if (mClass == QStringLiteral("signErr")) {
        text = i18n("Invalid signature.");
    } else if (mClass == QStringLiteral("signOkKeyBad")
               || mClass == QStringLiteral("signWarn")) {
        text = i18n("Not enough information to check signature validity.");
    } else if (mClass == QStringLiteral("signOkKeyOk")) {

        QString addr;
        if (!mBlock.signerMailAddresses.isEmpty()) {
            addr = mBlock.signerMailAddresses.first();
        }

        QString name = addr;
        if (name.isEmpty()) {
            name = mBlock.signer;
        }

        if (addr.isEmpty()) {
            text = i18n("Signature is valid.");
        } else {
            text = i18n("Signed by <a href=\"mailto:%1\">%2</a>.", addr, name);
        }

    } else {
        // should not happen
        text = i18n("Unknown signature state");
    }

    QString html;
    html += QStringLiteral("<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\"><tr>");
    html += QStringLiteral("<td>%1</td>").arg(text);
    if (!mPrinting) {
        html += QStringLiteral("<td align=\"right\">");
        html += QStringLiteral("<a href=\"kmail:showSignatureDetails\">%1</a>").arg(i18n("Show Details"));
        html += QStringLiteral("</td>");
    }
    html += QStringLiteral("</tr></table>");
    return html;
}

static QString beginVerboseSigstatHeader()
{
    return QStringLiteral("<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\"><tr><td rowspan=\"2\">");
}

static QString makeShowAuditLogLink(const GpgME::Error &err, const QString &auditLog)
{
    // more or less the same as
    // kleopatra/utils/auditlog.cpp:formatLink(), so any bug fixed here
    // equally applies there:
    if (const unsigned int code = err.code()) {
        if (code == GPG_ERR_NOT_IMPLEMENTED) {
            qCDebug(MIMETREEPARSER_LOG) << "not showing link (not implemented)";
            return QString();
        } else if (code == GPG_ERR_NO_DATA) {
            qCDebug(MIMETREEPARSER_LOG) << "not showing link (not available)";
            return i18n("No Audit Log available");
        } else {
            return i18n("Error Retrieving Audit Log: %1", QString::fromLocal8Bit(err.asString()));
        }
    }

    if (!auditLog.isEmpty()) {
        QUrl url;
        url.setScheme(QStringLiteral("kmail"));
        url.setPath(QStringLiteral("showAuditLog"));
        QUrlQuery urlquery(url);
        urlquery.addQueryItem(QStringLiteral("log"), auditLog);
        url.setQuery(urlquery);
        return QStringLiteral("<a href=\"%1\">%2</a>").arg(url.toDisplayString(), i18nc("The Audit Log is a detailed error log from the gnupg backend", "Show Audit Log"));
    }

    return QString();
}

static QString endVerboseSigstatHeader(const PartMetaData &pmd)
{
    QString html;
    html += QStringLiteral("</td><td align=\"right\" valign=\"top\" nowrap=\"nowrap\">");
    html += QStringLiteral("<a href=\"kmail:hideSignatureDetails\">");
    html += i18n("Hide Details");
    html += QStringLiteral("</a></td></tr>");
    html += QStringLiteral("<tr><td align=\"right\" valign=\"bottom\" nowrap=\"nowrap\">");
    html += makeShowAuditLogLink(pmd.auditLogError, pmd.auditLog);
    html += QStringLiteral("</td></tr></table>");
    return html;
}

void SignedBlock::internalEnter()
{
    if (mWriter && !entered) {
        entered = true;
        const bool isSMIME = mCryptoProto && (mCryptoProto == Kleo::CryptoBackendFactory::instance()->smime());
        QString signer = mBlock.signer;

        QString htmlStr, simpleHtmlStr;

        if (mBlock.inProgress) {
            mClass = QStringLiteral("signInProgress");
            mWriter->queue(QStringLiteral("<table cellspacing=\"1\" cellpadding=\"1\" class=\"signInProgress\">") +
                           QStringLiteral("<tr class=\"signInProgressH\"><td dir=\"%1\">").arg(dir()) +
                           i18n("Please wait while the signature is being verified...") +
                           QStringLiteral("</td></tr><tr class=\"signInProgressB\"><td>"));
            return;
        } else {
            const QStringList &blockAddrs(mBlock.signerMailAddresses);
            // note: At the moment frameColor and showKeyInfos are
            //       used for CMS only but not for PGP signatures
            // pending(khz): Implement usage of these for PGP sigs as well.
            int frameColor = SIG_FRAME_COL_UNDEF;
            bool showKeyInfos;
            bool onlyShowKeyURL = false;
            bool cannotCheckSignature = true;
            QString statusStr = sigStatusToString(mCryptoProto,
                                                  mBlock.status_code,
                                                  mBlock.sigSummary,
                                                  frameColor,
                                                  showKeyInfos);
            // if needed fallback to english status text
            // that was reported by the plugin
            if (statusStr.isEmpty()) {
                statusStr = mBlock.status;
            }
            if (mBlock.technicalProblem) {
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

            // compose the string for displaying the key ID
            // either as URL or not linked (for unknown crypto)
            // note: Once we can start PGP key manager programs
            //       from within KMail we could change this and
            //       always show the URL.    (khz, 2002/06/27)
            QString startKeyHREF;
            QString keyWithWithoutURL;
            if (mCryptoProto) {
                startKeyHREF =
                    QStringLiteral("<a href=\"kmail:showCertificate#%1 ### %2 ### %3\">")
                    .arg(mCryptoProto->displayName(),
                         mCryptoProto->name(),
                         QString::fromLatin1(mBlock.keyId));

                keyWithWithoutURL =
                    QStringLiteral("%1%2</a>").arg(startKeyHREF, QString::fromLatin1(QByteArray(QByteArray("0x") + mBlock.keyId)));
            } else {
                keyWithWithoutURL = QStringLiteral("0x") + QString::fromUtf8(mBlock.keyId);
            }

            // temporary hack: always show key information!
            showKeyInfos = true;

            if (isSMIME && (SIG_FRAME_COL_UNDEF != frameColor)) {

                // new frame settings for CMS:
                // beautify the status string
                if (!statusStr.isEmpty()) {
                    statusStr.prepend(QStringLiteral("<i>"));
                    statusStr.append(QStringLiteral("</i>"));
                }

                // special color handling: S/MIME uses only green/yellow/red.
                switch (frameColor) {
                case SIG_FRAME_COL_RED:
                    mClass = QStringLiteral("signErr");//"signCMSRed";
                    onlyShowKeyURL = true;
                    break;
                case SIG_FRAME_COL_YELLOW:
                    if (mBlock.technicalProblem) {
                        mClass = QStringLiteral("signWarn");
                    } else {
                        mClass = QStringLiteral("signOkKeyBad");    //"signCMSYellow";
                    }
                    break;
                case SIG_FRAME_COL_GREEN:
                    mClass = QStringLiteral("signOkKeyOk");//"signCMSGreen";
                    // extra hint for green case
                    // that email addresses in DN do not match fromAddress
                    QString greenCaseWarning;
                    QString msgFrom(KEmailAddress::extractEmailAddress(mFromAddress));
                    QString certificate;
                    if (mBlock.keyId.isEmpty()) {
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
                            for (QStringList::ConstIterator it = blockAddrs.constBegin();
                                    it != blockAddrs.constEnd(); ++it) {
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
                    if (!greenCaseWarning.isEmpty()) {
                        if (!statusStr.isEmpty()) {
                            statusStr.append(QStringLiteral("<br />&nbsp;<br />"));
                        }
                        statusStr.append(greenCaseWarning);
                    }
                    break;
                }

                QString frame = QStringLiteral("<table cellspacing=\"1\" cellpadding=\"1\" class=\"%1\">").arg(mClass) +
                                QStringLiteral("<tr class=\"%1H\"><td dir=\"%2\">").arg(mClass, dir());
                htmlStr += frame + beginVerboseSigstatHeader();
                simpleHtmlStr += frame;
                simpleHtmlStr += simpleHeader();
                if (mBlock.technicalProblem) {
                    htmlStr += mBlock.errorText;
                } else if (showKeyInfos) {
                    if (cannotCheckSignature) {
                        htmlStr += i18n("Not enough information to check "
                                        "signature. %1",
                                        keyWithWithoutURL);
                    } else {

                        if (mBlock.signer.isEmpty()) {
                            signer.clear();
                        } else {
                            if (!blockAddrs.empty()) {
                                const QUrl address = KEmailAddress::encodeMailtoUrl(blockAddrs.first());
                                signer = QStringLiteral("<a href=\"mailto:%1\">%2</a>").arg(QLatin1String(QUrl::toPercentEncoding(address.path())), signer);
                            }
                        }

                        if (mBlock.keyId.isEmpty()) {
                            if (signer.isEmpty() || onlyShowKeyURL) {
                                htmlStr += i18n("Message was signed with unknown key.");
                            } else {
                                htmlStr += i18n("Message was signed by %1.", signer);
                            }
                        } else {
                            QDateTime created = mBlock.creationTime;
                            if (created.isValid()) {
                                if (signer.isEmpty()) {
                                    if (onlyShowKeyURL) {
                                        htmlStr += i18n("Message was signed with key %1.", keyWithWithoutURL);
                                    } else
                                        htmlStr += i18n("Message was signed on %1 with key %2.",
                                                        QLocale::system().toString(created, QLocale::ShortFormat),
                                                        keyWithWithoutURL);
                                } else {
                                    if (onlyShowKeyURL) {
                                        htmlStr += i18n("Message was signed with key %1.", keyWithWithoutURL);
                                    } else
                                        htmlStr += i18n("Message was signed by %3 on %1 with key %2",
                                                        QLocale::system().toString(created, QLocale::ShortFormat),
                                                        keyWithWithoutURL,
                                                        signer);
                                }
                            } else {
                                if (signer.isEmpty() || onlyShowKeyURL) {
                                    htmlStr += i18n("Message was signed with key %1.", keyWithWithoutURL);
                                } else
                                    htmlStr += i18n("Message was signed by %2 with key %1.",
                                                    keyWithWithoutURL,
                                                    signer);
                            }
                        }
                    }
                    htmlStr += QStringLiteral("<br />");
                    if (!statusStr.isEmpty()) {
                        htmlStr += QStringLiteral("&nbsp;<br />");
                        htmlStr += i18n("Status: ");
                        htmlStr += statusStr;
                    }
                } else {
                    htmlStr += statusStr;
                }
                frame = QStringLiteral("</td></tr><tr class=\"%1B\"><td>").arg(mClass);
                htmlStr += endVerboseSigstatHeader(mBlock) + frame;
                simpleHtmlStr += frame;

            } else {

                QString content;
                // old frame settings for PGP:

                if (mBlock.signer.isEmpty() || mBlock.technicalProblem) {
                    mClass = QStringLiteral("signWarn");

                    if (mBlock.technicalProblem) {
                        content = mBlock.errorText;
                    } else {
                        if (!mBlock.keyId.isEmpty()) {
                            QDateTime created = mBlock.creationTime;
                            if (created.isValid())
                                content = i18n("Message was signed on %1 with unknown key %2.",
                                               QLocale::system().toString(created, QLocale::ShortFormat),
                                               keyWithWithoutURL);
                            else
                                content = i18n("Message was signed with unknown key %1.",
                                               keyWithWithoutURL);
                        } else {
                            content = i18n("Message was signed with unknown key.");
                        }
                        content += QStringLiteral("<br />");
                        content += i18n("The validity of the signature cannot be "
                                        "verified.");
                        if (!statusStr.isEmpty()) {
                            content += QStringLiteral("<br />");
                            content += i18n("Status: ");
                            content += QStringLiteral("<i>");
                            content += statusStr;
                            content += QStringLiteral("</i>");
                        }
                    }
                } else {
                    // HTMLize the signer's user id and create mailto: link
                    signer = MessageCore::StringUtil::quoteHtmlChars(signer, true);
                    signer = QStringLiteral("<a href=\"mailto:%1\">%1</a>").arg(signer);

                    if (mBlock.isGoodSignature) {
                        if (mBlock.keyTrust < GpgME::Signature::Marginal) {
                            mClass = QStringLiteral("signOkKeyBad");
                        } else {
                            mClass = QStringLiteral("signOkKeyOk");
                        }

                        if (!mBlock.keyId.isEmpty())
                            content = i18n("Message was signed by %2 (Key ID: %1).",
                                           keyWithWithoutURL,
                                           signer);
                        else {
                            content = i18n("Message was signed by %1.", signer);
                        }
                        content += QStringLiteral("<br />");

                        switch (mBlock.keyTrust) {
                        case GpgME::Signature::Unknown:
                            content += i18n("The signature is valid, but the key's "
                                            "validity is unknown.");
                            break;
                        case GpgME::Signature::Marginal:
                            content += i18n("The signature is valid and the key is "
                                            "marginally trusted.");
                            break;
                        case GpgME::Signature::Full:
                            content += i18n("The signature is valid and the key is "
                                            "fully trusted.");
                            break;
                        case GpgME::Signature::Ultimate:
                            content += i18n("The signature is valid and the key is "
                                            "ultimately trusted.");
                            break;
                        default:
                            content += i18n("The signature is valid, but the key is "
                                            "untrusted.");
                        }
                    } else {
                        mClass = QStringLiteral("signErr");

                        if (!mBlock.keyId.isEmpty())
                            content = i18n("Message was signed by %2 (Key ID: %1).",
                                           keyWithWithoutURL,
                                           signer);
                        else {
                            content = i18n("Message was signed by %1.", signer);
                        }
                        content += QStringLiteral("<br />");
                        content += i18n("Warning: The signature is bad.");
                    }
                }
                const QString beginFrame = QStringLiteral("<table cellspacing=\"1\" cellpadding=\"1\" class=\"%1\">").arg(mClass) +
                                           QStringLiteral("<tr class=\"%1H\"><td dir=\"%2\">").arg(mClass, dir());
                const QString endFrame = QStringLiteral("</td></tr><tr class=\"%1B\"><td>").arg(mClass);

                htmlStr += beginFrame + beginVerboseSigstatHeader();
                htmlStr += content;
                htmlStr += endVerboseSigstatHeader(mBlock) + endFrame;

                simpleHtmlStr += beginFrame;
                simpleHtmlStr += simpleHeader();
                simpleHtmlStr += endFrame;
            }
        }

        if (mSource->showSignatureDetails()) {
            mWriter->queue(htmlStr);
        }
        mWriter->queue(simpleHtmlStr);
    }
}

void SignedBlock::internalExit()
{
    if (!entered) {
        return;
    }

    mWriter->queue(QStringLiteral("</td></tr>"
                                  "<tr class=\"%1H\"><td dir=\"%2\">%3</td></tr>"
                                  "</table>").arg(mClass, dir(), i18n("End of signed message")));
    entered = false;
}

AttachmentMarkBlock::AttachmentMarkBlock(MimeTreeParser::HtmlWriter *writer, KMime::Content *node)
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

TextBlock::TextBlock(MimeTreeParser::HtmlWriter *writer, MimeTreeParser::NodeHelper *nodeHelper, KMime::Content *node, bool link)
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

    mWriter->queue(QLatin1String("<table cellspacing=\"1\" class=\"textAtm\">"
                                 "<tr class=\"textAtmH\"><td dir=\"") + dir() + QLatin1String("\">"));
    if (!mLink)
        mWriter->queue(QLatin1String("<a href=\"") + mNodeHelper->asHREF(mNode, QStringLiteral("body")) + QLatin1String("\">")
                       + label + QLatin1String("</a>"));
    else {
        mWriter->queue(label);
    }
    if (!comment.isEmpty()) {
        mWriter->queue(QLatin1String("<br/>") + comment);
    }
    mWriter->queue(QStringLiteral("</td></tr><tr class=\"textAtmB\"><td>"));
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
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    const CryptoBlock block(mOtp, &mMetaData, mOtp->cryptoProtocol(), mOtp->mSource, QString(), Q_NULLPTR);
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
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    if (mAsIcon == MimeTreeParser::NoIcon) {
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

        if (blocks.count() > 1 || blocks.at(0).type() != MimeTreeParser::NoPgpBlock) {
            mOtp->setCryptoProtocol(cryptProto);
        }

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
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (mDrawFrame) {
        block = HTMLBlock::Ptr(new TextBlock(writer, mOtp->nodeHelper(), mNode, mShowLink));
    }

    if (mAsIcon != MimeTreeParser::NoIcon) {
        mOtp->writePartIcon(mNode, (mAsIcon == MimeTreeParser::IconInline));
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

void CertMessagePart::html(bool decorate)
{
    Q_UNUSED(decorate);
    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

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

// A small class that eases temporary CryptPlugWrapper changes:
class CryptoProtocolSaver
{
    ObjectTreeParser *otp;
    const Kleo::CryptoBackend::Protocol *protocol;
public:
    CryptoProtocolSaver(ObjectTreeParser *_otp, const Kleo::CryptoBackend::Protocol *_w)
        : otp(_otp), protocol(_otp ? _otp->cryptoProtocol() : 0)
    {
        if (otp) {
            otp->setCryptoProtocol(_w);
        }
    }

    ~CryptoProtocolSaver()
    {
        if (otp) {
            otp->setCryptoProtocol(protocol);
        }
    }
};

//-----CryptMessageBlock---------------------
CryptoMessagePart::CryptoMessagePart(ObjectTreeParser *otp,
                                     const QString &text,
                                     const Kleo::CryptoBackend::Protocol *cryptoProto,
                                     const QString &fromAddress,
                                     KMime::Content *node)
    : MessagePart(otp, text)
    , mPassphraseError(false)
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

    CryptoProtocolSaver saver(mOtp, mCryptoProto);
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

    CryptoProtocolSaver saver(mOtp, mCryptoProto);
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
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress, mNode);
        writeDeferredDecryptionBlock();
    } else if (mMetaData.inProgress) {
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress, mNode);
        // In progress has no special body
    } else if (mMetaData.isEncrypted && !mMetaData.isDecryptable) {
        const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress, mNode);
        writer->queue(text());           //Do not quote ErrorText
    } else {
        if (mMetaData.isSigned && mVerifiedText.isEmpty() && !hideErrors) {
            const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress, mNode);
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
            const CryptoBlock block(mOtp, &mMetaData, mCryptoProto, mOtp->mSource, mFromAddress, mNode);
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
    if (!mSubMessagePart) {
        return;
    }

    MimeTreeParser::HtmlWriter *writer = mOtp->htmlWriter();

    if (!writer) {
        return;
    }

    const HTMLBlock::Ptr aBlock(attachmentBlock());

    const CryptoBlock block(mOtp, &mMetaData, Q_NULLPTR, Q_NULLPTR, mMessage->from()->asUnicodeString(), mMessage.data());
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
