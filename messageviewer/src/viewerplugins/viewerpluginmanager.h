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

#ifndef VIEWERPLUGINMANAGER_H
#define VIEWERPLUGINMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageViewer {
class ViewerPluginManagerPrivate;
class ViewerPlugin;
class MESSAGEVIEWER_EXPORT ViewerPluginManager : public QObject
{
    Q_OBJECT
public:

    explicit ViewerPluginManager(QObject *parent = nullptr);
    ~ViewerPluginManager();
    static ViewerPluginManager *self();

    Q_REQUIRED_RESULT QVector<MessageViewer::ViewerPlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;

    void setServiceTypeName(const QString &serviceName);
    Q_REQUIRED_RESULT QString serviceTypeName() const;

    void setPluginName(const QString &pluginName);
    Q_REQUIRED_RESULT QString pluginName() const;

    Q_REQUIRED_RESULT bool initializePluginList();

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT MessageViewer::ViewerPlugin *pluginFromIdentifier(const QString &id);
private:
    Q_DISABLE_COPY(ViewerPluginManager)
    ViewerPluginManagerPrivate *const d;
};
}

#endif // VIEWERPLUGINMANAGER_H
