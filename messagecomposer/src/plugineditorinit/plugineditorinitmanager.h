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
    /*!
     */
    ~PluginEditorInitManager() override;

    /*!
     */
    static PluginEditorInitManager *self();

    /*!
     */
    [[nodiscard]] QList<PluginEditorInit *> pluginsList() const;

    /*!
     */
    [[nodiscard]] QString configGroupName() const;
    /*!
     */
    /*!
     */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*!
     */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*!
     */
    [[nodiscard]] PluginEditorInit *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorInitManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorInitManagerPrivate> const d;
};
}
