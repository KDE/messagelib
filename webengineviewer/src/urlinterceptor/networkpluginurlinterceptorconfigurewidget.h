/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWidget>

namespace WebEngineViewer
{
/*!
 * \class WebEngineViewer::NetworkPluginUrlInterceptorConfigureWidget
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkPluginUrlInterceptorConfigureWidget
 *
 * \brief The NetworkPluginUrlInterceptorConfigureWidget class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    /*! Constructs a NetworkPluginUrlInterceptorConfigureWidget with the given parent. */
    explicit NetworkPluginUrlInterceptorConfigureWidget(QWidget *parent = nullptr);
    /*! Destroys the NetworkPluginUrlInterceptorConfigureWidget. */
    ~NetworkPluginUrlInterceptorConfigureWidget() override;

    /*! Loads the configuration settings. */
    virtual void loadSettings() = 0;
    /*! Saves the configuration settings. */
    virtual void saveSettings() = 0;
    /*! Resets the configuration settings to defaults. */
    virtual void resetSettings() = 0;

    /*! Returns the help anchor for this configuration widget. */
    [[nodiscard]] virtual QString helpAnchor() const;

Q_SIGNALS:
    /*! Emitted when the configuration has changed. */
    void configureChanged();
};
}
