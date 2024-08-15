/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWidget>

namespace WebEngineViewer
{
/**
 * @brief The NetworkPluginUrlInterceptorConfigureWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptorConfigureWidget(QWidget *parent = nullptr);
    ~NetworkPluginUrlInterceptorConfigureWidget() override;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;

    [[nodiscard]] virtual QString helpAnchor() const;

Q_SIGNALS:
    void configureChanged();
};
}
