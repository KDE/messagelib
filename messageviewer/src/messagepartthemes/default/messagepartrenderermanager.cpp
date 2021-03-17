/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>

#include <QGuiApplication>
#ifndef COMPILE_WITH_UNITY_CMAKE_SUPPORT
Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(const QGpgME::Protocol *)
Q_DECLARE_METATYPE(GpgME::Key)
#endif

// Read-only introspection of GpgME::DecryptionResult::Recipient object.
GRANTLEE_BEGIN_LOOKUP(GpgME::DecryptionResult::Recipient)
if (property == QLatin1String("keyID")) {
    return QString::fromLatin1(object.keyID());
}
GRANTLEE_END_LOOKUP
// Read-only introspection of QGpgME::Protocol object.
namespace Grantlee
{
template<> inline QVariant TypeAccessor<const QGpgME::Protocol *>::lookUp(const QGpgME::Protocol *const object, const QString &property)
{
    if (property == QLatin1String("name")) {
        return object->name();
    } else if (property == QLatin1String("displayName")) {
        return object->displayName();
    }
    return QVariant();
}
}

// Read-only introspection of std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> object.
namespace Grantlee
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
    return QVariant();
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

    QString layoutDirection() const
    {
        return QGuiApplication::isRightToLeft() ? QStringLiteral("rtl") : QStringLiteral("ltr");
    }

    int iconSize() const
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
    Grantlee::registerMetaType<GpgME::DecryptionResult::Recipient>();
    Grantlee::registerMetaType<const QGpgME::Protocol *>();
    Grantlee::registerMetaType<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key>>();
    m_engine = new GrantleeTheme::Engine;
    const auto libraryPaths = QCoreApplication::libraryPaths();
    for (const auto &p : libraryPaths) {
        m_engine->addPluginPath(p + QStringLiteral("/messageviewer"));
    }
    m_engine->addDefaultLibrary(QStringLiteral("messageviewer_grantlee_extension"));
    m_engine->localizer()->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));

    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(new Grantlee::FileSystemTemplateLoader());
    loader->setTemplateDirs({QStringLiteral(":/")});
    m_engine->addTemplateLoader(loader);
}

Grantlee::Template MessagePartRendererManager::loadByName(const QString &name)
{
    Grantlee::Template t = m_engine->loadByName(name);
    if (t->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << t->errorString() << ". Searched in subdir mimetreeparser/themes/default in these locations"
                                     << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }
    return t;
}

Grantlee::Context MessagePartRendererManager::createContext()
{
    Grantlee::Context c;

    // careful, m_engine->localizer() is actually a factory function!
    auto localizer = m_engine->localizer();
    localizer->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));
    c.setLocalizer(localizer);

    c.insert(QStringLiteral("global"), m_globalContext);
    return c;
}

#include "messagepartrenderermanager.moc"
