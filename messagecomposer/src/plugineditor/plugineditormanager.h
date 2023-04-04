/*
   SPDX-FileCopyrightText: 2015-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
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
    explicit PluginEditorManager(QObject *parent = nullptr);
    ~PluginEditorManager() override;

    static PluginEditorManager *self();

    Q_REQUIRED_RESULT QList<PluginEditor *> pluginsList() const;

    Q_REQUIRED_RESULT QList<PimCommon::PluginUtilData> pluginsDataList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT PluginEditor *pluginFromIdentifier(const QString &id);

private:
    std::unique_ptr<PluginEditorManagerPrivate> const d;
};
}
