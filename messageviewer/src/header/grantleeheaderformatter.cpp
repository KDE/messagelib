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

#include "grantleeheaderformatter.h"
#include "headerstyle_util.h"
#include "grantleetheme/grantleetheme.h"
#include "settings/messageviewersettings.h"
#include "utils/iconnamecache.h"
#include "config-messageviewer.h"

#include <MessageCore/StringUtil>

#include <kmime/kmime_message.h>
#include <kmime/kmime_dateformatter.h>

#include <KLocalizedString>
#include <KIconLoader>

#include <grantlee/templateloader.h>
#include <grantlee/engine.h>

using namespace MessageCore;

using namespace MessageViewer;

class Q_DECL_HIDDEN MessageViewer::GrantleeHeaderFormatter::Private
{
public:
    Private()
    {
        iconSize = KIconLoader::global()->currentSize(KIconLoader::Toolbar);
        engine = new Grantlee::Engine;
        templateLoader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(new Grantlee::FileSystemTemplateLoader);
        engine->addTemplateLoader(templateLoader);
    }
    ~Private()
    {
        delete engine;
    }

    MessageViewer::HeaderStyleUtil headerStyleUtil;
    QSharedPointer<Grantlee::FileSystemTemplateLoader> templateLoader;
    int iconSize;
    Grantlee::Engine *engine;
};

GrantleeHeaderFormatter::GrantleeHeaderFormatter()
    : d(new GrantleeHeaderFormatter::Private)
{
}

GrantleeHeaderFormatter::~GrantleeHeaderFormatter()
{
    delete d;
}

QString GrantleeHeaderFormatter::toHtml(const GrantleeHeaderFormatter::GrantleeHeaderFormatterSettings &settings) const
{
    QString errorMessage;
    if (!settings.theme.isValid()) {
        errorMessage = i18n("Grantlee theme \"%1\" is not valid.", settings.theme.name());
        return errorMessage;
    }
    d->templateLoader->setTemplateDirs(QStringList() << settings.theme.absolutePath());
    Grantlee::Template headerTemplate = d->engine->loadByName(settings.theme.themeFilename());
    if (headerTemplate->error()) {
        errorMessage = headerTemplate->errorString();
        return errorMessage;
    }
    return format(settings.theme.absolutePath(), headerTemplate, settings.theme.displayExtraVariables(), settings.isPrinting, settings.style, settings.message, settings.showMailAction);
}

QString GrantleeHeaderFormatter::toHtml(const QStringList &displayExtraHeaders, const QString &absolutPath, const QString &filename, const MessageViewer::HeaderStyle *style, KMime::Message *message, bool isPrinting) const
{
    d->templateLoader->setTemplateDirs(QStringList() << absolutPath);
    Grantlee::Template headerTemplate = d->engine->loadByName(filename);
    if (headerTemplate->error()) {
        return headerTemplate->errorString();
    }
    return format(absolutPath, headerTemplate, displayExtraHeaders, isPrinting, style, message);
}

QString GrantleeHeaderFormatter::format(const QString &absolutePath, const Grantlee::Template &headerTemplate, const QStringList &displayExtraHeaders, bool isPrinting, const MessageViewer::HeaderStyle *style, KMime::Message *message, bool showMailAction) const
{
    QVariantHash headerObject;

    // However, the direction of the message subject within the header is
    // determined according to the contents of the subject itself. Since
    // the "Re:" and "Fwd:" prefixes would always cause the subject to be
    // considered left-to-right, they are ignored when determining its
    // direction.
    const QString absoluteThemePath = QUrl::fromLocalFile(absolutePath + QLatin1Char('/')).url();
    headerObject.insert(QStringLiteral("absoluteThemePath"), absoluteThemePath);
    headerObject.insert(QStringLiteral("showMailAction"), showMailAction);
    headerObject.insert(QStringLiteral("applicationDir"), QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr"));
    headerObject.insert(QStringLiteral("subjectDir"), d->headerStyleUtil.subjectDirectionString(message));

    headerObject.insert(QStringLiteral("subjecti18n"), i18n("Subject:"));
    KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
    if (MessageViewer::MessageViewerSettings::self()->showEmoticons()) {
        flags |= KTextToHTML::ReplaceSmileys;
    }

    headerObject.insert(QStringLiteral("subject"), d->headerStyleUtil.subjectString(message, flags));

    if (message->to(false)) {
        headerObject.insert(QStringLiteral("toi18n"), i18n("To:"));
        headerObject.insert(QStringLiteral("to"), StringUtil::emailAddrAsAnchor(message->to(), StringUtil::DisplayFullAddress));
        headerObject.insert(QStringLiteral("toNameOnly"), StringUtil::emailAddrAsAnchor(message->to(), StringUtil::DisplayNameOnly));
        headerObject.insert(QStringLiteral("toStr"), message->to()->asUnicodeString());
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(message->to(), MessageCore::StringUtil::DisplayFullAddress,
                            QString(), MessageCore::StringUtil::ShowLink,
                            MessageCore::StringUtil::ExpandableAddresses, QStringLiteral("FullToAddressList"));
        headerObject.insert(QStringLiteral("toExpandable"), val);
        headerObject.insert(QStringLiteral("toMailbox"), QVariant::fromValue(message->to()));
    }

    if (message->replyTo(false)) {
        headerObject.insert(QStringLiteral("replyToi18n"), i18n("Reply to:"));
        headerObject.insert(QStringLiteral("replyTo"), StringUtil::emailAddrAsAnchor(message->replyTo(), StringUtil::DisplayFullAddress));
        headerObject.insert(QStringLiteral("replyToStr"), message->replyTo()->asUnicodeString());
        headerObject.insert(QStringLiteral("replyToNameOnly"), StringUtil::emailAddrAsAnchor(message->replyTo(), StringUtil::DisplayNameOnly));
    }

    if (message->cc(false)) {
        headerObject.insert(QStringLiteral("cci18n"), i18n("CC:"));
        headerObject.insert(QStringLiteral("cc"), StringUtil::emailAddrAsAnchor(message->cc(), StringUtil::DisplayFullAddress));
        headerObject.insert(QStringLiteral("ccStr"), message->cc()->asUnicodeString());
        headerObject.insert(QStringLiteral("ccNameOnly"), StringUtil::emailAddrAsAnchor(message->cc(), StringUtil::DisplayNameOnly));
        headerObject.insert(QStringLiteral("ccMailbox"), QVariant::fromValue(message->cc()));
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(message->cc(), MessageCore::StringUtil::DisplayFullAddress,
                            QString(), MessageCore::StringUtil::ShowLink,
                            MessageCore::StringUtil::ExpandableAddresses, QStringLiteral("FullCcAddressList"));
        headerObject.insert(QStringLiteral("ccExpandable"), val);
    }

    if (message->bcc(false)) {
        headerObject.insert(QStringLiteral("bcci18n"), i18n("BCC:"));
        headerObject.insert(QStringLiteral("bcc"), StringUtil::emailAddrAsAnchor(message->bcc(), StringUtil::DisplayFullAddress));
        headerObject.insert(QStringLiteral("bccNameOnly"), StringUtil::emailAddrAsAnchor(message->bcc(), StringUtil::DisplayNameOnly));
        headerObject.insert(QStringLiteral("bccStr"), message->bcc()->asUnicodeString());
        headerObject.insert(QStringLiteral("bccMailbox"), QVariant::fromValue(message->bcc()));
    }
    headerObject.insert(QStringLiteral("fromi18n"), i18n("From:"));
    headerObject.insert(QStringLiteral("from"),  StringUtil::emailAddrAsAnchor(message->from(), StringUtil::DisplayFullAddress));
    headerObject.insert(QStringLiteral("fromStr"), message->from()->asUnicodeString());

    //Sender
    headerObject.insert(QStringLiteral("sender"), message->sender()->asUnicodeString());
    headerObject.insert(QStringLiteral("senderi18n"), i18n("Sender:"));
    headerObject.insert(QStringLiteral("listidi18n"), i18n("List-Id:"));

    if (message->headerByType("List-Id")) {
        headerObject.insert(QStringLiteral("listid"), message->headerByType("List-Id")->asUnicodeString());
    }


    const QString spamHtml = d->headerStyleUtil.spamStatus(message);
    if (!spamHtml.isEmpty()) {
        headerObject.insert(QStringLiteral("spamstatusi18n"), i18n("Spam Status:"));
        headerObject.insert(QStringLiteral("spamHTML"), spamHtml);
    }
    headerObject.insert(QStringLiteral("datei18n"), i18n("Date:"));

    headerObject.insert(QStringLiteral("dateshort"), d->headerStyleUtil.strToHtml(d->headerStyleUtil.dateString(message, isPrinting, MessageViewer::HeaderStyleUtil::ShortDate)));
    headerObject.insert(QStringLiteral("datelong"), d->headerStyleUtil.strToHtml(d->headerStyleUtil.dateString(message, isPrinting, MessageViewer::HeaderStyleUtil::CustomDate)));
    headerObject.insert(QStringLiteral("date"), d->headerStyleUtil.dateStr(message->date()->dateTime()));
    headerObject.insert(QStringLiteral("datefancylong"), d->headerStyleUtil.strToHtml(d->headerStyleUtil.dateString(message, isPrinting, MessageViewer::HeaderStyleUtil::FancyLongDate)));
    headerObject.insert(QStringLiteral("datefancyshort"), d->headerStyleUtil.strToHtml(d->headerStyleUtil.dateString(message, isPrinting, MessageViewer::HeaderStyleUtil::FancyShortDate)));
    headerObject.insert(QStringLiteral("datelocalelong"), d->headerStyleUtil.strToHtml(d->headerStyleUtil.dateString(message, isPrinting, MessageViewer::HeaderStyleUtil::LongDate)));

    if (MessageViewer::MessageViewerSettings::self()->showUserAgent()) {
        if (auto hdr = message->userAgent(false)) {
            headerObject.insert(QStringLiteral("useragent"), d->headerStyleUtil.strToHtml(hdr->asUnicodeString()));
        }

        if (message->headerByType("X-Mailer")) {
            headerObject.insert(QStringLiteral("xmailer"), d->headerStyleUtil.strToHtml(message->headerByType("X-Mailer")->asUnicodeString()));
        }
    }

    if (message->headerByType("Resent-From")) {
        headerObject.insert(QStringLiteral("resentfromi18n"), i18n("resent from"));
        const QVector<KMime::Types::Mailbox> resentFrom = d->headerStyleUtil.resentFromList(message);
        headerObject.insert(QStringLiteral("resentfrom"), StringUtil::emailAddrAsAnchor(resentFrom, StringUtil::DisplayFullAddress));
    }

    if (message->headerByType("Resent-To")) {
        const QVector<KMime::Types::Mailbox> resentTo = d->headerStyleUtil.resentToList(message);
        headerObject.insert(QStringLiteral("resenttoi18n"), i18np("receiver was", "receivers were", resentTo.count()));
        headerObject.insert(QStringLiteral("resentto"), StringUtil::emailAddrAsAnchor(resentTo, StringUtil::DisplayFullAddress));
    }

    if (auto organization = message->organization(false)) {
        headerObject.insert(QStringLiteral("organization"), d->headerStyleUtil.strToHtml(organization->asUnicodeString()));
    }

    if (!style->vCardName().isEmpty()) {
        headerObject.insert(QStringLiteral("vcardname"), style->vCardName());
    }
    if (!style->collectionName().isEmpty()) {
        headerObject.insert(QStringLiteral("collectionname"), style->collectionName());
    }

    if (isPrinting) {
        //provide a bit more left padding when printing
        //kolab/issue3254 (printed mail cut at the left side)
        //Use it just for testing if we are in printing mode
        headerObject.insert(QStringLiteral("isprinting"), i18n("Printing mode"));
        headerObject.insert(QStringLiteral("printmode"), QStringLiteral("printmode"));
    } else {
        headerObject.insert(QStringLiteral("screenmode"), QStringLiteral("screenmode"));
    }

    // colors depend on if it is encapsulated or not
    QColor fontColor(Qt::white);
    QString linkColor = QStringLiteral("white");
    const QColor activeColor = KColorScheme(QPalette::Active, KColorScheme::Selection).background().color();
    QColor activeColorDark = activeColor.dark(130);
    // reverse colors for encapsulated
    if (!style->isTopLevel()) {
        activeColorDark = activeColor.dark(50);
        fontColor = QColor(Qt::black);
        linkColor = QStringLiteral("black");
    }

    // 3D borders
    headerObject.insert(QStringLiteral("activecolordark"), activeColorDark.name());
    headerObject.insert(QStringLiteral("fontcolor"), fontColor.name());
    headerObject.insert(QStringLiteral("linkcolor"), linkColor);

    MessageViewer::HeaderStyleUtil::xfaceSettings xface = d->headerStyleUtil.xface(style, message);
    if (!xface.photoURL.isEmpty()) {
        headerObject.insert(QStringLiteral("photowidth"), xface.photoWidth);
        headerObject.insert(QStringLiteral("photoheight"), xface.photoHeight);
        headerObject.insert(QStringLiteral("photourl"), xface.photoURL);
    }

    Q_FOREACH (QString header, displayExtraHeaders) {
        const QByteArray baHeader = header.toLocal8Bit();
        if (message->headerByType(baHeader.constData())) {
            //Grantlee doesn't support '-' in variable name => remove it.
            header = header.remove(QLatin1Char('-'));
            headerObject.insert(header, message->headerByType(baHeader.constData())->asUnicodeString());
        }
    }

    headerObject.insert(QStringLiteral("vcardi18n"), i18n("[vcard]"));
    headerObject.insert(QStringLiteral("readOnlyMessage"), style->readOnlyMessage());

    const bool messageHasAttachment = KMime::hasAttachment(message);
    headerObject.insert(QStringLiteral("hasAttachment"), messageHasAttachment);

    if (messageHasAttachment) {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral("mail-attachment"), KIconLoader::Toolbar);
        const QString html = QStringLiteral("<img height=\"%2\" width=\"%2\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), QString::number(d->iconSize));
        headerObject.insert(QStringLiteral("attachmentIcon"), html);
    }

    //Action
    d->headerStyleUtil.addMailAction(headerObject);
    QVariantHash mapping;
    mapping.insert(QStringLiteral("header"), headerObject);
    Grantlee::Context context(mapping);

    return headerTemplate->render(&context);
}
