/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "viewerplugintoolmanager.h"
#include "viewerplugin.h"
#include "viewerpluginmanager.h"

#include <QList>

using namespace MessageViewer;

class MessageViewer::ViewerPluginToolManagerPrivate
{
public:
    ViewerPluginToolManagerPrivate(ViewerPluginToolManager *qq, QWidget *parentWidget)
        : mParentWidget(parentWidget)
        , q(qq)
    {
    }

    void setPluginName(const QString &pluginName);
    [[nodiscard]] QString pluginDirectory() const;
    [[nodiscard]] QString pluginName() const;
    void createView();
    void refreshActionList();
    void closeAllTools();
    void setActionCollection(KActionCollection *ac);
    void updateActions(const Akonadi::Item &messageItem);
    [[nodiscard]] QList<QAction *> actionList(ViewerPluginInterface::SpecificFeatureTypes features) const;
    QList<MessageViewer::ViewerPluginInterface *> mListInterface;
    KActionCollection *mActionCollection = nullptr;
    QWidget *mParentWidget = nullptr;
    ViewerPluginToolManager *const q;
    void setPluginDirectory(const QString &directory);
};

void ViewerPluginToolManagerPrivate::setPluginDirectory(const QString &directory)
{
    MessageViewer::ViewerPluginManager::self()->setPluginDirectory(directory);
}

QString ViewerPluginToolManagerPrivate::pluginDirectory() const
{
    return MessageViewer::ViewerPluginManager::self()->pluginDirectory();
}

void ViewerPluginToolManagerPrivate::setPluginName(const QString &pluginName)
{
    MessageViewer::ViewerPluginManager::self()->setPluginName(pluginName);
}

QString ViewerPluginToolManagerPrivate::pluginName() const
{
    return MessageViewer::ViewerPluginManager::self()->pluginName();
}

void ViewerPluginToolManagerPrivate::refreshActionList()
{
    for (MessageViewer::ViewerPluginInterface *interface : std::as_const(mListInterface)) {
        interface->refreshActionList(mActionCollection);
    }
}

void ViewerPluginToolManagerPrivate::createView()
{
    const QList<MessageViewer::ViewerPlugin *> listPlugin = MessageViewer::ViewerPluginManager::self()->pluginsList();
    for (MessageViewer::ViewerPlugin *plugin : listPlugin) {
        if (plugin->isEnabled()) {
            MessageViewer::ViewerPluginInterface *interface = plugin->createView(mParentWidget, mActionCollection);
            q->connect(interface, &MessageViewer::ViewerPluginInterface::activatePlugin, q, &ViewerPluginToolManager::activatePlugin);
            q->connect(plugin, &ViewerPlugin::configChanged, q, &ViewerPluginToolManager::refreshActionList);
            mListInterface.append(interface);
        }
    }
}

void ViewerPluginToolManagerPrivate::closeAllTools()
{
    for (MessageViewer::ViewerPluginInterface *interface : std::as_const(mListInterface)) {
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
    for (MessageViewer::ViewerPluginInterface *interface : std::as_const(mListInterface)) {
        if (features & ViewerPluginInterface::All) {
            lstAction.append(interface->actions());
        } else {
            if (interface->featureTypes() & features) {
                lstAction.append(interface->actions());
            }
        }
    }
    return lstAction;
}

void ViewerPluginToolManagerPrivate::updateActions(const Akonadi::Item &messageItem)
{
    for (MessageViewer::ViewerPluginInterface *interface : std::as_const(mListInterface)) {
        interface->updateAction(messageItem);
    }
}

ViewerPluginToolManager::ViewerPluginToolManager(QWidget *parentWidget, QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ViewerPluginToolManagerPrivate(this, parentWidget))
{
}

ViewerPluginToolManager::~ViewerPluginToolManager() = default;

void ViewerPluginToolManager::closeAllTools()
{
    d->closeAllTools();
}

void ViewerPluginToolManager::refreshActionList()
{
    d->refreshActionList();
}

void ViewerPluginToolManager::createView()
{
    d->createView();
}

void ViewerPluginToolManager::setActionCollection(KActionCollection *ac)
{
    d->setActionCollection(ac);
}

void ViewerPluginToolManager::setPluginDirectory(const QString &directory)
{
    d->setPluginDirectory(directory);
}

QString ViewerPluginToolManager::pluginDirectory() const
{
    return d->pluginDirectory();
}

void ViewerPluginToolManager::setPluginName(const QString &pluginName)
{
    d->setPluginName(pluginName);
}

QString ViewerPluginToolManager::pluginName() const
{
    return d->pluginName();
}

bool ViewerPluginToolManager::initializePluginList()
{
    return MessageViewer::ViewerPluginManager::self()->initializePluginList();
}

QList<QAction *> ViewerPluginToolManager::viewerPluginActionList(ViewerPluginInterface::SpecificFeatureTypes features) const
{
    return d->actionList(features);
}

void ViewerPluginToolManager::updateActions(const Akonadi::Item &messageItem)
{
    d->updateActions(messageItem);
}

#include "moc_viewerplugintoolmanager.cpp"
