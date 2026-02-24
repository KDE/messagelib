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
/*!
 \class MessageComposer::PluginEditorGrammarManager
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorGrammarManager
 \author Laurent Montel <montel@kde.org>

 The PluginEditorGrammarManager class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorGrammarManager : public QObject
{
    Q_OBJECT
public:
    /*! \brief Destroys the PluginEditorGrammarManager. */
    ~PluginEditorGrammarManager() override;

    /*! \brief Returns the singleton instance of the manager. */
    static PluginEditorGrammarManager *self();

    /*! \brief Returns the list of all available grammar checker plugins. */
    [[nodiscard]] QList<PimCommon::CustomToolsPlugin *> pluginsList() const;

    /*! \brief Returns the configuration group name for the grammar plugins. */
    [[nodiscard]] QString configGroupName() const;
    /*! \brief Returns the configuration prefix setting key. */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*! \brief Returns plugin configuration data for all plugins. */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*! \brief Returns a plugin by its identifier.
        \param id The unique plugin identifier.
        \return The plugin, or nullptr if not found.
    */
    [[nodiscard]] PimCommon::CustomToolsPlugin *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorGrammarManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorGrammarManagerPrivate> const d;
};
}
