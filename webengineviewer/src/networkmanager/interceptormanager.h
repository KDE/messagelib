/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef INTERCEPTORMANAGER_H
#define INTERCEPTORMANAGER_H

#include "webengineviewer_export.h"
#include "webengineviewer/networkpluginurlinterceptor.h"
class KActionCollection;
class QWebEngineView;
class QAction;
namespace WebEngineViewer {
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
    ~InterceptorManager();
    void addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);
    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    void removeInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor);
private:
    NetworkAccessManagerWebEnginePrivate *const d;
};
}
#endif // INTERCEPTORMANAGER_H
