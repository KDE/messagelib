/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
namespace MessageComposer
{
class PluginEditorConvertTextManagerPrivate;
class PluginEditorConvertText;
/**
 * @brief The PluginEditorConvertTextManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorConvertTextManager(QObject *parent = nullptr);
    ~PluginEditorConvertTextManager() override;

    static PluginEditorConvertTextManager *self();

    Q_REQUIRED_RESULT QVector<PluginEditorConvertText *> pluginsList() const;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT PluginEditorConvertText *pluginFromIdentifier(const QString &id);

private:
    PluginEditorConvertTextManagerPrivate *const d;
};
}
