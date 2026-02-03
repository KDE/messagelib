/*
   SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingPlugin;
class MessageViewerCheckBeforeDeletingPluginManagerPrivate;
/*!
 * \class MessageViewer::MessageViewerCheckBeforeDeletingPluginManager
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/MessageViewerCheckBeforeDeletingPluginManager
 * \brief The MessageViewerCheckBeforeDeletingPluginManager class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingPluginManager : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    static MessageViewerCheckBeforeDeletingPluginManager *self();
    /*!
     */
    ~MessageViewerCheckBeforeDeletingPluginManager() override;

    /*!
     */
    [[nodiscard]] QList<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> pluginsList() const;

    /*!
     */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*!
     */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*!
     */
    [[nodiscard]] QString configGroupName() const;
    /*!
     */
    [[nodiscard]] MessageViewerCheckBeforeDeletingPlugin *pluginFromIdentifier(const QString &id);

private:
    explicit MessageViewerCheckBeforeDeletingPluginManager(QObject *parent = nullptr);
    std::unique_ptr<MessageViewerCheckBeforeDeletingPluginManagerPrivate> const d;
};
}
