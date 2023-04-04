/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "grantleeextension.h"

#include <KTextTemplate/Exception>
#include <KTextTemplate/Parser>

using namespace MessageViewer;

CallbackTag::CallbackTag(const QString &name, QObject *parent)
    : KTextTemplate::Node(parent)
    , m_name(name)
{
}

void CallbackTag::render(KTextTemplate::OutputStream *stream, KTextTemplate::Context *context) const
{
    auto cb = context->lookup(m_name).value<GrantleeCallback>();
    if (cb) {
        cb(stream);
    }
}

KTextTemplate::Node *CallbackTagFactory::getNode(const QString &tagContent, KTextTemplate::Parser *parser) const
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

QHash<QString, KTextTemplate::AbstractNodeFactory *> GrantleeTagLibrary::nodeFactories(const QString &name)
{
    Q_UNUSED(name)
    QHash<QString, KTextTemplate::AbstractNodeFactory *> nodeFactories;
    nodeFactories.insert(QStringLiteral("callback"), new CallbackTagFactory());
    return nodeFactories;
}
