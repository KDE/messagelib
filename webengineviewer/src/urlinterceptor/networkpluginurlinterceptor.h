/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
class QWebEngineView;
namespace WebEngineViewer
{
class NetworkPluginUrlInterceptorInterface;
/*!
 * \class WebEngineViewer::NetworkPluginUrlInterceptor
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkPluginUrlInterceptor
 *
 * \brief The NetworkPluginUrlInterceptor class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptor : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a NetworkPluginUrlInterceptor with the given parent. */
    explicit NetworkPluginUrlInterceptor(QObject *parent = nullptr);
    /*! Destroys the NetworkPluginUrlInterceptor object. */
    ~NetworkPluginUrlInterceptor() override;
    /*! Creates the interceptor interface for the given web engine view. */
    virtual NetworkPluginUrlInterceptorInterface *createInterface(QWebEngineView *webEngine, QObject *parent = nullptr) = 0;

    /*! Returns whether this interceptor has a configuration dialog. */
    [[nodiscard]] virtual bool hasConfigureDialog() const;
    /*! Shows the configuration dialog with the given parent widget. */
    virtual void showConfigureDialog(QWidget *parent = nullptr);

    /*! Sets whether this interceptor is enabled. */
    void setIsEnabled(bool enabled);
    /*! Returns whether this interceptor is enabled. */
    [[nodiscard]] bool isEnabled() const;

private:
    bool mIsEnabled = false;
};
}
