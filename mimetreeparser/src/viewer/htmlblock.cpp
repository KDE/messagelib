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

#include "htmlblock.h"
#include "mimetreeparser_debug.h"

#include "interfaces/objecttreesource.h"
#include "interfaces/htmlwriter.h"
#include "nodehelper.h"

#include <MessageCore/StringUtil>
#include <KMime/Content>

#include <gpgme.h>
#include <Libkleo/CryptoBackendFactory>

#include <KLocalizedString>
#include <KEmailAddress>

#include <QApplication>

using namespace MimeTreeParser;

HTMLBlock::~HTMLBlock()
{
}

QString HTMLBlock::dir() const
{
    return QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
}

//--------CryptoBlock-------------------
CryptoBlock::CryptoBlock(HtmlWriter *writer,
                         PartMetaData *block,
                         const Kleo::CryptoBackend::Protocol *cryptoProto,
                         Interface::ObjectTreeSource *source,
                         const QString &fromAddress)
    : HTMLBlock()
    , mWriter(writer)
    , mMetaData(block)
    , mCryptoProto(cryptoProto)
    , mSource(source)
    , mFromAddress(fromAddress)
{
    internalEnter();
}

CryptoBlock::~CryptoBlock()
{
    internalExit();
}

void CryptoBlock::internalEnter()
{
    if (mWriter && !entered) {
        entered = true;
        if (mMetaData->isEncrypted) {
            mInteralBlocks.append(HTMLBlock::Ptr(new EncryptedBlock(mWriter, *mMetaData)));
        }
        if (mMetaData->isSigned) {
            mInteralBlocks.append(HTMLBlock::Ptr(new SignedBlock(mWriter, *mMetaData, mCryptoProto, mSource, mFromAddress, false)));
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
                         Interface::ObjectTreeSource *source,
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