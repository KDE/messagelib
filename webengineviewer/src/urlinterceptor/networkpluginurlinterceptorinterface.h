/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "webengineviewer_export.h"
#include <QObject>
class QWebEngineUrlRequestInfo;
class KActionCollection;
class QAction;
namespace WebEngineViewer
{
class WebHitTestResult;
/*!
 * \class WebEngineViewer::NetworkPluginUrlInterceptorInterface
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkPluginUrlInterceptorInterface
 *
 * \brief The NetworkPluginUrlInterceptorInterface class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorInterface : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a NetworkPluginUrlInterceptorInterface with the given parent. */
    explicit NetworkPluginUrlInterceptorInterface(QObject *parent = nullptr);
    /*! Destroys the NetworkPluginUrlInterceptorInterface object. */
    ~NetworkPluginUrlInterceptorInterface() override;

    /*! Creates actions for the interceptor using the given action collection. */
    virtual void createActions(KActionCollection *ac);
    /*! Returns the list of interceptor actions for the given hit test result. */
    [[nodiscard]] virtual QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    /*! Intercepts the URL request. */
    [[nodiscard]] virtual bool interceptRequest(QWebEngineUrlRequestInfo &info) = 0;
};
}
