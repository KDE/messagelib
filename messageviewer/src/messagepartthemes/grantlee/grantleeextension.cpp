/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "grantleeextension.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <grantlee/exception.h>
#include <grantlee/parser.h>
#else
#include <KTextTemplate/Exception>
#include <KTextTemplate/Parser>
#endif

using namespace MessageViewer;

CallbackTag::CallbackTag(const QString &name, QObject *parent)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    : Grantlee::Node(parent)
#else
    : KTextTemplate::Node(parent)
#endif
    , m_name(name)
{
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void CallbackTag::render(Grantlee::OutputStream *stream, Grantlee::Context *context) const
#else
void CallbackTag::render(KTextTemplate::OutputStream *stream, KTextTemplate::Context *context) const
#endif
{
    auto cb = context->lookup(m_name).value<GrantleeCallback>();
    if (cb) {
        cb(stream);
    }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Grantlee::Node *CallbackTagFactory::getNode(const QString &tagContent, Grantlee::Parser *parser) const
#else
KTextTemplate::Node *CallbackTagFactory::getNode(const QString &tagContent, KTextTemplate::Parser *parser) const
#endif
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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QHash<QString, Grantlee::AbstractNodeFactory *> GrantleeTagLibrary::nodeFactories(const QString &name)
#else
QHash<QString, KTextTemplate::AbstractNodeFactory *> GrantleeTagLibrary::nodeFactories(const QString &name)
#endif
{
    Q_UNUSED(name)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories;
#else
    QHash<QString, KTextTemplate::AbstractNodeFactory *> nodeFactories;
#endif
    nodeFactories.insert(QStringLiteral("callback"), new CallbackTagFactory());
    return nodeFactories;
}
