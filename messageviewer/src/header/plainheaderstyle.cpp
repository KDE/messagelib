/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plainheaderstyle.h"
#include "header/headerstyle_util.h"
#include "messageviewer/messageviewersettings.h"

#include "header/headerstrategy.h"

#include <MessageCore/StringUtil>

#include <KLocalizedString>
#include <KMime/Message>
#include <QApplication>

using namespace MessageCore;

using namespace MessageViewer;

class MessageViewer::PlainHeaderStylePrivate
{
public:
    PlainHeaderStylePrivate() = default;

    [[nodiscard]] QString formatAllMessageHeaders(KMime::Message *message) const;
    MessageViewer::HeaderStyleUtil mHeaderStyleUtil;
};

QString PlainHeaderStylePrivate::formatAllMessageHeaders(KMime::Message *message) const
{
    QByteArray head = message->head();
    QByteArrayView headView(head);
    auto header = KMime::HeaderParsing::parseNextHeader(headView);
    QString result;
    while (header) {
        result += mHeaderStyleUtil.strToHtml(QLatin1StringView(header->type()) + QLatin1StringView(": ") + header->asUnicodeString());
        result += QLatin1StringView("<br />\n");
        header = KMime::HeaderParsing::parseNextHeader(headView);
    }

    return result;
}

PlainHeaderStyle::PlainHeaderStyle()
    : HeaderStyle()
    , d(new MessageViewer::PlainHeaderStylePrivate)
{
}

PlainHeaderStyle::~PlainHeaderStyle() = default;

//
// PlainHeaderStyle:
//   show every header field on a line by itself,
//   show subject larger
//
QString PlainHeaderStyle::format(KMime::Message *message) const
{
    if (!message) {
        return {};
    }
    const HeaderStrategy *strategy = headerStrategy();
    // The direction of the header is determined according to the direction
    // of the application layout.

    const QString dir = QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");

    // However, the direction of the message subject within the header is
    // determined according to the contents of the subject itself. Since
    // the "Re:" and "Fwd:" prefixes would always cause the subject to be
    // considered left-to-right, they are ignored when determining its
    // direction.

    const QString subjectDir = d->mHeaderStyleUtil.subjectDirectionString(message);
    QString headerStr;

    if (strategy->headersToDisplay().isEmpty() && strategy->defaultPolicy() == HeaderStrategy::Display) {
        // crude way to emulate "all" headers - Note: no strings have
        // i18n(), so direction should always be ltr.
        headerStr = QStringLiteral("<div class=\"header\" dir=\"ltr\">");
        headerStr += d->formatAllMessageHeaders(message);
        return headerStr + QLatin1StringView("</div>");
    }

    headerStr = QStringLiteral("<div class=\"header\" dir=\"%1\">").arg(dir);

    // case HdrLong:
    if (strategy->showHeader(QStringLiteral("subject"))) {
        KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
        if (showEmoticons()) {
            flags |= KTextToHTML::ReplaceSmileys;
        }

        headerStr += QStringLiteral("<div dir=\"%1\"><b style=\"font-size:130%\">").arg(subjectDir) + d->mHeaderStyleUtil.subjectString(message, flags)
            + QLatin1StringView("</b></div>\n");
    }

    if (strategy->showHeader(QStringLiteral("date"))) {
        const auto dateFormat = isPrinting() ? MessageViewer::HeaderStyleUtil::ShortDate : MessageViewer::HeaderStyleUtil::CustomDate;
        headerStr.append(i18n("Date: ") + HeaderStyleUtil::strToHtml(HeaderStyleUtil::dateString(message, dateFormat)) + QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("from"))) {
        headerStr.append(i18n("From: ") + StringUtil::emailAddrAsAnchor(message->from(), StringUtil::DisplayFullAddress, QString(), StringUtil::ShowLink));
        if (!vCardName().isEmpty()) {
            headerStr.append(QLatin1StringView("&nbsp;&nbsp;<a href=\"") + vCardName() + QLatin1StringView("\">") + i18n("[vCard]")
                             + QLatin1StringView("</a>"));
        }

        if (strategy->showHeader(QStringLiteral("organization")) && message->organization(false)) {
            headerStr.append(QLatin1StringView("&nbsp;&nbsp;(") + d->mHeaderStyleUtil.strToHtml(message->organization(false)->asUnicodeString())
                             + QLatin1Char(')'));
        }
        headerStr.append(QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("to"))) {
        headerStr.append(i18nc("To-field of the mailheader.", "To: ") + StringUtil::emailAddrAsAnchor(message->to(), StringUtil::DisplayFullAddress)
                         + QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(QStringLiteral("cc")) && message->cc(false)) {
        const QString str = StringUtil::emailAddrAsAnchor(message->cc(false), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("CC: ") + str + QLatin1StringView("<br/>\n"));
        }
    }

    if (strategy->showHeader(QStringLiteral("bcc")) && message->bcc(false)) {
        const QString str = StringUtil::emailAddrAsAnchor(message->bcc(false), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("BCC: ") + str + QLatin1StringView("<br/>\n"));
        }
    }

    if (strategy->showHeader(QStringLiteral("reply-to")) && message->replyTo(false)) {
        headerStr.append(i18n("Reply to: ") + StringUtil::emailAddrAsAnchor(message->replyTo(false), StringUtil::DisplayFullAddress)
                         + QLatin1StringView("<br/>\n"));
    }

    headerStr += QLatin1StringView("</div>\n");

    return headerStr;
}

const char *MessageViewer::PlainHeaderStyle::name() const
{
    return "plain";
}
