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

#include "viewerplugintoolmanager.h"
#include "viewerpluginmanager.h"
#include "viewerplugin.h"
#include "viewerplugininterface.h"

#include <QVector>

using namespace MessageViewer;

class MessageViewer::ViewerPluginToolManagerPrivate
{
public:
    ViewerPluginToolManagerPrivate(ViewerPluginToolManager *qq, QWidget *parentWidget)
        : mActionCollection(Q_NULLPTR),
          mParentWidget(parentWidget),
          q(qq)
    {

    }
    void setServiceTypeName(const QString &serviceName);
    QString serviceTypeName() const;
    void setPluginName(const QString &pluginName);
    QString pluginName() const;
    void createView();
    void closeAllTools();
    void setActionCollection(KActionCollection *ac);
    void updateActions(const Akonadi::Item &messageItem);
    QList<QAction *> actionList(ViewerPluginInterface::SpecificFeatureTypes features) const;
    QList<MessageViewer::ViewerPluginInterface *> mListInterface;
    KActionCollection *mActionCollection;
    QWidget *mParentWidget;
    ViewerPluginToolManager *q;
};

void ViewerPluginToolManagerPrivate::setServiceTypeName(const QString &serviceName)
{
    MessageViewer::ViewerPluginManager::self()->setServiceTypeName(serviceName);
}

QString ViewerPluginToolManagerPrivate::serviceTypeName() const
{
    return MessageViewer::ViewerPluginManager::self()->serviceTypeName();
}

void ViewerPluginToolManagerPrivate::setPluginName(const QString &pluginName)
{
    MessageViewer::ViewerPluginManager::self()->setPluginName(pluginName);
}

QString ViewerPluginToolManagerPrivate::pluginName() const
{
    return MessageViewer::ViewerPluginManager::self()->pluginName();
}

void ViewerPluginToolManagerPrivate::createView()
{
    QVector<MessageViewer::ViewerPlugin *> listPlugin = MessageViewer::ViewerPluginManager::self()->pluginsList();
    Q_FOREACH (MessageViewer::ViewerPlugin *plugin, listPlugin) {
        MessageViewer::ViewerPluginInterface *interface = plugin->createView(mParentWidget, mActionCollection);
        q->connect(interface, &MessageViewer::ViewerPluginInterface::activatePlugin, q, &ViewerPluginToolManager::activatePlugin);
        mListInterface.append(interface);
    }
}

void ViewerPluginToolManagerPrivate::closeAllTools()
{
    Q_FOREACH (MessageViewer::ViewerPluginInterface *interface, mListInterface) {
        interface->closePlugin();
    }
}

void ViewerPluginToolManagerPrivate::setActionCollection(KActionCollection *ac)
{
    mActionCollection = ac;
}

QList<QAction *> ViewerPluginToolManagerPrivate::actionList(ViewerPluginInterface::SpecificFeatureTypes features) const
{
    QList<QAction *> lstAction;
    Q_FOREACH (MessageViewer::ViewerPluginInterface *interface, mListInterface) {
        if (features & ViewerPluginInterface::All) {
            lstAction.append(interface->action());
        } else {
            if (interface->featureTypes() & features) {
                lstAction.append(interface->action());
            }
        }
    }
    return lstAction;
}

void ViewerPluginToolManagerPrivate::updateActions(const Akonadi::Item &messageItem)
{
    Q_FOREACH (MessageViewer::ViewerPluginInterface *interface, mListInterface) {
        interface->updateAction(messageItem);
    }
}

ViewerPluginToolManager::ViewerPluginToolManager(QWidget *parentWidget, QObject *parent)
    : QObject(parent),
      d(new MessageViewer::ViewerPluginToolManagerPrivate(this, parentWidget))
{
}

ViewerPluginToolManager::~ViewerPluginToolManager()
{
    delete d;
}

void ViewerPluginToolManager::closeAllTools()
{
    d->closeAllTools();
}

void ViewerPluginToolManager::createView()
{
    d->createView();
}

void ViewerPluginToolManager::setActionCollection(KActionCollection *ac)
{
    d->setActionCollection(ac);
}

void ViewerPluginToolManager::setServiceTypeName(const QString &serviceName)
{
    d->setServiceTypeName(serviceName);
}

QString ViewerPluginToolManager::serviceTypeName() const
{
    return d->serviceTypeName();
}

void ViewerPluginToolManager::setPluginName(const QString &pluginName)
{
    d->setPluginName(pluginName);
}

QString ViewerPluginToolManager::pluginName() const
{
    return d->pluginName();
}

QList<QAction *> ViewerPluginToolManager::viewerPluginActionList(ViewerPluginInterface::SpecificFeatureTypes features) const
{
    return d->actionList(features);
}

void ViewerPluginToolManager::updateActions(const Akonadi::Item &messageItem)
{
    d->updateActions(messageItem);
}
