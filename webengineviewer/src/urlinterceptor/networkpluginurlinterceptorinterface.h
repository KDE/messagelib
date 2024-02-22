/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
/**
 * @brief The NetworkPluginUrlInterceptorInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorInterface : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptorInterface(QObject *parent = nullptr);
    ~NetworkPluginUrlInterceptorInterface() override;

    virtual void createActions(KActionCollection *ac);
    [[nodiscard]] virtual QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;

    [[nodiscard]] virtual bool interceptRequest(QWebEngineUrlRequestInfo &info) = 0;
};
}
