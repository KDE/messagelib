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
#include "pluginloader.h"
#include "urlhandlermanager.h"
#include "messageviewer_debug.h"

#include <MimeTreeParser/BodyPartFormatter>

using namespace MessageViewer;

namespace
{

DEFINE_PLUGIN_LOADER(BodyPartFormatterPluginLoader,
                     Interface::BodyPartFormatterPlugin,
                     "create_bodypart_formatter_plugin",
                     "messageviewer/plugins/bodypartformatter/")

}

BodyPartFormatterFactory::BodyPartFormatterFactory()
    : BodyPartFormatterBaseFactory()
{
}

BodyPartFormatterFactory::~BodyPartFormatterFactory()
{
}

void BodyPartFormatterFactory::loadPlugins()
{
    const BodyPartFormatterPluginLoader *pl = BodyPartFormatterPluginLoader::instance();
    if (!pl) {
        qCWarning(MESSAGEVIEWER_LOG) << "BodyPartFormatterFactory: cannot instantiate plugin loader!";
        return;
    }
    const QStringList types = pl->types();
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
            qCDebug(MESSAGEVIEWER_LOG) << "plugin for " << type << subtype;
            insert(type, subtype, bfp);
        }
        const Interface::BodyPartURLHandler *handler;
        for (int i = 0; (handler = plugin->urlHandler(i)); ++i) {
            URLHandlerManager::instance()->registerHandler(handler);
        }
    }
}