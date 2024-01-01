/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QWidget>

namespace MessageComposer
{
/**
 * @brief The PluginEditorConfigureBaseWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConfigureBaseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginEditorConfigureBaseWidget(QWidget *parent = nullptr);
    ~PluginEditorConfigureBaseWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
