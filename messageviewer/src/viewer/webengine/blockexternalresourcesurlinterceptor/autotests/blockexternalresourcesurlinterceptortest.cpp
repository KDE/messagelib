/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blockexternalresourcesurlinterceptortest.h"
#include "viewer/webengine/blockexternalresourcesurlinterceptor/blockexternalresourcesurlinterceptor.h"
#include <qtwebenginewidgetsversion.h>
#include <QTest>
QTEST_MAIN(BlockExternalResourcesUrlInterceptorTest)
BlockExternalResourcesUrlInterceptorTest::BlockExternalResourcesUrlInterceptorTest(QObject *parent)
    : QObject(parent)
{
}

void BlockExternalResourcesUrlInterceptorTest::shouldIntercept()
{
    MessageViewer::BlockExternalResourcesUrlInterceptor interceptor;
    QVERIFY(!interceptor.interceptRequest(QStringLiteral("file"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QStringLiteral("data"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QStringLiteral("ftp"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeUnknown, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QStringLiteral("ftp"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypePing, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QStringLiteral("ftp"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeLink));
#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QVERIFY(interceptor.interceptRequest(QStringLiteral("ftp"), QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame, QWebEngineUrlRequestInfo::NavigationType::NavigationTypeRedirect));
#endif
}
