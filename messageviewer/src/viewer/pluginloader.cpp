#include "pluginloader.h"

#include "bodypartformatterfactory.h"
#include "bodypartformatterfactory_p.h"
#include "urlhandlermanager.h"
#include "messageviewer_debug.h"

#include "interfaces/bodypartformatter.h"

using namespace MessageViewer::PrivateBodyPartFormatterFactory;
using namespace MessageViewer;

namespace
{

DEFINE_PLUGIN_LOADER(BodyPartFormatterPluginLoader,
                     Interface::BodyPartFormatterPlugin,
                     "create_bodypart_formatter_plugin",
                     "messageviewer/plugins/bodypartformatter/")

}

void MessageViewer::PrivateBodyPartFormatterFactory::loadPlugins()
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
            insertBodyPartFormatter(type, subtype, bfp);
        }
        const Interface::BodyPartURLHandler *handler;
        for (int i = 0; (handler = plugin->urlHandler(i)); ++i) {
            URLHandlerManager::instance()->registerHandler(handler);
        }
    }
}