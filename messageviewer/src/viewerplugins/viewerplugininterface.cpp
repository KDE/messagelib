/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "viewerplugininterface.h"
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
    : QObject(parent),
      d(new MessageViewer::ViewerPluginInterfacePrivate)
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

void ViewerPluginInterface::addHelpTextAction(QAction *act, const QString &text)
{
    act->setStatusTip(text);
    act->setToolTip(text);
    if (act->whatsThis().isEmpty()) {
        act->setWhatsThis(text);
    }
}

void ViewerPluginInterface::slotActivatePlugin()
{
    Q_EMIT activatePlugin(this);
}
