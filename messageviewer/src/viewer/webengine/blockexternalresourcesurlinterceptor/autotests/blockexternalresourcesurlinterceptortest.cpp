/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blockexternalresourcesurlinterceptortest.h"
#include "viewer/webengine/blockexternalresourcesurlinterceptor/blockexternalresourcesurlinterceptor.h"
#include <QTest>
#include <QStandardPaths>
QTEST_MAIN(BlockExternalResourcesUrlInterceptorTest)
BlockExternalResourcesUrlInterceptorTest::BlockExternalResourcesUrlInterceptorTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void BlockExternalResourcesUrlInterceptorTest::shouldIntercept()
{
    MessageViewer::BlockExternalResourcesUrlInterceptor interceptor;
    QVERIFY(!interceptor.interceptRequest(QUrl(QStringLiteral("file://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QUrl(QStringLiteral("data://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QUrl(QStringLiteral("ftp://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeUnknown, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QUrl(QStringLiteral("ftp://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypePing, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QUrl(QStringLiteral("ftp://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeLink));
    QVERIFY(interceptor.interceptRequest(QUrl(QStringLiteral("ftp://")), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeRedirect));
}
