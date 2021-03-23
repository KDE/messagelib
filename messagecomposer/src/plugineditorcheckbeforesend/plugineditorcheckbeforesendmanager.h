/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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
    explicit PluginEditorCheckBeforeSendManager(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSendManager() override;

    static PluginEditorCheckBeforeSendManager *self();

    Q_REQUIRED_RESULT QVector<PluginEditorCheckBeforeSend *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);

private:
    PluginEditorCheckBeforeSendManagerPrivate *const d;
};
}
