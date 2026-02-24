/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
#include <memory>
namespace MessageComposer
{
class PluginEditorInitManagerPrivate;
class PluginEditorInit;
/*!
 \class MessageComposer::PluginEditorInitManager
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorInitManager
 \author Laurent Montel <montel@kde.org>

 The PluginEditorInitManager class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInitManager : public QObject
{
    Q_OBJECT
public:
    /*! \brief Destroys the PluginEditorInitManager. */
    ~PluginEditorInitManager() override;

    /*! \brief Returns the singleton instance of the manager. */
    static PluginEditorInitManager *self();

    /*! \brief Returns the list of all available editor initialization plugins. */
    [[nodiscard]] QList<PluginEditorInit *> pluginsList() const;

    /*! \brief Returns the configuration group name for the editor init plugins. */
    [[nodiscard]] QString configGroupName() const;
    /*! \brief Returns the configuration prefix setting key. */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*! \brief Returns plugin configuration data for all plugins. */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*! \brief Returns a plugin by its identifier.
        \param id The unique plugin identifier.
        \return The plugin, or nullptr if not found.
    */
    [[nodiscard]] PluginEditorInit *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorInitManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorInitManagerPrivate> const d;
};
}
