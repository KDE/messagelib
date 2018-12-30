/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#ifndef MESSAGEVIEWERCONFIGURESETTINGSPLUGINMANAGER_H
#define MESSAGEVIEWERCONFIGURESETTINGSPLUGINMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageViewer {
class MessageViewerConfigureSettingsPlugin;
class MessageViewerConfigureSettingsPluginManagerPrivate;
class MESSAGEVIEWER_EXPORT MessageViewerConfigureSettingsPluginManager : public QObject
{
    Q_OBJECT
public:
    static MessageViewerConfigureSettingsPluginManager *self();

    explicit MessageViewerConfigureSettingsPluginManager(QObject *parent = nullptr);
    ~MessageViewerConfigureSettingsPluginManager();

    Q_REQUIRED_RESULT QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> pluginsList() const;

    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
private:
    MessageViewerConfigureSettingsPluginManagerPrivate *const d;
};
}
#endif // MESSAGEVIEWERCONFIGURESETTINGSPLUGINMANAGER_H
