/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QWidget>

namespace MessageViewer
{
/**
 * @brief The MessageViewerCheckBeforeDeletingPluginWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageViewerCheckBeforeDeletingPluginWidget(QWidget *parent = nullptr);
    ~MessageViewerCheckBeforeDeletingPluginWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    [[nodiscard]] virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
