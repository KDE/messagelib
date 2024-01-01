/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "viewerplugininterface.h"
#include "utils/messageviewerutil_p.h"
#include <QAction>
using namespace MessageViewer;

class MessageViewer::ViewerPluginInterfacePrivate
{
public:
    ViewerPluginInterfacePrivate() = default;
};

ViewerPluginInterface::ViewerPluginInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ViewerPluginInterfacePrivate)
{
}

ViewerPluginInterface::~ViewerPluginInterface() = default;

void ViewerPluginInterface::execute()
{
    showWidget();
}

void ViewerPluginInterface::setText(const QString &text)
{
    Q_UNUSED(text)
    // Reimplement in subclass.
}

QList<QAction *> ViewerPluginInterface::actions() const
{
    // Reimplement in subclass
    return {};
}

void ViewerPluginInterface::setUrl(const QUrl &url)
{
    Q_UNUSED(url)
    // Reimplement in subclass
}

void ViewerPluginInterface::setMessage(const KMime::Message::Ptr &value)
{
    Q_UNUSED(value)
    // Reimplement in subclass
}

void ViewerPluginInterface::setMessageItem(const Akonadi::Item &item)
{
    Q_UNUSED(item)
    // Reimplement in subclass
}

void ViewerPluginInterface::setCurrentCollection(const Akonadi::Collection &col)
{
    Q_UNUSED(col)
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
    Q_UNUSED(item)
    // Reimplement in subclass
}

void ViewerPluginInterface::refreshActionList(KActionCollection *ac)
{
    // TODO
    Q_UNUSED(ac)
}

void ViewerPluginInterface::addHelpTextAction(QAction *act, const QString &text)
{
    MessageViewer::Util::addHelpTextAction(act, text);
}

void ViewerPluginInterface::slotActivatePlugin()
{
    Q_EMIT activatePlugin(this);
}

#include "moc_viewerplugininterface.cpp"
