/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H
#define MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H

#include <QObject>
#include "webengineviewer_export.h"
class QWebEngineUrlRequestInfo;
class KActionCollection;
class QAction;
namespace WebEngineViewer {
class WebHitTestResult;
/**
 * @brief The NetworkPluginUrlInterceptorInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorInterface : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptorInterface(QObject *parent = nullptr);
    ~NetworkPluginUrlInterceptorInterface();

    virtual void createActions(KActionCollection *ac);
    virtual QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    virtual bool interceptRequest(QWebEngineUrlRequestInfo &info) = 0;
};
}
#endif // MAILNETWORKPLUGINURLINTERCEPTORINTERFACE_H
