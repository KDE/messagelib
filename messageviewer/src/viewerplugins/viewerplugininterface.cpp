/*
   Copyright (C) 2015-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "viewerplugininterface.h"
#include "utils/messageviewerutil_p.h"
#include <QAction>
using namespace MessageViewer;

class MessageViewer::ViewerPluginInterfacePrivate
{
public:
    ViewerPluginInterfacePrivate()
    {
    }
};

ViewerPluginInterface::ViewerPluginInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ViewerPluginInterfacePrivate)
{
}

ViewerPluginInterface::~ViewerPluginInterface()
{
    delete d;
}

void ViewerPluginInterface::execute()
{
    showWidget();
}

void ViewerPluginInterface::setText(const QString &text)
{
    Q_UNUSED(text);
    // Reimplement in subclass.
}

QList<QAction *> ViewerPluginInterface::actions() const
{
    // Reimplement in subclass
    return {};
}

void ViewerPluginInterface::setUrl(const QUrl &url)
{
    Q_UNUSED(url);
    // Reimplement in subclass
}

void ViewerPluginInterface::setMessage(const KMime::Message::Ptr &value)
{
    Q_UNUSED(value);
    // Reimplement in subclass
}

void ViewerPluginInterface::setMessageItem(const Akonadi::Item &item)
{
    Q_UNUSED(item);
    // Reimplement in subclass
}

void ViewerPluginInterface::setCurrentCollection(const Akonadi::Collection &col)
{
    Q_UNUSED(col);
    // Reimplement in subclass
}

void ViewerPluginInterface::closePlugin()
{
    // Reimplement in subclass
}

void ViewerPluginInterface::showWidget()
{
    // Reimplement in subclass
}

void ViewerPluginInterface::updateAction(const Akonadi::Item &item)
{
    Q_UNUSED(item);
    // Reimplement in subclass
}

void ViewerPluginInterface::refreshActionList(KActionCollection *ac)
{
    //TODO
    Q_UNUSED(ac);
}

void ViewerPluginInterface::addHelpTextAction(QAction *act, const QString &text)
{
    MessageViewer::Util::addHelpTextAction(act, text);
}

void ViewerPluginInterface::slotActivatePlugin()
{
    Q_EMIT activatePlugin(this);
}
