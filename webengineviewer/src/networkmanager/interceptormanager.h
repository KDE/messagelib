/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer/networkpluginurlinterceptor.h"
#include "webengineviewer_export.h"
class KActionCollection;
class QWebEngineView;
class QAction;
namespace WebEngineViewer
{
class WebHitTestResult;
class NetworkPluginUrlInterceptorInterface;
class NetworkAccessManagerWebEnginePrivate;
/**
 * @brief The InterceptorManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT InterceptorManager : public QObject
{
    Q_OBJECT
public:
    explicit InterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent = nullptr);
    ~InterceptorManager() override;
    void addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);
    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    void removeInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);

private:
    NetworkAccessManagerWebEnginePrivate *const d;
};
}
