/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QWidget>

namespace MessageComposer
{
/**
 * @brief The PluginEditorCheckBeforeSendConfigureWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendConfigureWidget(QWidget *parent = nullptr);
    ~PluginEditorCheckBeforeSendConfigureWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
