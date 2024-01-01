/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QWidget>

namespace MessageViewer
{
/**
 * @brief The MessageViewerConfigureSettingsPluginWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerConfigureSettingsPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageViewerConfigureSettingsPluginWidget(QWidget *parent = nullptr);
    ~MessageViewerConfigureSettingsPluginWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    [[nodiscard]] virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
