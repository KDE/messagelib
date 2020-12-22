/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VIEWERPLUGINMANAGER_H
#define VIEWERPLUGINMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageViewer {
class ViewerPluginManagerPrivate;
class ViewerPlugin;
/**
 * @brief The ViewerPluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ViewerPluginManager : public QObject
{
    Q_OBJECT
public:

    explicit ViewerPluginManager(QObject *parent = nullptr);
    ~ViewerPluginManager() override;
    static ViewerPluginManager *self();

    Q_REQUIRED_RESULT QVector<MessageViewer::ViewerPlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;

    Q_REQUIRED_RESULT bool initializePluginList();

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT MessageViewer::ViewerPlugin *pluginFromIdentifier(const QString &id);
    Q_REQUIRED_RESULT QString pluginDirectory() const;
    void setPluginDirectory(const QString &directory);
    void setPluginName(const QString &pluginName);
    Q_REQUIRED_RESULT QString pluginName() const;
private:
    Q_DISABLE_COPY(ViewerPluginManager)
    ViewerPluginManagerPrivate *const d;
};
}

#endif // VIEWERPLUGINMANAGER_H
