/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

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

namespace MessageViewer
{
class ViewerPluginManagerPrivate;
class ViewerPlugin;
class MESSAGEVIEWER_EXPORT ViewerPluginManager : public QObject
{
    Q_OBJECT
public:
    class ViewerPluginData
    {
    public:
        ViewerPluginData()
            : mEnableByDefault(false)
        {

        }
        QString mDescription;
        QString mName;
        QString mIdentifier;
        bool mEnableByDefault;
    };

    explicit ViewerPluginManager(QObject *parent = Q_NULLPTR);
    ~ViewerPluginManager();
    static ViewerPluginManager *self();

    QVector<MessageViewer::ViewerPlugin *> pluginsList() const;
    QVector<MessageViewer::ViewerPluginManager::ViewerPluginData> pluginsDataList() const;

    void setServiceTypeName(const QString &serviceName);
    QString serviceTypeName() const;

    void setPluginName(const QString &pluginName);
    QString pluginName() const;

    bool initializePluginList();

    QString configGroupName() const;
    QString configPrefixSettingKey() const;
private:
    ViewerPluginManagerPrivate *const d;
};
}

#endif // VIEWERPLUGINMANAGER_H
