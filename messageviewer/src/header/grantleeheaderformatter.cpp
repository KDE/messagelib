/*
   SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "grantleeheaderformatter.h"
using namespace Qt::Literals::StringLiterals;

#include "headerstyle_util.h"
#include "settings/messageviewersettings.h"
#include "utils/iconnamecache.h"

#include <MessageCore/StringUtil>
#include <MimeTreeParser/NodeHelper>

#include <KMime/Message>

#include <KColorScheme>
#include <KIconLoader>
#include <KLocalizedString>
#include <KTextTemplate/Engine>
#include <KTextTemplate/MetaType>

using namespace MessageCore;

using namespace MessageViewer;

Q_DECLARE_METATYPE(const KMime::Headers::Generics::AddressList *)
Q_DECLARE_METATYPE(QSharedPointer<KMime::Headers::Generics::AddressList>)
Q_DECLARE_METATYPE(const KMime::Headers::Generics::MailboxList *)
Q_DECLARE_METATYPE(QSharedPointer<KMime::Headers::Generics::MailboxList>)
Q_DECLARE_METATYPE(QDateTime)

// Read-only introspection of KMime::Headers::Generics::AddressList object.
namespace KTextTemplate
{
template<>
inline QVariant TypeAccessor<const KMime::Headers::Generics::AddressList *>::lookUp(const KMime::Headers::Generics::AddressList *const object,
                                                                                    const QString &property)
{
    if (property == QLatin1StringView("nameOnly")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayNameOnly);
    } else if (property == QLatin1StringView("isSet")) {
        return !object->asUnicodeString().isEmpty();
    } else if (property == QLatin1StringView("fullAddress")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayFullAddress);
    } else if (property == QLatin1StringView("str")) {
        return object->asUnicodeString();
    } else if (property.startsWith(QLatin1StringView("expandable"))) {
        const auto &name = property.mid(10);
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(object,
                                                                       MessageCore::StringUtil::DisplayFullAddress,
                                                                       QString(),
                                                                       MessageCore::StringUtil::ShowLink,
                                                                       MessageCore::StringUtil::ExpandableAddresses,
                                                                       u"Full"_s + name + u"AddressList"_s);
        return val;
    }
    return {};
}

template<>
inline QVariant TypeAccessor<QByteArray &>::lookUp(const QByteArray &object, const QString &property)
{
    return object;
}
}
KTEXTTEMPLATE_BEGIN_LOOKUP(QSharedPointer<KMime::Headers::Generics::MailboxList>)
if (property == QLatin1StringView("nameOnly")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayNameOnly);
} else if (property == QLatin1StringView("isSet")) {
    return !object->asUnicodeString().isEmpty();
} else if (property == QLatin1StringView("fullAddress")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayFullAddress);
} else if (property == QLatin1StringView("str")) {
    return object->asUnicodeString();
} else if (property.startsWith(QLatin1StringView("expandable"))) {
    const auto &name = property.mid(10);
    const QString val = MessageCore::StringUtil::emailAddrAsAnchor(object.data(),
                                                                   MessageCore::StringUtil::DisplayFullAddress,
                                                                   QString(),
                                                                   MessageCore::StringUtil::ShowLink,
                                                                   MessageCore::StringUtil::ExpandableAddresses,
                                                                   u"Full"_s + name + u"AddressList"_s);
    return val;
}
KTEXTTEMPLATE_END_LOOKUP

// Read-only introspection of KMime::Headers::Generics::MailboxList object.
namespace KTextTemplate
{
template<>
inline QVariant TypeAccessor<const KMime::Headers::Generics::MailboxList *>::lookUp(const KMime::Headers::Generics::MailboxList *const object,
                                                                                    const QString &property)
{
    if (property == QLatin1StringView("nameOnly")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayNameOnly);
    } else if (property == QLatin1StringView("isSet")) {
        return !object->asUnicodeString().isEmpty();
    } else if (property == QLatin1StringView("fullAddress")) {
        return StringUtil::emailAddrAsAnchor(object, StringUtil::DisplayFullAddress);
    } else if (property == QLatin1StringView("str")) {
        return object->asUnicodeString();
    } else if (property.startsWith(QLatin1StringView("expandable"))) {
        const auto &name = property.mid(10);
        const QString val = MessageCore::StringUtil::emailAddrAsAnchor(object,
                                                                       MessageCore::StringUtil::DisplayFullAddress,
                                                                       QString(),
                                                                       MessageCore::StringUtil::ShowLink,
                                                                       MessageCore::StringUtil::ExpandableAddresses,
                                                                       u"Full"_s + name + u"AddressList"_s);
        return val;
    }
    return {};
}
}
KTEXTTEMPLATE_BEGIN_LOOKUP(QSharedPointer<KMime::Headers::Generics::AddressList>)
if (property == QLatin1StringView("nameOnly")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayNameOnly);
} else if (property == QLatin1StringView("isSet")) {
    return !object->asUnicodeString().isEmpty();
} else if (property == QLatin1StringView("fullAddress")) {
    return StringUtil::emailAddrAsAnchor(object.data(), StringUtil::DisplayFullAddress);
} else if (property == QLatin1StringView("str")) {
    return object->asUnicodeString();
} else if (property.startsWith(QLatin1StringView("expandable"))) {
    const auto &name = property.mid(10);
    const QString val = MessageCore::StringUtil::emailAddrAsAnchor(object.data(),
                                                                   MessageCore::StringUtil::DisplayFullAddress,
                                                                   QString(),
                                                                   MessageCore::StringUtil::ShowLink,
                                                                   MessageCore::StringUtil::ExpandableAddresses,
                                                                   u"Full"_s + name + u"AddressList"_s);
    return val;
}
KTEXTTEMPLATE_END_LOOKUP
namespace KTextTemplate
{
template<>
inline QVariant TypeAccessor<QDateTime &>::lookUp(const QDateTime &object, const QString &property)
{
    MessageViewer::HeaderStyleUtil::HeaderStyleUtilDateFormat dateFormat;
    if (property == QLatin1StringView("str")) {
        return HeaderStyleUtil::dateStr(object);
    } else if (property == QLatin1StringView("short")) {
        dateFormat = MessageViewer::HeaderStyleUtil::ShortDate;
    } else if (property == QLatin1StringView("long")) {
        dateFormat = MessageViewer::HeaderStyleUtil::LongDate;
    } else if (property == QLatin1StringView("fancylong")) {
        dateFormat = MessageViewer::HeaderStyleUtil::FancyLongDate;
    } else if (property == QLatin1StringView("fancyshort")) {
        dateFormat = MessageViewer::HeaderStyleUtil::FancyShortDate;
    } else if (property == QLatin1StringView("localelong")) {
        dateFormat = MessageViewer::HeaderStyleUtil::LongDate;
    } else {
        return {};
    }

    return HeaderStyleUtil::strToHtml(HeaderStyleUtil::dateString(object, dateFormat));
}
}

class Q_DECL_HIDDEN HeaderFormatter
{
public:
    virtual ~HeaderFormatter() = default;

    virtual QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) = 0;
    virtual QString i18nName() = 0;
};

class DefaultHeaderFormatter : public HeaderFormatter
{
public:
    DefaultHeaderFormatter(const QByteArray &h)
        : header(h)
    {
    }

    QString i18nName() override
    {
        if (header == "list-id") {
            return i18n("List-Id:");
        } else {
            return {};
        }
    }

    QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) override
    {
        Q_UNUSED(showEmoticons);
        return nodeHelper->mailHeaderAsBase(header.constData(), message)->asUnicodeString();
    }

private:
    QByteArray header;
};

class SubjectFormatter : public HeaderFormatter
{
public:
    QString i18nName() override
    {
        return i18n("Subject:");
    }

    QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) override
    {
        KTextToHTML::Options flags = KTextToHTML::PreserveSpaces;
        if (showEmoticons) {
            flags |= KTextToHTML::ReplaceSmileys;
        }
        const auto subjectStr = nodeHelper->mailHeaderAsBase("subject", message)->asUnicodeString();

        return HeaderStyleUtil::strToHtml(subjectStr, flags);
    }
};

class GrantleeHeaderDateFormatter : public HeaderFormatter
{
public:
    QString i18nName() override
    {
        return i18n("Date:");
    }

    QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) override
    {
        Q_UNUSED(showEmoticons);
        const auto value = nodeHelper->dateHeader(message);
        return value;
    }
};

class MessageIdFormatter : public HeaderFormatter
{
public:
    QString i18nName() override
    {
        return i18n("Message-Id:");
    }

    QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) override
    {
        const auto messageIdHeader = nodeHelper->mailHeaderAsBase("Message-Id", message);
        if (messageIdHeader != nullptr) {
            return static_cast<const KMime::Headers::MessageID *>(messageIdHeader)->identifier();
        }
        return {};
    }
};

class AddressHeaderFormatter : public HeaderFormatter
{
public:
    AddressHeaderFormatter(const QByteArray &h)
        : header(h)
    {
    }

    QString i18nName() override
    {
        if (header == "to") {
            return i18n("To:");
        } else if (header == "reply-To") {
            return i18n("Reply To:");
        } else if (header == "cc") {
            return i18n("CC:");
        } else if (header == "bcc") {
            return i18n("BCC:");
        } else if (header == "from") {
            return i18n("From:");
        } else if (header == "sender") {
            return i18n("Sender:");
        } else if (header == "resent-From") {
            return i18n("resent from:");
        } else if (header == "resent-To") {
            return i18n("resent to:");
        } else {
            return {};
        }
    }

    QVariant format(KMime::Message *message, MimeTreeParser::NodeHelper *nodeHelper, bool showEmoticons) override
    {
        Q_UNUSED(showEmoticons);
        const auto value = nodeHelper->mailHeaderAsAddressList(header.constData(), message);
        return QVariant::fromValue(value);
    }

protected:
    QByteArray header;
};

class MessageViewer::GrantleeHeaderFormatter::GrantleeHeaderFormatterPrivate
{
public:
    GrantleeHeaderFormatterPrivate()
        : engine(new KTextTemplate::Engine)
    {
        KTextTemplate::registerMetaType<const KMime::Headers::Generics::AddressList *>();
        KTextTemplate::registerMetaType<const KMime::Headers::Generics::MailboxList *>();
        KTextTemplate::registerMetaType<QSharedPointer<KMime::Headers::Generics::MailboxList>>();
        KTextTemplate::registerMetaType<QSharedPointer<KMime::Headers::Generics::AddressList>>();
        KTextTemplate::registerMetaType<QDateTime>();
        KTextTemplate::registerMetaType<QByteArray>();
        iconSize = KIconLoader::global()->currentSize(KIconLoader::Toolbar);
        templateLoader = QSharedPointer<KTextTemplate::FileSystemTemplateLoader>(new KTextTemplate::FileSystemTemplateLoader);
        engine->addTemplateLoader(templateLoader);

        QList<QByteArray> addressHeaders;
        addressHeaders << "to"
                       << "reply-To"
                       << "cc"
                       << "bcc"
                       << "from"
                       << "sender"
                       << "resent-From"
                       << "resent-To";

        for (const auto &header : std::as_const(addressHeaders)) {
            registerHeaderFormatter(header, QSharedPointer<HeaderFormatter>(new AddressHeaderFormatter(header)));
        }

        registerHeaderFormatter("subject", QSharedPointer<HeaderFormatter>(new SubjectFormatter()));
        registerHeaderFormatter("date", QSharedPointer<HeaderFormatter>(new GrantleeHeaderDateFormatter()));
        registerHeaderFormatter("Message-Id", QSharedPointer<HeaderFormatter>(new MessageIdFormatter()));
    }

    ~GrantleeHeaderFormatterPrivate()
    {
        delete engine;
    }

    void registerHeaderFormatter(const QByteArray &header, QSharedPointer<HeaderFormatter> formatter)
    {
        headerFormatter[header] = formatter;
    }
    QSharedPointer<KTextTemplate::FileSystemTemplateLoader> templateLoader;
    KTextTemplate::Engine *const engine;
    QMap<QByteArray, QSharedPointer<HeaderFormatter>> headerFormatter;
    MessageViewer::HeaderStyleUtil headerStyleUtil;
    QColor activeColor;

    int iconSize;
};

GrantleeHeaderFormatter::GrantleeHeaderFormatter()
    : d(new GrantleeHeaderFormatter::GrantleeHeaderFormatterPrivate)
{
}

GrantleeHeaderFormatter::~GrantleeHeaderFormatter() = default;

QString GrantleeHeaderFormatter::toHtml(const GrantleeHeaderFormatter::GrantleeHeaderFormatterSettings &settings) const
{
    QString errorMessage;
    if (!settings.theme.isValid()) {
        errorMessage = i18n("Grantlee theme \"%1\" is not valid.", settings.theme.name());
        return errorMessage;
    }
    d->templateLoader->setTemplateDirs(QStringList() << settings.theme.absolutePath());
    KTextTemplate::Template headerTemplate = d->engine->loadByName(settings.theme.themeFilename());
    if (headerTemplate->error()) {
        errorMessage = headerTemplate->errorString();
        return errorMessage;
    }
    return format(settings.theme.absolutePath(),
                  headerTemplate,
                  settings.theme.displayExtraVariables(),
                  settings.isPrinting,
                  settings.style,
                  settings.message,
                  settings.showEmoticons);
}

QString GrantleeHeaderFormatter::toHtml(const QStringList &displayExtraHeaders,
                                        const QString &absolutPath,
                                        const QString &filename,
                                        const MessageViewer::HeaderStyle *style,
                                        KMime::Message *message,
                                        bool isPrinting) const
{
    d->templateLoader->setTemplateDirs(QStringList() << absolutPath);
    KTextTemplate::Template headerTemplate = d->engine->loadByName(filename);
    if (headerTemplate->error()) {
        return headerTemplate->errorString();
    }
    return format(absolutPath, headerTemplate, displayExtraHeaders, isPrinting, style, message);
}

QString GrantleeHeaderFormatter::format(const QString &absolutePath,
                                        const KTextTemplate::Template &headerTemplate,
                                        const QStringList &displayExtraHeaders,
                                        bool isPrinting,
                                        const MessageViewer::HeaderStyle *style,
                                        KMime::Message *message,
                                        bool showEmoticons) const
{
    QVariantHash headerObject;
    const auto nodeHelper = style->nodeHelper();

    // However, the direction of the message subject within the header is
    // determined according to the contents of the subject itself. Since
    // the "Re:" and "Fwd:" prefixes would always cause the subject to be
    // considered left-to-right, they are ignored when determining its
    // direction.
    const QString absoluteThemePath = QUrl::fromLocalFile(absolutePath + u'/').url();
    headerObject.insert(u"absoluteThemePath"_s, absoluteThemePath);
    headerObject.insert(u"applicationDir"_s, QApplication::isRightToLeft() ? u"rtl"_s : QStringLiteral("ltr"));

    // TODO: use correct subject from nodeHelper->mailHeader
    headerObject.insert(u"subjectDir"_s, d->headerStyleUtil.subjectDirectionString(message));

    QList<QByteArray> defaultHeaders;
    defaultHeaders << "to"
                   << "reply-To"
                   << "cc"
                   << "bcc"
                   << "from"
                   << "sender"
                   << "resent-From"
                   << "resent-To"
                   << "subject"
                   << "organization"
                   << "list-id"
                   << "date"
                   << "Message-Id";

    for (const auto &header : std::as_const(defaultHeaders)) {
        QSharedPointer<HeaderFormatter> formatter;
        if (d->headerFormatter.contains(header)) {
            formatter = d->headerFormatter.value(header);
        } else {
            formatter = QSharedPointer<HeaderFormatter>(new DefaultHeaderFormatter(header));
        }
        const auto i18nName = formatter->i18nName();
        const auto objectName = QString::fromUtf8(header).remove(u'-');
        if (nodeHelper->hasMailHeader(header.constData(), message)) {
            const auto value = formatter->format(message, nodeHelper, showEmoticons);
            headerObject.insert(objectName, value);
        }
        if (!i18nName.isEmpty()) {
            headerObject.insert(objectName + u"i18n"_s, i18nName);
        }
    }

    if (!nodeHelper->hasMailHeader("subject", message)) {
        headerObject.insert(u"subject"_s, i18n("No Subject"));
    }

    const QString spamHtml = d->headerStyleUtil.spamStatus(message);
    if (!spamHtml.isEmpty()) {
        headerObject.insert(u"spamstatusi18n"_s, i18n("Spam Status:"));
        headerObject.insert(u"spamHTML"_s, spamHtml);
    }

    if (!style->vCardName().isEmpty()) {
        headerObject.insert(u"vcardname"_s, style->vCardName());
    }

    if (isPrinting) {
        // provide a bit more left padding when printing
        // kolab/issue3254 (printed mail cut at the left side)
        // Use it just for testing if we are in printing mode
        headerObject.insert(u"isprinting"_s, i18n("Printing mode"));
        headerObject.insert(u"printmode"_s, u"printmode"_s);
    } else {
        headerObject.insert(u"screenmode"_s, u"screenmode"_s);
    }

    // colors depend on if it is encapsulated or not
    QColor fontColor(Qt::white);
    QString linkColor = u"white"_s;

    if (!d->activeColor.isValid()) {
        d->activeColor = KColorScheme(QPalette::Active, KColorScheme::Selection).background().color();
    }
    QColor activeColorDark = d->activeColor.darker(130);
    // reverse colors for encapsulated
    if (!style->isTopLevel()) {
        activeColorDark = d->activeColor.darker(50);
        fontColor = QColor(Qt::black);
        linkColor = u"black"_s;
    }

    // 3D borders
    headerObject.insert(u"activecolordark"_s, activeColorDark.name());
    headerObject.insert(u"fontcolor"_s, fontColor.name());
    headerObject.insert(u"linkcolor"_s, linkColor);

    MessageViewer::HeaderStyleUtil::xfaceSettings xface = d->headerStyleUtil.xface(style, message);
    if (!xface.photoURL.isEmpty()) {
        headerObject.insert(u"photowidth"_s, xface.photoWidth);
        headerObject.insert(u"photoheight"_s, xface.photoHeight);
        headerObject.insert(u"photourl"_s, xface.photoURL);
    }

    for (QString header : std::as_const(displayExtraHeaders)) {
        const QByteArray baHeader = header.toLocal8Bit();
        if (auto hrd = message->headerByType(baHeader.constData())) {
            // Grantlee doesn't support '-' in variable name => remove it.
            header.remove(u'-');
            headerObject.insert(header, hrd->asUnicodeString());
        }
    }

    headerObject.insert(u"vcardi18n"_s, i18n("[vcard]"));
    headerObject.insert(u"readOnlyMessage"_s, style->readOnlyMessage());

    const QString attachmentHtml = style->attachmentHtml();
    const bool messageHasAttachment = !attachmentHtml.isEmpty();
    headerObject.insert(u"hasAttachment"_s, messageHasAttachment);
    headerObject.insert(u"attachmentHtml"_s, attachmentHtml);
    headerObject.insert(u"attachmentI18n"_s, i18n("Attachments:"));

    if (messageHasAttachment) {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(u"mail-attachment"_s, KIconLoader::Toolbar);
        const QString html = u"<img height=\"%2\" width=\"%2\" src=\"%1\"></a>"_s.arg(iconPath, QString::number(d->iconSize));
        headerObject.insert(u"attachmentIcon"_s, html);
    }

    const bool messageIsSigned = KMime::isSigned(message);
    headerObject.insert(u"messageIsSigned"_s, messageIsSigned);
    if (messageIsSigned) {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(u"mail-signed"_s, KIconLoader::Toolbar);
        const QString html = u"<img height=\"%2\" width=\"%2\" src=\"%1\"></a>"_s.arg(iconPath, QString::number(d->iconSize));
        headerObject.insert(u"signedIcon"_s, html);
    }

    const bool messageIsEncrypted = KMime::isEncrypted(message);
    headerObject.insert(u"messageIsEncrypted"_s, messageIsEncrypted);
    if (messageIsEncrypted) {
        const QString iconPath = MessageViewer::IconNameCache::instance()->iconPath(u"mail-encrypted"_s, KIconLoader::Toolbar);
        const QString html = u"<img height=\"%2\" width=\"%2\" src=\"%1\"></a>"_s.arg(iconPath, QString::number(d->iconSize));
        headerObject.insert(u"encryptedIcon"_s, html);
    }

    const bool messageHasSecurityInfo = messageIsEncrypted || messageIsSigned;
    headerObject.insert(u"messageHasSecurityInfo"_s, messageHasSecurityInfo);
    headerObject.insert(u"messageHasSecurityInfoI18n"_s, i18n("Security:"));

    QVariantHash mapping;
    mapping.insert(u"header"_s, headerObject);
    KTextTemplate::Context context(mapping);

    return headerTemplate->render(&context);
}
