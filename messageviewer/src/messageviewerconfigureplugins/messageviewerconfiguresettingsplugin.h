/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
#include <memory>
namespace MessageViewer
{
class MessageViewerConfigureSettingsPluginPrivate;
/**
 * @brief The MessageViewerConfigureSettingsPlugin class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerConfigureSettingsPlugin : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerConfigureSettingsPlugin(QObject *parent = nullptr);
    ~MessageViewerConfigureSettingsPlugin() override;

    virtual void showConfigureDialog(QWidget *parent);

Q_SIGNALS:
    void configChanged();

private:
    std::unique_ptr<MessageViewerConfigureSettingsPluginPrivate> const d;
};
}
