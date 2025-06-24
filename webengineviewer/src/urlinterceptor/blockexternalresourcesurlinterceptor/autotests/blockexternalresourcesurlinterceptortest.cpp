/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blockexternalresourcesurlinterceptortest.h"
using namespace Qt::Literals::StringLiterals;

#include <QStandardPaths>
#include <QTest>
#include <WebEngineViewer/BlockExternalResourcesUrlInterceptor>
QTEST_GUILESS_MAIN(BlockExternalResourcesUrlInterceptorTest)
BlockExternalResourcesUrlInterceptorTest::BlockExternalResourcesUrlInterceptorTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void BlockExternalResourcesUrlInterceptorTest::shouldIntercept()
{
    WebEngineViewer::BlockExternalResourcesUrlInterceptor interceptor;
    QVERIFY(!interceptor.interceptRequest(QUrl(u"file://"_s),
                                          QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia,
                                          QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QUrl(u"data://"_s),
                                          QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMedia,
                                          QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QUrl(u"ftp://"_s),
                                         QWebEngineUrlRequestInfo::ResourceType::ResourceTypeUnknown,
                                         QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(interceptor.interceptRequest(QUrl(u"ftp://"_s),
                                         QWebEngineUrlRequestInfo::ResourceType::ResourceTypePing,
                                         QWebEngineUrlRequestInfo::NavigationType::NavigationTypeReload));
    QVERIFY(!interceptor.interceptRequest(QUrl(u"ftp://"_s),
                                          QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame,
                                          QWebEngineUrlRequestInfo::NavigationType::NavigationTypeLink));
    QVERIFY(interceptor.interceptRequest(QUrl(u"ftp://"_s),
                                         QWebEngineUrlRequestInfo::ResourceType::ResourceTypeMainFrame,
                                         QWebEngineUrlRequestInfo::NavigationType::NavigationTypeRedirect));
}

#include "moc_blockexternalresourcesurlinterceptortest.cpp"
