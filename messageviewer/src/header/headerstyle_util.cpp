/*
   Copyright (C) 2013-2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "headerstyle_util.h"
#include "messageviewer_debug.h"

#include "contactdisplaymessagememento.h"
#include "kxface.h"

#include "header/headerstyle.h"
#include "utils/iconnamecache.h"
#include "settings/messageviewersettings.h"

#include <MessageCore/StringUtil>
#include <MimeTreeParser/NodeHelper>

#include <MessageCore/MessageCoreSettings>

#include <KEmailAddress>
#include <KIconLoader>
#include <KLocalizedString>
#include <KTextToHTML>

#include <QBuffer>
#include <QVariantHash>

using namespace MessageCore;

using namespace MessageViewer;
//
// Convenience functions:
//
HeaderStyleUtil::HeaderStyleUtil()
{
    mIconSize = KIconLoader::global()->currentSize(KIconLoader::Toolbar);
}

QString HeaderStyleUtil::directionOf(const QString &str) const
{
    return str.isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
}

QString HeaderStyleUtil::strToHtml(const QString &str, KTextToHTML::Options flags) const
{
    return KTextToHTML::convertToHtml(str, flags);
}

// Prepare the date string (when printing always use the localized date)
QString HeaderStyleUtil::dateString(KMime::Message *message, bool printing, HeaderStyleUtilDateFormat dateFormat) const
{
    const QDateTime dateTime = message->date()->dateTime();
    if (!dateTime.isValid()) {
        return i18nc("Unknown date", "Unknown");
    }
    if (printing) {
        return QLocale::system().toString(dateTime, QLocale::ShortFormat);
    } else {
        switch (dateFormat) {
        case ShortDate:
            return QLocale::system().toString(dateTime, QLocale::ShortFormat);
        case LongDate:
            return QLocale::system().toString(dateTime, QLocale::LongFormat);
        case FancyShortDate:
            return dateShortStr(dateTime);
        case FancyLongDate:
        //Laurent fix me
        //TODO return QLocale::system().toString(dateTime, QLocale::LongFormat);
        case CustomDate:
        default:
            return dateStr(dateTime);
        }
    }
}

QString HeaderStyleUtil::subjectString(KMime::Message *message, KTextToHTML::Options flags) const
{
    QString subjectStr;
    const KMime::Headers::Subject *const subject = message->subject(false);
    if (subject) {
        subjectStr = subject->asUnicodeString();
        if (subjectStr.isEmpty()) {
            subjectStr = i18n("No Subject");
        } else {
            subjectStr = strToHtml(subjectStr, flags);
        }
    } else {
        subjectStr = i18n("No Subject");
    }
    return subjectStr;
}

QString HeaderStyleUtil::subjectDirectionString(KMime::Message *message) const
{
    QString subjectDir;
    if (message->subject(false)) {
        subjectDir = directionOf(MimeTreeParser::NodeHelper::cleanSubject(message));
    } else {
        subjectDir = directionOf(i18n("No Subject"));
    }
    return subjectDir;
}

QString HeaderStyleUtil::spamStatus(KMime::Message *message) const
{
    QString spamHTML;
    const SpamScores scores = SpamHeaderAnalyzer::getSpamScores(message);

    for (SpamScores::const_iterator it = scores.constBegin(), end = scores.constEnd(); it != end; ++it)
        spamHTML += (*it).agent() + QLatin1Char(' ') +
                    drawSpamMeter((*it).error(), (*it).score(), (*it).confidence(), (*it).spamHeader(), (*it).confidenceHeader());
    return spamHTML;
}

QString HeaderStyleUtil::drawSpamMeter(SpamError spamError, double percent, double confidence,
                                       const QString &filterHeader, const QString &confidenceHeader) const
{
    static const int meterWidth = 20;
    static const int meterHeight = 5;
    QImage meterBar(meterWidth, 1, QImage::Format_Indexed8/*QImage::Format_RGB32*/);
    meterBar.setNumColors(24);

    meterBar.setColor(meterWidth + 1, qRgb(255, 255, 255));
    meterBar.setColor(meterWidth + 2, qRgb(170, 170, 170));
    if (spamError != noError) { // grey is for errors
        meterBar.fill(meterWidth + 2);
    } else {
        static const unsigned short gradient[meterWidth][3] = {
            {   0, 255,   0 },
            {  27, 254,   0 },
            {  54, 252,   0 },
            {  80, 250,   0 },
            { 107, 249,   0 },
            { 135, 247,   0 },
            { 161, 246,   0 },
            { 187, 244,   0 },
            { 214, 242,   0 },
            { 241, 241,   0 },
            { 255, 228,   0 },
            { 255, 202,   0 },
            { 255, 177,   0 },
            { 255, 151,   0 },
            { 255, 126,   0 },
            { 255, 101,   0 },
            { 255,  76,   0 },
            { 255,  51,   0 },
            { 255,  25,   0 },
            { 255,   0,   0 }
        };

        meterBar.fill(meterWidth + 1);
        const int max = qMin(meterWidth, static_cast<int>(percent) / 5);
        for (int i = 0; i < max; ++i) {
            meterBar.setColor(i + 1, qRgb(gradient[i][0], gradient[i][1],
                                          gradient[i][2]));
            meterBar.setPixel(i, 0, i + 1);
        }
    }

    QString titleText;
    QString confidenceString;
    if (spamError == noError) {
        if (confidence >= 0) {
            confidenceString = QString::number(confidence) + QLatin1String("% &nbsp;");
            titleText = i18n("%1% probability of being spam with confidence %3%.\n\n"
                             "Full report:\nProbability=%2\nConfidence=%4",
                             QString::number(percent, 'f', 2), filterHeader, confidence, confidenceHeader);
        } else { // do not show negative confidence
            confidenceString = QString() + QLatin1String("&nbsp;");
            titleText = i18n("%1% probability of being spam.\n\n"
                             "Full report:\nProbability=%2",
                             QString::number(percent, 'f', 2), filterHeader);
        }
    } else {
        QString errorMsg;
        switch (spamError) {
        case errorExtractingAgentString:
            errorMsg = i18n("No Spam agent");
            break;
        case couldNotConverScoreToFloat:
            errorMsg = i18n("Spam filter score not a number");
            break;
        case couldNotConvertThresholdToFloatOrThresholdIsNegative:
            errorMsg = i18n("Threshold not a valid number");
            break;
        case couldNotFindTheScoreField:
            errorMsg = i18n("Spam filter score could not be extracted from header");
            break;
        case couldNotFindTheThresholdField:
            errorMsg = i18n("Threshold could not be extracted from header");
            break;
        default:
            errorMsg = i18n("Error evaluating spam score");
            break;
        }
        // report the error in the spam filter
        titleText = i18n("%1.\n\n"
                         "Full report:\n%2",
                         errorMsg, filterHeader);
    }
    return QStringLiteral("<img src=\"%1\" width=\"%2\" height=\"%3\" style=\"border: 1px solid black;\" title=\"%4\"> &nbsp;")
           .arg(imgToDataUrl(meterBar), QString::number(meterWidth),
                QString::number(meterHeight), titleText) + confidenceString;
}

QString HeaderStyleUtil::imgToDataUrl(const QImage &image) const
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return QStringLiteral("data:image/%1;base64,%2").arg(QStringLiteral("PNG"), QString::fromLatin1(ba.toBase64()));
}

QString HeaderStyleUtil::dateStr(const QDateTime &dateTime) const
{
    const time_t unixTime = dateTime.toTime_t();
    return KMime::DateFormatter::formatDate(
               static_cast<KMime::DateFormatter::FormatType>(
                   MessageCore::MessageCoreSettings::self()->dateFormat()),
               unixTime, MessageCore::MessageCoreSettings::self()->customDateFormat());
}

QString HeaderStyleUtil::dateShortStr(const QDateTime &dateTime) const
{
    KMime::DateFormatter formatter(KMime::DateFormatter::Fancy);
    return formatter.dateString(dateTime);
}

QVector<KMime::Types::Mailbox> HeaderStyleUtil::resentFromList(KMime::Message *message) const
{
    // Get the resent-from header into a Mailbox
    QVector<KMime::Types::Mailbox> resentFrom;
    if (message->headerByType("Resent-From")) {
        const QByteArray data = message->headerByType("Resent-From")->as7BitString(false);
        const char *start = data.data();
        const char *end = start + data.length();
        KMime::Types::AddressList addressList;
        KMime::HeaderParsing::parseAddressList(start, end, addressList);
        foreach (const KMime::Types::Address &addr, addressList) {
            foreach (const KMime::Types::Mailbox &mbox, addr.mailboxList) {
                resentFrom.append(mbox);
            }
        }
    }
    return resentFrom;
}

QVector<KMime::Types::Mailbox> HeaderStyleUtil::resentToList(KMime::Message *message) const
{
    // Get the resent-from header into a Mailbox
    QVector<KMime::Types::Mailbox> resentTo;
    if (message->headerByType("Resent-To")) {
        const QByteArray data = message->headerByType("Resent-To")->as7BitString(false);
        const char *start = data.data();
        const char *end = start + data.length();
        KMime::Types::AddressList addressList;
        KMime::HeaderParsing::parseAddressList(start, end, addressList);
        foreach (const KMime::Types::Address &addr, addressList) {
            foreach (const KMime::Types::Mailbox &mbox, addr.mailboxList) {
                resentTo.append(mbox);
            }
        }
    }
    return resentTo;
}

void HeaderStyleUtil::updateXFaceSettings(QImage photo, xfaceSettings &settings) const
{
    if (!photo.isNull()) {
        settings.photoWidth = photo.width();
        settings.photoHeight = photo.height();
        // scale below 60, otherwise it can get way too large
        if (settings.photoHeight > 60) {
            double ratio = (double)settings.photoHeight / (double)settings.photoWidth;
            settings.photoHeight = 60;
            settings.photoWidth = (int)(60 / ratio);
            photo = photo.scaled(settings.photoWidth, settings.photoHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        settings.photoURL = MessageViewer::HeaderStyleUtil::imgToDataUrl(photo);
    }
}

HeaderStyleUtil::xfaceSettings HeaderStyleUtil::xface(const MessageViewer::HeaderStyle *style, KMime::Message *message) const
{

    xfaceSettings settings;
    bool useOtherPhotoSources = false;

    if (style->allowAsync()) {

        Q_ASSERT(style->nodeHelper());
        Q_ASSERT(style->sourceObject());

        ContactDisplayMessageMemento *photoMemento =
            dynamic_cast<ContactDisplayMessageMemento *>(style->nodeHelper()->bodyPartMemento(message, "contactphoto"));
        if (!photoMemento) {
            const QString email = QString::fromLatin1(KEmailAddress::firstEmailAddress(message->from()->as7BitString(false)));
            photoMemento = new ContactDisplayMessageMemento(email);
            style->nodeHelper()->setBodyPartMemento(message, "contactphoto", photoMemento);
            QObject::connect(photoMemento, SIGNAL(update(MimeTreeParser::UpdateMode)),
                             style->sourceObject(), SLOT(update(MimeTreeParser::UpdateMode)));

            QObject::connect(photoMemento, SIGNAL(changeDisplayMail(Viewer::DisplayFormatMessage,bool)),
                             style->sourceObject(), SIGNAL(changeDisplayMail(Viewer::DisplayFormatMessage,bool)));
        }

        if (photoMemento->finished()) {

            useOtherPhotoSources = true;
            if (photoMemento->photo().isIntern()) {
                // get photo data and convert to data: url
                const QImage photo = photoMemento->photo().data();
                updateXFaceSettings(photo, settings);
            } else if (!photoMemento->imageFromUrl().isNull()) {
                updateXFaceSettings(photoMemento->imageFromUrl(), settings);
            } else if (!photoMemento->photo().url().isEmpty()) {
                settings.photoURL = photoMemento->photo().url();
                if (settings.photoURL.startsWith(QLatin1Char('/'))) {
                    settings.photoURL.prepend(QStringLiteral("file:"));
                }
            } else if (!photoMemento->gravatarPixmap().isNull()) {
                const QImage photo = photoMemento->gravatarPixmap().toImage();
                updateXFaceSettings(photo, settings);
            }
        } else {
            // if the memento is not finished yet, use other photo sources instead
            useOtherPhotoSources = true;
        }
    } else {
        useOtherPhotoSources = true;
    }

    if (settings.photoURL.isEmpty() && message->headerByType("Face") && useOtherPhotoSources) {
        // no photo, look for a Face header
        const QString faceheader = message->headerByType("Face")->asUnicodeString();
        if (!faceheader.isEmpty()) {

            qCDebug(MESSAGEVIEWER_LOG) << "Found Face: header";

            const QByteArray facestring = faceheader.toUtf8();
            // Spec says header should be less than 998 bytes
            // Face: is 5 characters
            if (facestring.length() < 993) {
                const QByteArray facearray = QByteArray::fromBase64(facestring);

                QImage faceimage;
                if (faceimage.loadFromData(facearray, "png")) {
                    // Spec says image must be 48x48 pixels
                    if ((48 == faceimage.width()) && (48 == faceimage.height())) {
                        settings.photoURL = MessageViewer::HeaderStyleUtil::imgToDataUrl(faceimage);
                        settings.photoWidth = 48;
                        settings.photoHeight = 48;
                    } else {
                        qCDebug(MESSAGEVIEWER_LOG) << "Face: header image is" << faceimage.width() << "by"
                                                   << faceimage.height() << "not 48x48 Pixels";
                    }
                } else {
                    qCDebug(MESSAGEVIEWER_LOG) << "Failed to load decoded png from Face: header";
                }
            } else {
                qCDebug(MESSAGEVIEWER_LOG) << "Face: header too long at" << facestring.length();
            }
        }
    }

    if (settings.photoURL.isEmpty() && message->headerByType("X-Face") && useOtherPhotoSources) {
        // no photo, look for a X-Face header
        const QString xfhead = message->headerByType("X-Face")->asUnicodeString();
        if (!xfhead.isEmpty()) {
            MessageViewer::KXFace xf;
            settings.photoURL = MessageViewer::HeaderStyleUtil::imgToDataUrl(xf.toImage(xfhead));
            settings.photoWidth = 48;
            settings.photoHeight = 48;
        }
    }

    return settings;
}

void HeaderStyleUtil::addMailAction(QVariantHash &headerObject)
{
    headerObject.insert(QStringLiteral("trashaction"), mailAction(Viewer::Trash));
    headerObject.insert(QStringLiteral("replyaction"), mailAction(Viewer::Reply));
    headerObject.insert(QStringLiteral("replyallaction"), mailAction(Viewer::ReplyToAll));
    headerObject.insert(QStringLiteral("forwardaction"), mailAction(Viewer::Forward));
    headerObject.insert(QStringLiteral("newmessageaction"), mailAction(Viewer::NewMessage));
    headerObject.insert(QStringLiteral("printmessageaction"), mailAction(Viewer::Print));
    headerObject.insert(QStringLiteral("printpreviewmessageaction"), mailAction(Viewer::PrintPreview));
}

QString HeaderStyleUtil::mailAction(Viewer::MailAction action) const
{
    QString html;
    switch (action) {
    case Viewer::Trash: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("user-trash"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:trash\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Move to Trash"), QString::number(mIconSize));
        break;
    }
    case Viewer::Reply: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("mail-reply-sender"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:reply\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Reply"), QString::number(mIconSize));
        break;
    }
    case Viewer::ReplyToAll: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("mail-reply-all"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:replyToAll\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Reply to All"), QString::number(mIconSize));
        break;
    }
    case Viewer::Forward: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("mail-forward"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:forward\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Forward"), QString::number(mIconSize));
        break;
    }
    case Viewer::NewMessage: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("mail-message-new"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:newMessage\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("New Message"), QString::number(mIconSize));
        break;
    }
    case Viewer::Print: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("document-print"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:print\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Print"), QString::number(mIconSize));
        break;
    }
    case Viewer::PrintPreview: {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("document-print-preview"), KIconLoader::Toolbar);
        html = QStringLiteral("<a href=\"kmailaction:printpreview\"><img title=\"%2\" height=\"%3\" width=\"%3\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), i18n("Print Preview"), QString::number(mIconSize));
        break;
    }
    }
    return html;
}
