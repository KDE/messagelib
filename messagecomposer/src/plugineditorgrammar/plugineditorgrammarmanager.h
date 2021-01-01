/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
