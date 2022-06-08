/*
   SPDX-FileCopyrightText: 2016-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepartrenderermanager.h"
#include "messageviewer_debug.h"
#include <GrantleeTheme/GrantleeKi18nLocalizer>
#include <GrantleeTheme/GrantleeThemeEngine>
#include <KIconLoader>
#include <QStandardPaths>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/key.h>
#include <gpgme++/verificationresult.h>

#include <QGpgME/Protocol>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>
#else
#include <KTextTemplate/context.h>
#include <KTextTemplate/engine.h>
#include <KTextTemplate/metatype.h>
#include <KTextTemplate/templateloader.h>
#endif

#include <QGuiApplication>
Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(const QGpgME::Protocol *)
Q_DECLARE_METATYPE(GpgME::Key)

// Read-only introspection of GpgME::DecryptionResult::Recipient object.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
GRANTLEE_BEGIN_LOOKUP(GpgME::DecryptionResult::Recipient)
#else
KTEXTTEMPLATE_BEGIN_LOOKUP(GpgME::DecryptionResult::Recipient)
#endif
if (property == QLatin1String("keyID")) {
    return QString::fromLatin1(object.keyID());
}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
GRANTLEE_END_LOOKUP
#else
KTEXTTEMPLATE_END_LOOKUP
#endif
// Read-only introspection of QGpgME::Protocol object.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
namespace Grantlee
#else
namespace KTextTemplate
#endif
{
template<>
inline QVariant TypeAccessor<const QGpgME::Protocol *>::lookUp(const QGpgME::Protocol *const object, const QString &property)
{
    if (property == QLatin1String("name")) {
        return object->name();
    } else if (property == QLatin1String("displayName")) {
        return object->displayName();
    }
    return {};
}
}

// Read-only introspection of std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> object.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
namespace Grantlee
#else
namespace KTextTemplate
#endif
{
template<>
inline QVariant
TypeAccessor<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> &>::lookUp(std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> const &object,
                                                                                  const QString &property)
{
    if (property == QLatin1String("keyID")) {
        return QString::fromLatin1(object.first.keyID());
    }
    if (property == QLatin1String("id")) {
        return QString::fromUtf8(object.second.userID(0).id());
    }
    if (property == QLatin1String("mainID")) {
        return QString::fromLatin1(object.second.keyID());
    }
    return {};
}
}

namespace MessageViewer
{
class GlobalContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dir READ layoutDirection CONSTANT)
    Q_PROPERTY(int iconSize READ iconSize CONSTANT)
public:
    explicit GlobalContext(QObject *parent)
        : QObject(parent)
    {
    }

    Q_REQUIRED_RESULT QString layoutDirection() const
    {
        return QGuiApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
    }

    Q_REQUIRED_RESULT int iconSize() const
    {
        return KIconLoader::global()->currentSize(KIconLoader::Desktop);
    }
};
}

using namespace MessageViewer;

MessagePartRendererManager::MessagePartRendererManager(QObject *parent)
    : QObject(parent)
    , m_globalContext(new GlobalContext(this))
{
    initializeRenderer();
}

MessagePartRendererManager::~MessagePartRendererManager()
{
    delete m_engine;
}

MessagePartRendererManager *MessagePartRendererManager::self()
{
    static MessagePartRendererManager s_self;
    return &s_self;
}

void MessagePartRendererManager::initializeRenderer()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::registerMetaType<GpgME::DecryptionResult::Recipient>();
    Grantlee::registerMetaType<const QGpgME::Protocol *>();
    Grantlee::registerMetaType<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key>>();
#else
    KTextTemplate::registerMetaType<GpgME::DecryptionResult::Recipient>();
    KTextTemplate::registerMetaType<const QGpgME::Protocol *>();
    KTextTemplate::registerMetaType<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key>>();
#endif
    m_engine = new GrantleeTheme::Engine;
    const auto libraryPaths = QCoreApplication::libraryPaths();
    const QString subPath = QStringLiteral("/pim" QT_STRINGIFY(QT_VERSION_MAJOR)) + QStringLiteral("/messageviewer");
    for (const auto &p : libraryPaths) {
        m_engine->addPluginPath(p + subPath);
    }
    m_engine->addDefaultLibrary(QStringLiteral("messageviewer_grantlee_extension"));
    m_engine->localizer()->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(new Grantlee::FileSystemTemplateLoader());
#else
    auto loader = QSharedPointer<KTextTemplate::FileSystemTemplateLoader>(new KTextTemplate::FileSystemTemplateLoader());
#endif
    loader->setTemplateDirs({QStringLiteral(":/")});
    m_engine->addTemplateLoader(loader);
}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Grantlee::Template MessagePartRendererManager::loadByName(const QString &name)
#else
KTextTemplate::Template MessagePartRendererManager::loadByName(const QString &name)
#endif
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::Template t = m_engine->loadByName(name);
#else
    KTextTemplate::Template t = m_engine->loadByName(name);
#endif
    if (t->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << t->errorString() << ". Searched in subdir mimetreeparser/themes/default in these locations"
                                     << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }
    return t;
}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Grantlee::Context MessagePartRendererManager::createContext()
#else
KTextTemplate::Context MessagePartRendererManager::createContext()
#endif
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::Context c;
#else
    KTextTemplate::Context c;
#endif

    // careful, m_engine->localizer() is actually a factory function!
    auto localizer = m_engine->localizer();
    localizer->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));
    c.setLocalizer(localizer);

    c.insert(QStringLiteral("global"), m_globalContext);
    return c;
}

#include "messagepartrenderermanager.moc"
