/*
   Copyright (C) 2013-2018 Laurent Montel <montel@kde.org>

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

#include "plainheaderstyle.h"
#include "messageviewer/messageviewersettings.h"
#include "header/headerstyle_util.h"

#include "header/headerstrategy.h"

#include <MessageCore/StringUtil>

#include <KLocalizedString>
#include <QApplication>
#include <kmime/kmime_message.h>

using namespace MessageCore;

using namespace MessageViewer;

class MessageViewer::PlainHeaderStylePrivate
{
public:
    PlainHeaderStylePrivate()
    {
    }

    QString formatAllMessageHeaders(KMime::Message *message) const;
    MessageViewer::HeaderStyleUtil mHeaderStyleUtil;
};

QString PlainHeaderStylePrivate::formatAllMessageHeaders(KMime::Message *message) const
{
    QByteArray head = message->head();
    KMime::Headers::Base *header = KMime::HeaderParsing::extractFirstHeader(head);
    QString result;
    while (header) {
        result += mHeaderStyleUtil.strToHtml(QLatin1String(header->type()) + QLatin1String(
                                                 ": ") + header->asUnicodeString());
        result += QLatin1String("<br />\n");
        delete header;
        header = KMime::HeaderParsing::extractFirstHeader(head);
    }

    return result;
}

PlainHeaderStyle::PlainHeaderStyle()
    : HeaderStyle()
    , d(new MessageViewer::PlainHeaderStylePrivate)
{
}

PlainHeaderStyle::~PlainHeaderStyle()
{
    delete d;
}

//
// PlainHeaderStyle:
//   show every header field on a line by itself,
//   show subject larger
//
QString PlainHeaderStyle::format(KMime::Message *message) const
{
    if (!message) {
        return QString();
    }
    const HeaderStrategy *strategy = headerStrategy();
    // The direction of the header is determined according to the direction
    // of the application layout.

    const QString dir
        = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");

    // However, the direction of the message subject within the header is
    // determined according to the contents of the subject itself. Since
    // the "Re:" and "Fwd:" prefixes would always cause the subject to be
    // considered left-to-right, they are ignored when determining its
    // direction.

    const QString subjectDir = d->mHeaderStyleUtil.subjectDirectionString(message);
    QString headerStr;

    if (strategy->headersToDisplay().isEmpty()
        && strategy->defaultPolicy() == HeaderStrategy::Display) {
        // crude way to emulate "all" headers - Note: no strings have
        // i18n(), so direction should always be ltr.
        headerStr = QStringLiteral("<div class=\"header\" dir=\"ltr\">");
        headerStr += d->formatAllMessageHeaders(message);
        return headerStr + QLatin1String("</div>");
    }

    headerStr = QStringLiteral("<div class=\"header\" dir=\"%1\">").arg(dir);

    //case HdrLong:
    if (strategy->showHeader(QStringLiteral("subject"))) {
        KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
        if (showEmoticons()) {
            flags |= KTextToHTML::ReplaceSmileys;
        }

        headerStr += QStringLiteral("<div dir=\"%1\"><b style=\"font-size:130%\">").arg(subjectDir)
                     +d->mHeaderStyleUtil.subjectString(message, flags) + QLatin1String(
            "</b></div>\n");
    }

    if (strategy->showHeader(QStringLiteral("date"))) {
        const auto dateFormat = isPrinting() ? MessageViewer::HeaderStyleUtil::ShortDate : MessageViewer::HeaderStyleUtil::CustomDate;
        headerStr.append(i18n("Date: ")
                         + HeaderStyleUtil::strToHtml(HeaderStyleUtil::dateString(message,
                                                                                  dateFormat))
                         + QLatin1String("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("from"))) {
        headerStr.append(i18n("From: ")
                         +StringUtil::emailAddrAsAnchor(message->from(),
                                                        StringUtil::DisplayFullAddress, QString(),
                                                        StringUtil::ShowLink));
        if (!vCardName().isEmpty()) {
            headerStr.append(QLatin1String("&nbsp;&nbsp;<a href=\"") + vCardName()
                             +QLatin1String("\">") + i18n("[vCard]") + QLatin1String("</a>"));
        }

        if (strategy->showHeader(QStringLiteral("organization"))
            && message->organization(false)) {
            headerStr.append(QLatin1String("&nbsp;&nbsp;(")
                             +d->mHeaderStyleUtil.strToHtml(
                                 message->organization()->asUnicodeString()) + QLatin1Char(')'));
        }
        headerStr.append(QLatin1String("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("to"))) {
        headerStr.append(i18nc("To-field of the mailheader.", "To: ")
                         +StringUtil::emailAddrAsAnchor(
                             message->to(),
                             StringUtil::DisplayFullAddress)
                         + QLatin1String("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("cc")) && message->cc(false)) {
        const QString str = StringUtil::emailAddrAsAnchor(
            message->cc(), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("CC: ") + str + QLatin1String("<br/>\n"));
        }
    }

    if (strategy->showHeader(QStringLiteral("bcc")) && message->bcc(false)) {
        const QString str = StringUtil::emailAddrAsAnchor(
            message->bcc(), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("BCC: ") + str + QLatin1String("<br/>\n"));
        }
    }

    if (strategy->showHeader(QStringLiteral("reply-to")) && message->replyTo(false)) {
        headerStr.append(i18n("Reply to: ")
                         +StringUtil::emailAddrAsAnchor(
                             message->replyTo(),
                             StringUtil::DisplayFullAddress)
                         + QLatin1String("<br/>\n"));
    }

    headerStr += QLatin1String("</div>\n");

    return headerStr;
}

const char *MessageViewer::PlainHeaderStyle::name() const
{
    return "plain";
}
