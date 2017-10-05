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
    QHash<QString, Grantlee::AbstractNodeFactory *> nodeFactories(const QString &name) override;
};
}

Q_DECLARE_METATYPE(MessageViewer::GrantleeCallback)

#endif // MESSAGEVIEWER_GRANTLEEEXTENSION_H
