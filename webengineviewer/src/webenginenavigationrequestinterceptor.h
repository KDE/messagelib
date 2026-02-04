/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWebEnginePage>

namespace WebEngineViewer
{
class WebEnginePage;
class WebEngineNavigationRequestInterceptor : public QWebEnginePage
{
    Q_OBJECT
public:
    /*! Constructs a WebEngineNavigationRequestInterceptor for the given page. */
    explicit WebEngineNavigationRequestInterceptor(QWebEnginePage *page);
    /*! Destroys the WebEngineNavigationRequestInterceptor object. */
    ~WebEngineNavigationRequestInterceptor() override;

protected:
    /*! Handles navigation requests. */
    [[nodiscard]] bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;

private:
    QWebEnginePage *const mTargetPage;
};
}
