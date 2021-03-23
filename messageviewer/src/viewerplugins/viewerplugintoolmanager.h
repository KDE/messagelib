/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include "viewerplugininterface.h"
#include <Item>
#include <QObject>
class KActionCollection;
class QAction;
namespace MessageViewer
{
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
    ~ViewerPluginToolManager() override;

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
