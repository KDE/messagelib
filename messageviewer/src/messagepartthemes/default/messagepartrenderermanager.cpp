/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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
#include <GrantleeTheme/GrantleeThemeEngine>

#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>

#include <QGpgME/Protocol>

#include <grantlee/context.h>
#include <grantlee/engine.h>
#include <grantlee/metatype.h>
#include <grantlee/templateloader.h>

Q_DECLARE_METATYPE(GpgME::DecryptionResult::Recipient)
Q_DECLARE_METATYPE(const QGpgME::Protocol *)
// Read-only introspection of GpgME::DecryptionResult::Recipient object.
GRANTLEE_BEGIN_LOOKUP(GpgME::DecryptionResult::Recipient)
if (property == QStringLiteral("keyID")) {
    return QString::fromLatin1(object.keyID());
}
GRANTLEE_END_LOOKUP
// Read-only introspection of QGpgME::Protocol object.
namespace Grantlee {
template<>
inline QVariant TypeAccessor<const QGpgME::Protocol *>::lookUp(const QGpgME::Protocol *const object,
                                                               const QString &property)
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

MessagePartRendererManager::MessagePartRendererManager(QObject *parent)
    : QObject(parent)
    , m_engine(nullptr)
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
    m_engine = new GrantleeTheme::Engine;
    m_engine->localizer()->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));

    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>(
        new GrantleeTheme::QtResourceTemplateLoader());
    m_engine->addTemplateLoader(loader);

    mCurrentIconSize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
}

Grantlee::Template MessagePartRendererManager::loadByName(const QString &name)
{
    Grantlee::Template t = m_engine->loadByName(name);
    if (t->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << t->errorString()
                                     <<
            ". Searched in subdir mimetreeparser/themes/default in these locations"
                                     << QStandardPaths::standardLocations(
            QStandardPaths::GenericDataLocation);
    }
    return t;
}

Grantlee::Context MessagePartRendererManager::createContext()
{
    Grantlee::Context c;
    m_engine->localizer()->setApplicationDomain(QByteArrayLiteral("libmessageviewer"));

    c.setLocalizer(m_engine->localizer());
    return c;
}

int MessagePartRendererManager::iconCurrentSize() const
{
    return mCurrentIconSize;
}
