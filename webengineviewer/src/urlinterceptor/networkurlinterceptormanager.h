/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "networkpluginurlinterceptor.h"
#include "networkpluginurlinterceptorinterface.h"
#include "webengineviewer_export.h"
#include <QObject>
#include <QVector>
#include <WebEngineViewer/WebHitTestResult>
class KActionCollection;
class QWebEngineView;
namespace WebEngineViewer
{
class WebHitTestResult;
class NetworkUrlInterceptorManagerPrivate;
/**
 * @brief The NetworkUrlInterceptorManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUrlInterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent = nullptr);
    ~NetworkUrlInterceptorManager() override;

    Q_REQUIRED_RESULT QVector<NetworkPluginUrlInterceptorInterface *> interfaceList() const;
    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

private:
    NetworkUrlInterceptorManagerPrivate *const d;
};
}
