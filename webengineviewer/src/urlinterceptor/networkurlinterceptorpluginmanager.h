/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#ifndef MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H
#define MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H

#include <QObject>
#include <QVector>
#include <PimCommon/PluginUtil>
#include "webengineviewer_export.h"
namespace WebEngineViewer {
class NetworkUrlInterceptorPluginManagerPrivate;
class NetworkPluginUrlInterceptor;
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorPluginManager : public QObject
{
    Q_OBJECT
public:
    static NetworkUrlInterceptorPluginManager *self();
    explicit NetworkUrlInterceptorPluginManager(QObject *parent = nullptr);
    ~NetworkUrlInterceptorPluginManager();

    Q_REQUIRED_RESULT QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT WebEngineViewer::NetworkPluginUrlInterceptor *pluginFromIdentifier(const QString &id);
private:
    NetworkUrlInterceptorPluginManagerPrivate *const d;
};
}

#endif // MAILNETWORKURLINTERCEPTORPLUGINMANAGER_H
