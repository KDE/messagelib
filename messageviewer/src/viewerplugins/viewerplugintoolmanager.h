/*
   Copyright (C) 2015-2020 Laurent Montel <montel@kde.org>

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

#ifndef VIEWERPLUGINTOOLMANAGER_H
#define VIEWERPLUGINTOOLMANAGER_H

#include <Item>
#include <QObject>
#include "messageviewer_export.h"
#include "viewerplugininterface.h"
class KActionCollection;
class QAction;
namespace MessageViewer {
class ViewerPluginToolManagerPrivate;
class ViewerPluginInterface;
/**
 * @brief The ViewerPluginToolManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ViewerPluginToolManager : public QObject
{
    Q_OBJECT
public:
    explicit ViewerPluginToolManager(QWidget *parentWidget, QObject *parent = nullptr);
    ~ViewerPluginToolManager();

    void closeAllTools();

    void createView();
    void setActionCollection(KActionCollection *ac);

    void setPluginName(const QString &pluginName);
    Q_REQUIRED_RESULT QString pluginName() const;
    Q_REQUIRED_RESULT bool initializePluginList();
    Q_REQUIRED_RESULT QList<QAction *> viewerPluginActionList(ViewerPluginInterface::SpecificFeatureTypes features) const;

    void updateActions(const Akonadi::Item &messageItem);

    /**
     * @brief refreshActionList Refresh the list of action menu.
     */
    void refreshActionList();

    void setPluginDirectory(const QString &directory);
    Q_REQUIRED_RESULT QString pluginDirectory() const;
Q_SIGNALS:
    void activatePlugin(MessageViewer::ViewerPluginInterface *);

private:
    ViewerPluginToolManagerPrivate *const d;
};
}
#endif // VIEWERPLUGINTOOLMANAGER_H
