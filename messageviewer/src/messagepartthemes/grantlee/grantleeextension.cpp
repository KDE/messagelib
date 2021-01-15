/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    auto expr = tagContent.split(QLatin1Char(' '), Qt::SkipEmptyParts);
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
    Q_UNUSED(name)
    QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories;
    nodeFactories.insert(QStringLiteral("callback"), new CallbackTagFactory());
    return nodeFactories;
}
