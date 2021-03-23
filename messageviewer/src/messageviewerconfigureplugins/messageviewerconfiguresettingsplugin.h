/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "messageviewer_export.h"
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
    ~MessageViewerConfigureSettingsPlugin();

    virtual void showConfigureDialog(QWidget *parent);

Q_SIGNALS:
    void configChanged();

private:
    MessageViewerConfigureSettingsPluginPrivate *const d;
};
}
