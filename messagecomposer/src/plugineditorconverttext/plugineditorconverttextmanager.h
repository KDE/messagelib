/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
namespace MessageComposer
{
class PluginEditorConvertTextManagerPrivate;
class PluginEditorConvertText;
/*!
 \class MessageComposer::PluginEditorConvertTextManager
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorConvertTextManager
 \author Laurent Montel <montel@kde.org>

 The PluginEditorConvertTextManager class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextManager : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    ~PluginEditorConvertTextManager() override;

    /*!
     */
    static PluginEditorConvertTextManager *self();

    /*!
     */
    [[nodiscard]] QList<PluginEditorConvertText *> pluginsList() const;

    /*!
     */
    [[nodiscard]] QString configGroupName() const;
    /*!
     */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*!
     */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*!
     */
    [[nodiscard]] PluginEditorConvertText *pluginFromIdentifier(const QString &id);

private:
    explicit PluginEditorConvertTextManager(QObject *parent = nullptr);
    std::unique_ptr<PluginEditorConvertTextManagerPrivate> const d;
};
}
