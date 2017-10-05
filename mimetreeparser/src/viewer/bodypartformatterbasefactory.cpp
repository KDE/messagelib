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

#include "bodypartformatterbasefactory.h"
#include "bodypartformatterbasefactory_p.h"
#include "mimetreeparser_debug.h"

#include <QMimeDatabase>
#include <QMimeType>

#include <assert.h>

using namespace MimeTreeParser;

BodyPartFormatterBaseFactoryPrivate::BodyPartFormatterBaseFactoryPrivate(BodyPartFormatterBaseFactory *factory)
    : q(factory)
{
}

void BodyPartFormatterBaseFactoryPrivate::setup()
{
    if (registry.empty()) {
        messageviewer_create_builtin_bodypart_formatters();
        q->loadPlugins();
    }
    assert(!registry.empty());
}

void BodyPartFormatterBaseFactoryPrivate::insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority)
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
    std::stable_sort(v.begin(), v.end(), [](const FormatterInfo &lhs, const FormatterInfo &rhs) {
        return lhs.priority > rhs.priority;
    });
}

void BodyPartFormatterBaseFactoryPrivate::appendFormattersForType(const QString &mimeType, QVector<const Interface::BodyPartFormatter *> &formatters)
{
    const auto it = registry.constFind(mimeType);
    if (it == registry.constEnd()) {
        return;
    }
    for (const auto &f : it.value()) {
        formatters.push_back(f.formatter);
    }
}

BodyPartFormatterBaseFactory::BodyPartFormatterBaseFactory()
    : d(new BodyPartFormatterBaseFactoryPrivate(this))
{
}

BodyPartFormatterBaseFactory::~BodyPartFormatterBaseFactory()
{
    delete d;
}

void BodyPartFormatterBaseFactory::insert(const QString &mimeType, const Interface::BodyPartFormatter *formatter, int priority)
{
    d->insert(mimeType.toLower(), formatter, priority);
}

QVector<const Interface::BodyPartFormatter *> BodyPartFormatterBaseFactory::formattersForType(const QString &mimeType) const
{
    QVector<const Interface::BodyPartFormatter *> r;
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
        if (processedTypes[i] == QLatin1String("application/octet-stream")) { // we'll deal with that later
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
    if (mimeType.startsWith(QLatin1String("multipart/"))) {
        if (mimeType != QLatin1String("multipart/mixed")) {
            d->appendFormattersForType(QStringLiteral("multipart/mixed"), r);
        }
    } else {
        d->appendFormattersForType(QStringLiteral("application/octet-stream"), r);
    }
    assert(!r.empty());
    return r;
}

void BodyPartFormatterBaseFactory::loadPlugins()
{
    qCDebug(MIMETREEPARSER_LOG) << "plugin loading is not enabled in libmimetreeparser";
}
