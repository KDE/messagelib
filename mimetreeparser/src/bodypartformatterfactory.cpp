/*
    bodypartformatterfactory.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "bodypartformatterfactory.h"
#include "bodypartformatterfactory_p.h"
#include "interfaces/bodypartformatter.h"
#include "mimetreeparser_debug.h"

#include <KPluginMetaData>
#include <QJsonArray>
#include <QMimeDatabase>

#include <QPluginLoader>
#include <cassert>

using namespace MimeTreeParser;

BodyPartFormatterFactoryPrivate::BodyPartFormatterFactoryPrivate(BodyPartFormatterFactory *factory)
    : q(factory)
{
}

BodyPartFormatterFactoryPrivate::~BodyPartFormatterFactoryPrivate()
{
    QHashIterator<QString, std::vector<FormatterInfo>> i(registry);
    while (i.hasNext()) {
        i.next();
        auto formatterInfo = i.value();
        formatterInfo.erase(formatterInfo.begin(), formatterInfo.end());
    }
}

void BodyPartFormatterFactoryPrivate::setup()
{
    if (registry.empty()) {
        messageviewer_create_builtin_bodypart_formatters();
        q->loadPlugins();
    }
    assert(!registry.empty());
}

void BodyPartFormatterFactoryPrivate::insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority)
{
    if (mimeType.isEmpty() || !formatter) {
        return;
    }

    QMimeDatabase db;
    const auto mt = db.mimeTypeForName(mimeType);
    FormatterInfo info;
    info.formatter = formatter;
    info.priority = priority;

    auto &v = registry[mt.isValid() ? mt.name() : mimeType];
    v.push_back(info);
    std::stable_sort(v.begin(), v.end(), [](FormatterInfo lhs, FormatterInfo rhs) {
        return lhs.priority > rhs.priority;
    });
}

void BodyPartFormatterFactoryPrivate::appendFormattersForType(const QString &mimeType, QList<const Interface::BodyPartFormatter *> &formatters)
{
    const auto it = registry.constFind(mimeType);
    if (it == registry.constEnd()) {
        return;
    }
    for (const auto &f : it.value()) {
        formatters.push_back(f.formatter);
    }
}

BodyPartFormatterFactory::BodyPartFormatterFactory()
    : d(new BodyPartFormatterFactoryPrivate(this))
{
}

BodyPartFormatterFactory::~BodyPartFormatterFactory() = default;

BodyPartFormatterFactory *BodyPartFormatterFactory::instance()
{
    static BodyPartFormatterFactory s_instance;
    return &s_instance;
}

void BodyPartFormatterFactory::insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority)
{
    d->insert(mimeType.toLower(), formatter, priority);
}

QList<const Interface::BodyPartFormatter *> BodyPartFormatterFactory::formattersForType(const QString &mimeType) const
{
    QList<const Interface::BodyPartFormatter *> r;
    d->setup();

    QMimeDatabase db;
    std::vector<QString> processedTypes;
    processedTypes.push_back(mimeType.toLower());

    // add all formatters we have along the mimetype hierarchy
    for (std::size_t i = 0; i < processedTypes.size(); ++i) {
        const auto mt = db.mimeTypeForName(processedTypes[i]);
        if (mt.isValid()) {
            processedTypes[i] = mt.name(); // resolve alias if necessary
        }
        if (processedTypes[i] == QLatin1StringView("application/octet-stream")) { // we'll deal with that later
            continue;
        }
        d->appendFormattersForType(processedTypes[i], r);

        const auto parentTypes = mt.parentMimeTypes();
        for (const auto &parentType : parentTypes) {
            if (std::find(processedTypes.begin(), processedTypes.end(), parentType) != processedTypes.end()) {
                continue;
            }
            processedTypes.push_back(parentType);
        }
    }

    // make sure we always have a suitable fallback formatter
    if (mimeType.startsWith(QLatin1StringView("multipart/"))) {
        if (mimeType != QLatin1StringView("multipart/mixed")) {
            d->appendFormattersForType(QStringLiteral("multipart/mixed"), r);
        }
    } else {
        d->appendFormattersForType(QStringLiteral("application/octet-stream"), r);
    }
    assert(!r.empty());
    return r;
}

void BodyPartFormatterFactory::loadPlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/messageviewer/bodypartformatter"));
    for (const auto &md : plugins) {
        const auto formatterData = md.rawData().value(QLatin1StringView("formatter")).toArray();
        if (formatterData.isEmpty()) {
            continue;
        }
        QPluginLoader loader(md.fileName());
        auto plugin = qobject_cast<MimeTreeParser::Interface::BodyPartFormatterPlugin *>(loader.instance());
        if (!plugin) {
            continue;
        }

        const MimeTreeParser::Interface::BodyPartFormatter *bfp = nullptr;
        for (int i = 0; (bfp = plugin->bodyPartFormatter(i)) && i < formatterData.size(); ++i) {
            const auto metaData = formatterData.at(i).toObject();
            const auto mimetype = metaData.value(QLatin1StringView("mimetype")).toString();
            if (mimetype.isEmpty()) {
                qCWarning(MIMETREEPARSER_LOG) << "BodyPartFormatterFactory: plugin" << md.fileName() << "returned empty mimetype specification for index" << i;
                break;
            }
            // priority should always be higher than the built-in ones, otherwise what's the point?
            const auto priority = metaData.value(QLatin1StringView("priority")).toInt() + 100;
            qCDebug(MIMETREEPARSER_LOG) << "plugin for " << mimetype << priority;
            insert(mimetype, bfp, priority);
        }
    }
}
