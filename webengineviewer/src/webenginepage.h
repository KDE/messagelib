/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QWebEnginePage>
#include "webengineviewer_export.h"
#include <QVariant>
class QWebEngineProfile;
class QWebEngineDownloadItem;
namespace WebEngineViewer
{
class WebHitTest;
class WebEnginePagePrivate;
class WEBENGINEVIEWER_EXPORT WebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit WebEnginePage(QObject *parent = nullptr);
    explicit WebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);

    ~WebEnginePage();
    WebEngineViewer::WebHitTest *hitTestContent(const QPoint &pos);

    void saveHtml(QWebEngineDownloadItem *download);
    bool execPrintPreviewPage(QPrinter *printer, int timeout);

Q_SIGNALS:
    void urlClicked(const QUrl &url);
    void showConsoleMessage(const QString &message);

protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;

private:
    void init();
    WebEnginePagePrivate *const d;
};
}

#endif // WEBENGINEPAGE_H
