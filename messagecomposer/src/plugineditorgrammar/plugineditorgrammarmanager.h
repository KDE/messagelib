/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
#include <memory>
namespace PimCommon
{
class CustomToolsPlugin;
}
namespace MessageComposer
{
class PluginEditorGrammarManagerPrivate;
/**
 * @brief The PluginEditorGrammarManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorGrammarManager : public QObject
{
    Q_OBJECT
public:
    ~PluginEditorGrammarManager() override;

    static PluginEditorGrammarManager *self();

    [[nodiscard]] QList<PimCommon::CustomToolsPlugin *> pluginsList() const;

    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    [[nodiscard]] PimCommon::CustomToolsPlugin *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorGrammarManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorGrammarManagerPrivate> const d;
};
}
