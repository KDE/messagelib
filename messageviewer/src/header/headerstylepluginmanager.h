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

#ifndef HEADERSTYLEPLUGINMANAGER_H
#define HEADERSTYLEPLUGINMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageViewer {
class HeaderStylePlugin;
class HeaderStylePluginManagerPrivate;
class MESSAGEVIEWER_EXPORT HeaderStylePluginManager : public QObject
{
    Q_OBJECT
public:
    static HeaderStylePluginManager *self();

    explicit HeaderStylePluginManager(QObject *parent = nullptr);
    ~HeaderStylePluginManager();

    Q_REQUIRED_RESULT QVector<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QStringList pluginListName() const;

    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);
private:
    HeaderStylePluginManagerPrivate *const d;
};
}
#endif // HEADERSTYLEPLUGINMANAGER_H
