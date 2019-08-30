/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORGRAMMARMANAGER_H
#define PLUGINEDITORGRAMMARMANAGER_H

#include <QObject>
#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
namespace PimCommon {
class CustomToolsPlugin;
}
namespace MessageComposer {
class PluginEditorGrammarManagerPrivate;
/**
 * @brief The PluginEditorGrammarManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorGrammarManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorGrammarManager(QObject *parent = nullptr);
    ~PluginEditorGrammarManager();

    static PluginEditorGrammarManager *self();

    Q_REQUIRED_RESULT QVector<PimCommon::CustomToolsPlugin *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PimCommon::CustomToolsPlugin *pluginFromIdentifier(const QString &id);
private:
    PluginEditorGrammarManagerPrivate *const d;
};
}
#endif // PLUGINEDITORGRAMMARMANAGER_H
