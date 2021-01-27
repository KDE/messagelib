/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H
#define BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H

#include "messageviewer_private_export.h"
#include <QUrl>
#include <QWebEngineUrlRequestInfo>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT BlockExternalResourcesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit BlockExternalResourcesUrlInterceptor(QObject *parent = nullptr);
    ~BlockExternalResourcesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    Q_REQUIRED_RESULT bool
    interceptRequest(const QUrl &url, QWebEngineUrlRequestInfo::ResourceType resourceType, QWebEngineUrlRequestInfo::NavigationType navigationType);
Q_SIGNALS:
    void formSubmittedForbidden();
};
}
#endif // BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H
