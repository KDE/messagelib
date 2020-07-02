/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILNETWORKURLINTERCEPTOR_H
#define MAILNETWORKURLINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include "webengineviewer_export.h"

namespace WebEngineViewer {
class NetworkUrlInterceptorPrivate;
class NetworkPluginUrlInterceptorInterface;
/**
 * @brief The NetworkUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    explicit NetworkUrlInterceptor(QObject *parent = nullptr);
    ~NetworkUrlInterceptor() override;

    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    void addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    void removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
private:
    NetworkUrlInterceptorPrivate *const d;
};
}
#endif // MAILNETWORKURLINTERCEPTOR_H
