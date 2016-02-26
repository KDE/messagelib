/*
    Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

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

#include "pluginloader.h"

#include "bodypartformatterfactory.h"
#include "urlhandlermanager.h"
#include "messageviewer_debug.h"

#include "interfaces/bodypartformatter.h"

using namespace MessageViewer;

namespace
{

DEFINE_PLUGIN_LOADER(BodyPartFormatterPluginLoader,
                     Interface::BodyPartFormatterPlugin,
                     "create_bodypart_formatter_plugin",
                     "messageviewer/plugins/bodypartformatter/")

}

void BodyPartFormatterFactory::loadPlugins()
{
    const BodyPartFormatterPluginLoader *pl = BodyPartFormatterPluginLoader::instance();
    if (!pl) {
        qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: cannot instantiate plugin loader!";
        return;
    }
    const QStringList types = pl->types();
    qDebug() << "BodyPartFormatterFactory: found" << types.size() << "plugins.";
    qCDebug(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: found" << types.size() << "plugins.";
    for (QStringList::const_iterator it = types.begin(); it != types.end(); ++it) {
        const Interface::BodyPartFormatterPlugin *plugin = pl->createForName(*it);
        if (!plugin) {
            qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << *it << "is not valid!";
            continue;
        }
        const Interface::BodyPartFormatter *bfp;
        for (int i = 0; (bfp = plugin->bodyPartFormatter(i)); ++i) {
            const char *type = plugin->type(i);
            if (!type || !*type) {
                qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << *it
                                             << "returned empty type specification for index"
                                             << i;
                break;
            }
            const char *subtype = plugin->subtype(i);
            if (!subtype || !*subtype) {
                qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: plugin" << *it
                                             << "returned empty subtype specification for index"
                                             << i;
                break;
            }
            qDebug() << "plugin for " << type << subtype;
            insert(type, subtype, bfp);
        }
        const Interface::BodyPartURLHandler *handler;
        for (int i = 0; (handler = plugin->urlHandler(i)); ++i) {
            URLHandlerManager::instance()->registerHandler(handler);
        }
    }
}