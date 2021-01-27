/*
    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "messagepartrendererfactory.h"
#include "messagepartrendererfactory_p.h"
#include "messagepartrenderplugin.h"
#include "viewer/urlhandlermanager.h"

#include "messagepartrendererbase.h"
#include "messageviewer_debug.h"

#include "plugins/attachmentmessagepartrenderer.h"
#include "plugins/messagepartrenderer.h"
#include "plugins/textmessagepartrenderer.h"

#include <KPluginLoader>

#include <QJsonArray>
#include <QMimeDatabase>
#include <algorithm>

using namespace MessageViewer;

MessagePartRendererFactoryPrivate::~MessagePartRendererFactoryPrivate()
{
    QHashIterator<QByteArray, std::vector<RendererInfo>> i(m_renderers);
    while (i.hasNext()) {
        i.next();
        auto renderInfo = i.value();
        renderInfo.erase(renderInfo.begin(), renderInfo.end());
    }
}

void MessagePartRendererFactoryPrivate::setup()
{
    if (m_renderers.isEmpty()) {
        initialize_builtin_renderers();
        loadPlugins();
    }
    Q_ASSERT(!m_renderers.isEmpty());
}

void MessagePartRendererFactoryPrivate::loadPlugins()
{
    if (m_pluginSubdir.isEmpty()) {
        return;
    }

    KPluginLoader::forEachPlugin(m_pluginSubdir, [this](const QString &path) {
        QPluginLoader loader(path);
        const auto pluginData = loader.metaData().value(QLatin1String("MetaData")).toObject().value(QLatin1String("renderer")).toArray();
        if (pluginData.isEmpty()) {
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin" << path << "has no meta data.";
            return;
        }

        auto plugin = qobject_cast<MessagePartRenderPlugin *>(loader.instance());
        if (!plugin) {
            qCWarning(MESSAGEVIEWER_LOG) << path << "is not a MessagePartRendererPlugin";
            return;
        }

        MessagePartRendererBase *renderer = nullptr;
        for (int i = 0; (renderer = plugin->renderer(i)) && i < pluginData.size(); ++i) {
            const auto metaData = pluginData.at(i).toObject();
            const auto type = metaData.value(QLatin1String("type")).toString().toUtf8();
            if (type.isEmpty()) {
                qCWarning(MESSAGEVIEWER_LOG) << path << "returned empty type specification for index" << i;
                break;
            }
            const auto mimetype = metaData.value(QLatin1String("mimetype")).toString().toLower();
            // priority should always be higher than the built-in ones, otherwise what's the point?
            const auto priority = metaData.value(QLatin1String("priority")).toInt() + 100;
            qCDebug(MESSAGEVIEWER_LOG) << "renderer plugin for " << type << mimetype << priority;
            insert(type, renderer, mimetype, priority);
        }

        const Interface::BodyPartURLHandler *handler = nullptr;
        for (int i = 0; (handler = plugin->urlHandler(i)); ++i) {
            const auto metaData = pluginData.at(i).toObject();
            const auto mimeType = metaData.value(QLatin1String("mimetype")).toString().toLower();
            URLHandlerManager::instance()->registerHandler(handler, mimeType);
        }
    });
}

void MessagePartRendererFactoryPrivate::initialize_builtin_renderers()
{
    insert("MimeTreeParser::MessagePart", new MessagePartRenderer());
    insert("MimeTreeParser::TextMessagePart", new TextMessagePartRenderer());
    insert("MimeTreeParser::AttachmentMessagePart", new AttachmentMessagePartRenderer());
}

void MessagePartRendererFactoryPrivate::insert(const QByteArray &type, MessagePartRendererBase *renderer, const QString &mimeType, int priority)
{
    if (type.isEmpty() || !renderer) {
        return;
    }

    QMimeDatabase db;
    const auto mt = db.mimeTypeForName(mimeType);

    RendererInfo info;
    info.renderer.reset(renderer);
    info.mimeType = mt.isValid() ? mt.name() : mimeType;
    info.priority = priority;

    auto &v = m_renderers[type];
    v.push_back(info);
}

MessagePartRendererFactory::MessagePartRendererFactory()
    : d(new MessagePartRendererFactoryPrivate)
{
}

MessagePartRendererFactory::~MessagePartRendererFactory() = default;

void MessagePartRendererFactory::setPluginPath(const QString &subdir)
{
    d->m_pluginSubdir = subdir;
}

MessagePartRendererFactory *MessagePartRendererFactory::instance()
{
    static MessagePartRendererFactory s_instance;
    return &s_instance;
}

QVector<MessagePartRendererBase *> MessagePartRendererFactory::renderersForPart(const QMetaObject *mo, const MimeTreeParser::MessagePartPtr &mp) const
{
    d->setup();

    const auto mtName = mp->content() ? QString::fromUtf8(mp->content()->contentType()->mimeType().toLower()) : QString();
    QMimeDatabase db;
    const auto mt = db.mimeTypeForName(mtName);
    auto ancestors = mt.allAncestors();
    if (mt.isValid() || !mtName.isEmpty()) {
        ancestors.prepend(mt.isValid() ? mt.name() : mtName);
    }

    auto candidates = d->m_renderers.value(mo->className());

    // remove candidates with a mimetype set that don't match the mimetype of the part
    candidates.erase(std::remove_if(candidates.begin(),
                                    candidates.end(),
                                    [ancestors](const RendererInfo &info) {
                                        if (info.mimeType.isEmpty()) {
                                            return false;
                                        }
                                        return !ancestors.contains(info.mimeType);
                                    }),
                     candidates.end());

    // sort most specific mimetpypes first
    std::stable_sort(candidates.begin(), candidates.end(), [ancestors](const RendererInfo &lhs, const RendererInfo &rhs) {
        if (lhs.mimeType == rhs.mimeType) {
            return lhs.priority > rhs.priority;
        }
        if (lhs.mimeType.isEmpty()) {
            return false;
        }
        if (rhs.mimeType.isEmpty()) {
            return true;
        }
        return ancestors.indexOf(lhs.mimeType) < ancestors.indexOf(rhs.mimeType);
    });

    QVector<MessagePartRendererBase *> r;
    r.reserve(candidates.size());
    for (const auto &candidate : candidates) {
        r.push_back(candidate.renderer.data());
    }
    return r;
}
