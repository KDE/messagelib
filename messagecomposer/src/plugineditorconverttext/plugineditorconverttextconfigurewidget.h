/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QWidget>

namespace MessageComposer
{
/**
 * @brief The PluginEditorConvertTextConfigureWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginEditorConvertTextConfigureWidget(QWidget *parent = nullptr);
    ~PluginEditorConvertTextConfigureWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
