/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include <QObject>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <grantlee/node.h>
#include <grantlee/taglibraryinterface.h>
#else
#include <KTextTemplate/Node>
#include <KTextTemplate/TagLibraryInterface>
#endif

#include <functional>

namespace MessageViewer
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using GrantleeCallback = std::function<void(Grantlee::OutputStream *)>;
#else
using GrantleeCallback = std::function<void(KTextTemplate::OutputStream *)>;
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class CallbackTag : public Grantlee::Node
#else
class CallbackTag : public KTextTemplate::Node
#endif
{
    Q_OBJECT
public:
    explicit CallbackTag(const QString &name, QObject *parent = nullptr);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void render(Grantlee::OutputStream *stream, Grantlee::Context *context) const override;
#else
    void render(KTextTemplate::OutputStream *stream, KTextTemplate::Context *context) const override;
#endif

private:
    const QString m_name;
};

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class CallbackTagFactory : public Grantlee::AbstractNodeFactory
#else
class CallbackTagFactory : public KTextTemplate::AbstractNodeFactory
#endif
{
    Q_OBJECT
public:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::Node *getNode(const QString &tagContent, Grantlee::Parser *parser) const override;
#else
    KTextTemplate::Node *getNode(const QString &tagContent, KTextTemplate::Parser *parser) const override;
#endif
};
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class GrantleeTagLibrary : public QObject, public Grantlee::TagLibraryInterface
#else
class GrantleeTagLibrary : public QObject, public KTextTemplate::TagLibraryInterface
#endif
{
    Q_OBJECT
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_INTERFACES(Grantlee::TagLibraryInterface)
#else
    Q_INTERFACES(KTextTemplate::TagLibraryInterface)
#endif
    Q_PLUGIN_METADATA(IID "org.grantlee.TagLibraryInterface")
public:
    explicit GrantleeTagLibrary(QObject *parent = nullptr);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_REQUIRED_RESULT QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories(const QString &name) override;
#else
    Q_REQUIRED_RESULT QHash<QString, KTextTemplate::AbstractNodeFactory *> nodeFactories(const QString &name) override;
#endif
};
}

Q_DECLARE_METATYPE(MessageViewer::GrantleeCallback)
