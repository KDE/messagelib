/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H
#define BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H

#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
#include <QWebEngineUrlRequestInfo>
#include "messageviewer_private_export.h"

namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT BlockExternalResourcesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit BlockExternalResourcesUrlInterceptor(QObject *parent = nullptr);
    ~BlockExternalResourcesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    void setAllowExternalContent(bool b);
    Q_REQUIRED_RESULT bool allowExternalContent() const;
    Q_REQUIRED_RESULT bool interceptRequest(const QString &scheme, QWebEngineUrlRequestInfo::ResourceType resourceType, QWebEngineUrlRequestInfo::NavigationType navigationType);
Q_SIGNALS:
    void formSubmittedForbidden();
};
}
#endif // BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H
