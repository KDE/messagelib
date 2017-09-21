/*
    bodypartformatterfactory.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2004 Marc Mutz <mutz@kde.org>,
                       Ingo Kloecker <kloecker@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "bodypartformatterfactory.h"
#include "urlhandlermanager.h"
#include "messageviewer_debug.h"

#include <MimeTreeParser/BodyPartFormatter>

#include <KPluginLoader>

#include <QJsonArray>
#include <QJsonObject>

using namespace MessageViewer;

BodyPartFormatterFactory::BodyPartFormatterFactory()
    : MimeTreeParser::BodyPartFormatterBaseFactory()
{
}

BodyPartFormatterFactory::~BodyPartFormatterFactory()
{
}

void BodyPartFormatterFactory::loadPlugins()
{
    KPluginLoader::forEachPlugin(QStringLiteral("messageviewer/bodypartformatter"), [this](const QString &path) {
        QPluginLoader loader(path);
        const auto formatterData = loader.metaData().value(QLatin1String("MetaData")).toObject().value(QLatin1String("formatter")).toArray();
        if (formatterData.isEmpty()) {
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin" << path << "has no meta data.";
            return;
        }

        auto plugin = qobject_cast<MimeTreeParser::Interface::BodyPartFormatterPlugin *>(loader.instance());
        if (!plugin) {
            qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << path
                                         << "is not valid!";
            return;
        }

        const MimeTreeParser::Interface::BodyPartFormatter *bfp = nullptr;
        for (int i = 0; (bfp = plugin->bodyPartFormatter(i)) && i < formatterData.size(); ++i) {
            const auto metaData = formatterData.at(i).toObject();
            const auto type = metaData.value(QLatin1String("type")).toString().toUtf8();
            if (type.isEmpty()) {
                qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << path
                                             << "returned empty type specification for index"
                                             << i;
                break;
            }
            const auto subtype = metaData.value(QLatin1String("subtype")).toString().toUtf8();
            if (subtype.isEmpty()) {
                qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << path
                                             << "returned empty subtype specification for index"
                                             << i;
                break;
            }
            qCDebug(MESSAGEVIEWER_LOG) << "plugin for " << type << subtype;
            insert(type, subtype, bfp);
        }

        const MimeTreeParser::Interface::BodyPartURLHandler *handler = nullptr;
        for (int i = 0; (handler = plugin->urlHandler(i)); ++i) {
            URLHandlerManager::instance()->registerHandler(handler);
        }
    });
}
