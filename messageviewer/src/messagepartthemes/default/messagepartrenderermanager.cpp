/*
   Copyright (c) 2016 Laurent Montel <montel@kde.org>

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


#include "messagepartrenderermanager.h"
#include "messageviewer_debug.h"
#include <KIconLoader>
#include <QStandardPaths>
#include <GrantleeTheme/GrantleeKi18nLocalizer>
#include <GrantleeTheme/QtResourceTemplateLoader>


#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>

#include <Libkleo/CryptoBackend>
#include <Libkleo/CryptoBackendFactory>


#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>

Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(const Kleo::CryptoBackend::Protocol *)
// Read-only introspection of GpgME::DecryptionResult::Recipient object.
GRANTLEE_BEGIN_LOOKUP(GpgME::DecryptionResult::Recipient)
if (property == QStringLiteral("keyID"))
{
    return QString::fromLatin1(object.keyID());
}
GRANTLEE_END_LOOKUP
// Read-only introspection of Kleo::CryptoBackend::Protocol object.
namespace Grantlee
{
template<>
inline QVariant TypeAccessor<const Kleo::CryptoBackend::Protocol *>::lookUp(const Kleo::CryptoBackend::Protocol *const object, const QString &property)
{
    if (property == QStringLiteral("name")) {
        return object->name();
    } else if (property == QStringLiteral("displayName")) {
        return object->displayName();
    }
    return QVariant();
}
}



using namespace MessageViewer;

class MessagePartRendererManagerInstancePrivate
{
public:
    MessagePartRendererManagerInstancePrivate()
        : manager(new MessagePartRendererManager)
    {
    }

    ~MessagePartRendererManagerInstancePrivate()
    {
        delete manager;
    }

    MessagePartRendererManager *manager;
};

Q_GLOBAL_STATIC(MessagePartRendererManagerInstancePrivate, sInstance)


MessagePartRendererManager::MessagePartRendererManager(QObject *parent)
    : QObject(parent),
      m_engine(Q_NULLPTR),
      m_localized(Q_NULLPTR)
{
    initializeRenderer();
}

MessagePartRendererManager::~MessagePartRendererManager()
{
    delete m_engine;
}

MessageViewer::MessagePartRendererManager *MessagePartRendererManager::self()
{
    return sInstance->manager;
}

void MessagePartRendererManager::initializeRenderer()
{
    Grantlee::registerMetaType<GpgME::DecryptionResult::Recipient>();
    Grantlee::registerMetaType<const Kleo::CryptoBackend::Protocol *>();
    m_engine = new Grantlee::Engine;
    m_engine->setSmartTrimEnabled(true);
    m_engine->addDefaultLibrary(QStringLiteral("grantlee_i18n"));
    m_engine->addDefaultLibrary(QStringLiteral("grantlee_scriptabletags"));
    m_engine->addDefaultLibrary(QStringLiteral("kde_grantlee_plugin"));

    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(new GrantleeTheme::QtResourceTemplateLoader());
    m_engine->addTemplateLoader(loader);
    if (!m_localized) {
        m_localized.reset(new GrantleeTheme::GrantleeKi18nLocalizer());
    }
}

Grantlee::Template MessagePartRendererManager::loadByName(const QString &name)
{
    Grantlee::Template t = m_engine->loadByName(name);
    if (t->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << t->errorString() << ". Searched in subdir mimetreeparser/themes/default in these locations" << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }
    return t;
}

Grantlee::Context MessagePartRendererManager::createContext()
{
    Grantlee::Context c;
    m_localized->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));
    c.setLocalizer(m_localized);
    return c;
}
