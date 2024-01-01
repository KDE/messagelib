/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webenginenavigationrequestinterceptor.h"
#include "webenginepage.h"
using namespace WebEngineViewer;

WebEngineNavigationRequestInterceptor::WebEngineNavigationRequestInterceptor(QWebEnginePage *page)
    : QWebEnginePage(page)
    , mTargetPage(page)
{
}

WebEngineNavigationRequestInterceptor::~WebEngineNavigationRequestInterceptor() = default;

bool WebEngineNavigationRequestInterceptor::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    Q_UNUSED(isMainFrame)
    auto page = qobject_cast<WebEnginePage *>(mTargetPage);
    if (type == NavigationTypeLinkClicked && page) {
        Q_EMIT page->urlClicked(url);
        return false;
    }
    return false;
}

#include "moc_webenginenavigationrequestinterceptor.cpp"
