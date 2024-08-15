/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
namespace MessageComposer
{
class PluginEditorCheckBeforeSendManagerPrivate;
class PluginEditorCheckBeforeSend;
/**
 * @brief The PluginEditorCheckBeforeSendManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendManager : public QObject
{
    Q_OBJECT
public:
    ~PluginEditorCheckBeforeSendManager() override;

    static PluginEditorCheckBeforeSendManager *self();

    [[nodiscard]] QList<PluginEditorCheckBeforeSend *> pluginsList() const;

    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    [[nodiscard]] PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorCheckBeforeSendManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorCheckBeforeSendManagerPrivate> const d;
};
}
