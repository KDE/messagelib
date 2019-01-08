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
#include <KColorScheme>

#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>

using namespace MessageCore;

using namespace MessageViewer;

Q_DECLARE_METATYPE(const KMime::Headers::Generics::AddressList *)
Q_DECLARE_METATYPE(const KMime::Headers::Generics::MailboxList *)
Q_DECLARE_METATYPE(QSharedPointer<KMime::Headers::Generics::MailboxList>)
Q_DECLARE_METATYPE(const KMime::Headers::Date *)

// Read-only introspection of KMime::Headers::Generics::AddressList object.
namespace Grantlee {
template<>
inline QVariant TypeAccessor<const KMime::Headers::Generics::AddressList *>::lookUp(const KMime::Headers::Generics::AddressList *const object, const QString &property)
{
    if (property == QStringLiteral("nameOnly")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayNameOnly);
    } else if (property == QStringLiteral("isSet")) {
        return !object->asUnicodeString().isEmpty();
    } else if (property == QStringLiteral("fullAddress")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayFullAddress);
    } else if (property == QStringLiteral("str")) {
        return object->asUnicodeString();
    } else if (property.startsWith(QStringLiteral("expandable"))) {
        const auto &name = property.mid(10);
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(
            object, MessageCore::StringUtil::DisplayFullAddress,
            QString(), MessageCore::StringUtil::ShowLink,
            MessageCore::StringUtil::ExpandableAddresses,
            QStringLiteral("Full") + name + QStringLiteral("AddressList"));
        return val;
    }
    return QVariant();
}
}

// Read-only introspection of KMime::Headers::Generics::MailboxList object.
namespace Grantlee {
template<>
inline QVariant TypeAccessor<const KMime::Headers::Generics::MailboxList *>::lookUp(const KMime::Headers::Generics::MailboxList *const object, const QString &property)
{
    if (property == QStringLiteral("nameOnly")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayNameOnly);
    } else if (property == QStringLiteral("isSet")) {
        return !object->asUnicodeString().isEmpty();
    } else if (property == QStringLiteral("fullAddress")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayFullAddress);
    } else if (property == QStringLiteral("str")) {
        return object->asUnicodeString();
    } else if (property.startsWith(QStringLiteral("expandable"))) {
        const auto &name = property.mid(10);
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(
            object, MessageCore::StringUtil::DisplayFullAddress,
            QString(), MessageCore::StringUtil::ShowLink,
            MessageCore::StringUtil::ExpandableAddresses,
            QStringLiteral("Full") + name + QStringLiteral("AddressList"));
        return val;
    }
    return QVariant();
}
}

GRANTLEE_BEGIN_LOOKUP(QSharedPointer<KMime::Headers::Generics::MailboxList>)
if (property == QStringLiteral("nameOnly")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayNameOnly);
} else if (property == QStringLiteral("isSet")) {
    return !object->asUnicodeString().isEmpty();
} else if (property == QStringLiteral("fullAddress")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayFullAddress);
} else if (property == QStringLiteral("str")) {
    return object->asUnicodeString();
} else if (property.startsWith(QStringLiteral("expandable"))) {
    const auto &name = property.mid(10);
    const QString val = MessageCore::StringUtil::emailAddrAsAnchor(
        object.data(), MessageCore::StringUtil::DisplayFullAddress,
        QString(), MessageCore::StringUtil::ShowLink,
        MessageCore::StringUtil::ExpandableAddresses,
        QStringLiteral("Full") + name + QStringLiteral("AddressList"));
    return val;
}
GRANTLEE_END_LOOKUP

namespace Grantlee {
template<>
inline QVariant TypeAccessor<const KMime::Headers::Date *>::lookUp(const KMime::Headers::Date *const object, const QString &property)
{
    MessageViewer::HeaderStyleUtil::HeaderStyleUtilDateFormat dateFormat;
    if (property == QStringLiteral("str")) {
        return HeaderStyleUtil::dateStr(object->dateTime());
    } else if (property == QStringLiteral("short")) {
        dateFormat = MessageViewer::HeaderStyleUtil::ShortDate;
    } else if (property == QStringLiteral("long")) {
        dateFormat = MessageViewer::HeaderStyleUtil::CustomDate;
    } else if (property == QStringLiteral("fancylong")) {
        dateFormat = MessageViewer::HeaderStyleUtil::FancyLongDate;
    } else if (property == QStringLiteral("fancyshort")) {
        dateFormat = MessageViewer::HeaderStyleUtil::FancyShortDate;
    } else if (property == QStringLiteral("localelong")) {
        dateFormat = MessageViewer::HeaderStyleUtil::LongDate;
    } else {
        return QVariant();
    }

    return HeaderStyleUtil::strToHtml(HeaderStyleUtil::dateString(object, dateFormat));
}
}

class Q_DECL_HIDDEN MessageViewer::GrantleeHeaderFormatter::Private
{
public:
    Private()
    {
        Grantlee::registerMetaType<const KMime::Headers::Generics::AddressList *>();
        Grantlee::registerMetaType<const KMime::Headers::Generics::MailboxList *>();
        Grantlee::registerMetaType<QSharedPointer<KMime::Headers::Generics::MailboxList> >();
        Grantlee::registerMetaType<const KMime::Headers::Date *>();
        iconSize = KIconLoader::global()->currentSize(KIconLoader::Toolbar);
        engine = new Grantlee::Engine;
        templateLoader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(
            new Grantlee::FileSystemTemplateLoader);
        engine->addTemplateLoader(templateLoader);
    }

    ~Private()
    {
        delete engine;
    }

    QSharedPointer<Grantlee::FileSystemTemplateLoader> templateLoader;
    Grantlee::Engine *engine = nullptr;
    MessageViewer::HeaderStyleUtil headerStyleUtil;
    int iconSize;
};

GrantleeHeaderFormatter::GrantleeHeaderFormatter()
    : d(new GrantleeHeaderFormatter::Private)
{
}

GrantleeHeaderFormatter::~GrantleeHeaderFormatter()
{
    delete d;
}

QString GrantleeHeaderFormatter::toHtml(
    const GrantleeHeaderFormatter::GrantleeHeaderFormatterSettings &settings) const
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
    return format(
        settings.theme.absolutePath(), headerTemplate,
        settings.theme.displayExtraVariables(), settings.isPrinting, settings.style, settings.message,
        settings.showEmoticons);
}

QString GrantleeHeaderFormatter::toHtml(const QStringList &displayExtraHeaders, const QString &absolutPath, const QString &filename, const MessageViewer::HeaderStyle *style, KMime::Message *message,
                                        bool isPrinting) const
{
    d->templateLoader->setTemplateDirs(QStringList() << absolutPath);
    Grantlee::Template headerTemplate = d->engine->loadByName(filename);
    if (headerTemplate->error()) {
        return headerTemplate->errorString();
    }
    return format(absolutPath, headerTemplate, displayExtraHeaders, isPrinting, style, message);
}

QString GrantleeHeaderFormatter::format(const QString &absolutePath, const Grantlee::Template &headerTemplate, const QStringList &displayExtraHeaders, bool isPrinting,
                                        const MessageViewer::HeaderStyle *style, KMime::Message *message, bool showEmoticons) const
{
    QVariantHash headerObject;

    // However, the direction of the message subject within the header is
    // determined according to the contents of the subject itself. Since
    // the "Re:" and "Fwd:" prefixes would always cause the subject to be
    // considered left-to-right, they are ignored when determining its
    // direction.
    const QString absoluteThemePath = QUrl::fromLocalFile(absolutePath + QLatin1Char('/')).url();
    headerObject.insert(QStringLiteral("absoluteThemePath"), absoluteThemePath);
    headerObject.insert(QStringLiteral("applicationDir"),
                        QApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral(
                            "ltr"));
    headerObject.insert(QStringLiteral("subjectDir"),
                        d->headerStyleUtil.subjectDirectionString(message));

    headerObject.insert(QStringLiteral("subjecti18n"), i18n("Subject:"));
    KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
    if (showEmoticons) {
        flags |= KTextToHTML::ReplaceSmileys;
    }

    headerObject.insert(QStringLiteral("subject"),
                        d->headerStyleUtil.subjectString(message, flags));

    if (message->to(false)) {
        headerObject.insert(QStringLiteral("toi18n"), i18n("To:"));
        headerObject.insert(QStringLiteral("to"), QVariant::fromValue(static_cast<const KMime::Headers::Generics::AddressList *>(message->to())));
    }

    if (message->replyTo(false)) {
        headerObject.insert(QStringLiteral("replyToi18n"), i18n("Reply to:"));
        headerObject.insert(QStringLiteral("replyTo"), QVariant::fromValue(static_cast<const KMime::Headers::Generics::AddressList *>(message->replyTo())));
    }

    if (message->cc(false)) {
        headerObject.insert(QStringLiteral("cci18n"), i18n("CC:"));
        headerObject.insert(QStringLiteral("cc"), QVariant::fromValue(static_cast<const KMime::Headers::Generics::AddressList *>(message->cc())));
    }

    if (message->bcc(false)) {
        headerObject.insert(QStringLiteral("bcci18n"), i18n("BCC:"));
        headerObject.insert(QStringLiteral("bcc"), QVariant::fromValue(static_cast<const KMime::Headers::Generics::AddressList *>(message->bcc())));
    }
    headerObject.insert(QStringLiteral("fromi18n"), i18n("From:"));
    headerObject.insert(QStringLiteral("from"), QVariant::fromValue(static_cast<const KMime::Headers::Generics::MailboxList *>(message->from())));

    //Sender
    headerObject.insert(QStringLiteral("senderi18n"), i18n("Sender:"));
    headerObject.insert(QStringLiteral("sender"),
                        HeaderStyleUtil::strToHtml(message->sender()->asUnicodeString()));
    headerObject.insert(QStringLiteral("listidi18n"), i18n("List-Id:"));

    if (auto hrd = message->headerByType("List-Id")) {
        headerObject.insert(QStringLiteral("listid"), hrd->asUnicodeString());
    }

    const QString spamHtml = d->headerStyleUtil.spamStatus(message);
    if (!spamHtml.isEmpty()) {
        headerObject.insert(QStringLiteral("spamstatusi18n"), i18n("Spam Status:"));
        headerObject.insert(QStringLiteral("spamHTML"), spamHtml);
    }

    headerObject.insert(QStringLiteral("datei18n"), i18n("Date:"));
    headerObject.insert(QStringLiteral("date"), QVariant::fromValue(static_cast<const KMime::Headers::Date *>(message->date())));

    if (message->hasHeader("Resent-From")) {
        headerObject.insert(QStringLiteral("resentfromi18n"), i18n("resent from"));
        headerObject.insert(QStringLiteral("resentfrom"),
                            QVariant::fromValue(HeaderStyleUtil::resentFromList(message)));
    }

    if (message->hasHeader("Resent-To")) {
        auto resentTo = HeaderStyleUtil::resentToList(message);
        headerObject.insert(QStringLiteral("resenttoi18n"),
                            i18np("receiver was", "receivers were", resentTo->mailboxes().count()));
        headerObject.insert(QStringLiteral("resentto"),
                            QVariant::fromValue(HeaderStyleUtil::resentToList(message)));
    }

    if (auto organization = message->organization(false)) {
        headerObject.insert(QStringLiteral("organization"),
                            HeaderStyleUtil::strToHtml(organization->asUnicodeString()));
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
    const QColor activeColor
        = KColorScheme(QPalette::Active, KColorScheme::Selection).background().color();
    QColor activeColorDark = activeColor.darker(130);
    // reverse colors for encapsulated
    if (!style->isTopLevel()) {
        activeColorDark = activeColor.darker(50);
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

    for (QString header : qAsConst(displayExtraHeaders)) {
        const QByteArray baHeader = header.toLocal8Bit();
        if (auto hrd = message->headerByType(baHeader.constData())) {
            //Grantlee doesn't support '-' in variable name => remove it.
            header = header.remove(QLatin1Char('-'));
            headerObject.insert(header, hrd->asUnicodeString());
        }
    }

    headerObject.insert(QStringLiteral("vcardi18n"), i18n("[vcard]"));
    headerObject.insert(QStringLiteral("readOnlyMessage"), style->readOnlyMessage());

    const QString attachmentHtml = style->attachmentHtml();
    const bool messageHasAttachment = KMime::hasAttachment(message) || !attachmentHtml.isEmpty();
    headerObject.insert(QStringLiteral("hasAttachment"), messageHasAttachment);
    headerObject.insert(QStringLiteral("attachmentHtml"), attachmentHtml);

    if (messageHasAttachment) {
        const QString iconPath
            = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral(
                                                                     "mail-attachment"),
                                                                 KIconLoader::Toolbar);
        const QString html = QStringLiteral("<img height=\"%2\" width=\"%2\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), QString::number(d->iconSize));
        headerObject.insert(QStringLiteral("attachmentIcon"), html);
    }

    const bool messageIsSigned = KMime::isSigned(message);
    headerObject.insert(QStringLiteral("messageIsSigned"), messageIsSigned);
    if (messageIsSigned) {
        const QString iconPath
            = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral(
                                                                     "mail-signed"),
                                                                 KIconLoader::Toolbar);
        const QString html = QStringLiteral("<img height=\"%2\" width=\"%2\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), QString::number(d->iconSize));
        headerObject.insert(QStringLiteral("signedIcon"), html);
    }

    const bool messageIsEncrypted = KMime::isEncrypted(message);
    headerObject.insert(QStringLiteral("messageIsEncrypted"), messageIsEncrypted);
    if (messageIsEncrypted) {
        const QString iconPath
            = MessageViewer::IconNameCache::instance()->iconPath(QStringLiteral(
                                                                     "mail-encrypted"),
                                                                 KIconLoader::Toolbar);
        const QString html = QStringLiteral("<img height=\"%2\" width=\"%2\" src=\"%1\"></a>").arg(QUrl::fromLocalFile(iconPath).url(), QString::number(d->iconSize));
        headerObject.insert(QStringLiteral("encryptedIcon"), html);
    }

    const bool messageHasSecurityInfo = messageIsEncrypted || messageIsSigned;
    headerObject.insert(QStringLiteral("messageHasSecurityInfo"), messageHasSecurityInfo);
    headerObject.insert(QStringLiteral("messageHasSecurityInfoI18n"), i18n("Security:"));

    QVariantHash mapping;
    mapping.insert(QStringLiteral("header"), headerObject);
    Grantlee::Context context(mapping);

    return headerTemplate->render(&context);
}
