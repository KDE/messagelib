/*
   Copyright (c) 2017 Volker Krause <vkrause@kde.org>

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

#include "grantleeextension.h"

#include <grantlee/exception.h>
#include <grantlee/parser.h>

using namespace MessageViewer;

CallbackTag::CallbackTag(const QString &name, QObject *parent)
    : Grantlee::Node(parent)
    , m_name(name)
{
}

void CallbackTag::render(Grantlee::OutputStream *stream, Grantlee::Context *context) const
{
    auto cb = context->lookup(m_name).value<GrantleeCallback>();
    if (cb) {
        cb(stream);
    }
}

Grantlee::Node *CallbackTagFactory::getNode(const QString &tagContent, Grantlee::Parser *parser) const
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    auto expr = tagContent.split(QLatin1Char(' '), QString::SkipEmptyParts);
#else
    auto expr = tagContent.split(QLatin1Char(' '), Qt::SkipEmptyParts);
#endif
    if (expr.size() != 2) {
        return nullptr;
    }

    return new CallbackTag(expr.at(1), parser);
}

GrantleeTagLibrary::GrantleeTagLibrary(QObject *parent)
    : QObject(parent)
{
}

QHash<QString, Grantlee::AbstractNodeFactory *> GrantleeTagLibrary::nodeFactories(const QString &name)
{
    Q_UNUSED(name);
    QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories;
    nodeFactories.insert(QStringLiteral("callback"), new CallbackTagFactory());
    return nodeFactories;
}
