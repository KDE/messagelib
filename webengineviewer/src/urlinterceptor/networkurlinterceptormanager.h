/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QList>
#include <QObject>
#include <memory>
class KActionCollection;
class QAction;
class QWebEngineView;
namespace WebEngineViewer
{
class NetworkPluginUrlInterceptorInterface;
class WebHitTestResult;
class NetworkUrlInterceptorManagerPrivate;
/*!
 * \class WebEngineViewer::NetworkUrlInterceptorManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkUrlInterceptorManager
 *
 * \brief The NetworkUrlInterceptorManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorManager : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a NetworkUrlInterceptorManager for the given web engine view. */
    explicit NetworkUrlInterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent = nullptr);
    /*! Destroys the NetworkUrlInterceptorManager object. */
    ~NetworkUrlInterceptorManager() override;

    /*! Returns the list of interceptor interfaces. */
    [[nodiscard]] QList<NetworkPluginUrlInterceptorInterface *> interfaceList() const;
    /*! Returns the list of interceptor actions for the given hit test result. */
    [[nodiscard]] QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

private:
    std::unique_ptr<NetworkUrlInterceptorManagerPrivate> const d;
};
}
