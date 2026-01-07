/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
namespace MessageComposer
{
class PluginEditorManagerPrivate;
class PluginEditor;
/**
 * @brief The PluginEditorManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorManager : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    ~PluginEditorManager() override;

    /*!
     */
    static PluginEditorManager *self();

    /*!
     */
    [[nodiscard]] QList<PluginEditor *> pluginsList() const;

    /*!
     */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;

    /*!
     */
    [[nodiscard]] QString configGroupName() const;
    /*!
     */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*!
     */
    [[nodiscard]] PluginEditor *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorManagerPrivate> const d;
};
}
