/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plainheaderstyle.h"
using namespace Qt::Literals::StringLiterals;

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
    : d(new MessageViewer::PlainHeaderStylePrivate)
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

    const QString dir = QApplication::isRightToLeft() ? u"rtl"_s : u"ltr"_s;

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
        headerStr = u"<div class=\"header\" dir=\"ltr\">"_s;
        headerStr += d->formatAllMessageHeaders(message);
        return headerStr + QLatin1StringView("</div>");
    }

    headerStr = u"<div class=\"header\" dir=\"%1\">"_s.arg(dir);

    // case HdrLong:
    if (strategy->showHeader(u"subject"_s)) {
        KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
        if (showEmoticons()) {
            flags |= KTextToHTML::ReplaceSmileys;
        }

        headerStr += u"<div dir=\"%1\"><b style=\"font-size:130%\">"_s.arg(subjectDir) + d->mHeaderStyleUtil.subjectString(message, flags)
            + QLatin1StringView("</b></div>\n");
    }

    if (strategy->showHeader(u"date"_s)) {
        const auto dateFormat = isPrinting() ? MessageViewer::HeaderStyleUtil::ShortDate : MessageViewer::HeaderStyleUtil::CustomDate;
        headerStr.append(i18n("Date: ") + HeaderStyleUtil::strToHtml(HeaderStyleUtil::dateString(message, dateFormat)) + QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(u"from"_s)) {
        headerStr.append(i18n("From: ") + StringUtil::emailAddrAsAnchor(message->from(), StringUtil::DisplayFullAddress, QString(), StringUtil::ShowLink));
        if (!vCardName().isEmpty()) {
            headerStr.append(QLatin1StringView("&nbsp;&nbsp;<a href=\"") + vCardName() + QLatin1StringView("\">") + i18n("[vCard]")
                             + QLatin1StringView("</a>"));
        }

        if (strategy->showHeader(u"organization"_s) && message->organization(KMime::CreatePolicy::DontCreate)) {
            headerStr.append(QLatin1StringView("&nbsp;&nbsp;(")
                             + d->mHeaderStyleUtil.strToHtml(message->organization(KMime::CreatePolicy::DontCreate)->asUnicodeString()) + u')');
        }
        headerStr.append(QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(u"to"_s)) {
        headerStr.append(i18nc("To-field of the mailheader.", "To: ") + StringUtil::emailAddrAsAnchor(message->to(), StringUtil::DisplayFullAddress)
                         + QLatin1StringView("<br/>\n"));
    }

    if (strategy->showHeader(u"cc"_s) && message->cc(KMime::CreatePolicy::DontCreate)) {
        const QString str = StringUtil::emailAddrAsAnchor(message->cc(KMime::CreatePolicy::DontCreate), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("CC: ") + str + QLatin1StringView("<br/>\n"));
        }
    }

    if (strategy->showHeader(u"bcc"_s) && message->bcc(KMime::CreatePolicy::DontCreate)) {
        const QString str = StringUtil::emailAddrAsAnchor(message->bcc(KMime::CreatePolicy::DontCreate), StringUtil::DisplayFullAddress);
        if (!str.isEmpty()) {
            headerStr.append(i18n("BCC: ") + str + QLatin1StringView("<br/>\n"));
        }
    }

    if (strategy->showHeader(u"reply-to"_s) && message->replyTo(KMime::CreatePolicy::DontCreate)) {
        headerStr.append(i18n("Reply to: ") + StringUtil::emailAddrAsAnchor(message->replyTo(KMime::CreatePolicy::DontCreate), StringUtil::DisplayFullAddress)
                         + QLatin1StringView("<br/>\n"));
    }

    headerStr += QLatin1StringView("</div>\n");

    return headerStr;
}

const char *MessageViewer::PlainHeaderStyle::name() const
{
    return "plain";
}
