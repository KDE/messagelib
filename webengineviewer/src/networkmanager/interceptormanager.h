/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer/networkpluginurlinterceptor.h"
#include "webengineviewer_export.h"
#include <memory>
class KActionCollection;
class QWebEngineView;
class QAction;
namespace WebEngineViewer
{
class WebHitTestResult;
class NetworkPluginUrlInterceptorInterface;
class NetworkAccessManagerWebEnginePrivate;
/*!
 * \class WebEngineViewer::InterceptorManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/InterceptorManager
 *
 * \brief The InterceptorManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT InterceptorManager : public QObject
{
    Q_OBJECT
public:
    /*! Constructs an InterceptorManager for the given web engine view. */
    explicit InterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent = nullptr);
    /*! Destroys the InterceptorManager object. */
    ~InterceptorManager() override;
    /*! Adds an interceptor. */
    void addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);
    /*! Returns the list of URL interceptor actions for the given hit test result. */
    [[nodiscard]] QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    /*! Removes an interceptor. */
    void removeInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);

private:
    std::unique_ptr<NetworkAccessManagerWebEnginePrivate> const d;
};
}
