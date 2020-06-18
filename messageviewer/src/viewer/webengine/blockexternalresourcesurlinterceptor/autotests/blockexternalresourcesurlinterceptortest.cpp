/*
   Copyright (C) 2020 Laurent Montel <montel@kde.org>

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
