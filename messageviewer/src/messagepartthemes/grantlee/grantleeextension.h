/*
   SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_GRANTLEEEXTENSION_H
#define MESSAGEVIEWER_GRANTLEEEXTENSION_H

#include <grantlee/node.h>
#include <grantlee/taglibraryinterface.h>

#include <QObject>

#include <functional>

namespace MessageViewer {
typedef std::function<void (Grantlee::OutputStream *stream)> GrantleeCallback;

class CallbackTag : public Grantlee::Node
{
    Q_OBJECT
public:
    explicit CallbackTag(const QString &name, QObject *parent = nullptr);
    void render(Grantlee::OutputStream *stream, Grantlee::Context *context) const override;

private:
    QString m_name;
};

class CallbackTagFactory : public Grantlee::AbstractNodeFactory
{
    Q_OBJECT
public:
    Grantlee::Node *getNode(const QString &tagContent, Grantlee::Parser *parser) const override;
};

class GrantleeTagLibrary : public QObject, public Grantlee::TagLibraryInterface
{
    Q_OBJECT
    Q_INTERFACES(Grantlee::TagLibraryInterface)
    Q_PLUGIN_METADATA(IID "org.grantlee.TagLibraryInterface")
public:
    explicit GrantleeTagLibrary(QObject *parent = nullptr);
    Q_REQUIRED_RESULT QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories(const QString &name) override;
};
}

Q_DECLARE_METATYPE(MessageViewer::GrantleeCallback)

#endif // MESSAGEVIEWER_GRANTLEEEXTENSION_H
