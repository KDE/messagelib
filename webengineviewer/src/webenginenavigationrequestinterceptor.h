/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINENAVIGATIONREQUESTINTERCEPTOR_H
#define WEBENGINENAVIGATIONREQUESTINTERCEPTOR_H

#include <QWebEnginePage>

namespace WebEngineViewer {
class WebEnginePage;
class WebEngineNavigationRequestInterceptor : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebEngineNavigationRequestInterceptor(QWebEnginePage *page);
    ~WebEngineNavigationRequestInterceptor() override;

protected:
    Q_REQUIRED_RESULT bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;

private:
    QWebEnginePage *const mTargetPage;
};
}

#endif // WEBENGINENAVIGATIONREQUESTINTERCEPTOR_H
