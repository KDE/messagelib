/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HEADERSTYLEPLUGINMANAGER_H
#define HEADERSTYLEPLUGINMANAGER_H

#include <QObject>
#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
namespace MessageViewer {
class HeaderStylePlugin;
class HeaderStylePluginManagerPrivate;
/**
 * @brief The HeaderStylePluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
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
