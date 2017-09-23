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

#include "webenginepage.h"
#include "webhittest.h"
#include "webhittestresult.h"

#include <KLocalizedString>

#include <QEventLoop>
#include <QWebEngineDownloadItem>
#include <QPointer>
#include <QTimer>
#include <QFileDialog>
#include <QWebEngineProfile>
#include <QPrinter>
using namespace WebEngineViewer;

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(parent)
{
    init();
}

WebEnginePage::WebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
    init();
}

WebEnginePage::~WebEnginePage()
{
}

void WebEnginePage::init()
{
    connect(profile(), &QWebEngineProfile::downloadRequested, this, &WebEnginePage::saveHtml);
}

WebEngineViewer::WebHitTest *WebEnginePage::hitTestContent(const QPoint &pos)
{
    return new WebHitTest(this, pos);
}

void WebEnginePage::saveHtml(QWebEngineDownloadItem *download)
{
    const QString fileName = QFileDialog::getSaveFileName(view(), i18n("Save HTML Page"));
    if (!fileName.isEmpty()) {
        download->setSavePageFormat(QWebEngineDownloadItem::SingleHtmlSaveFormat);
        download->setPath(fileName);
        download->accept();
    }
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    if (isMainFrame && type == NavigationTypeLinkClicked) {
        Q_EMIT urlClicked(url);
        return false;
    }
    return true;
}

void WebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    Q_UNUSED(level);
    Q_UNUSED(sourceID);
    qDebug() << lineNumber << ":" << message;
    Q_EMIT showConsoleMessage(message);
}

bool WebEnginePage::execPrintPreviewPage(QPrinter *printer, int timeout)
{
    QPointer<QEventLoop> loop = new QEventLoop;
    bool result = false;
    QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

    print(printer, [loop, &result](bool res) {
        if (loop && loop->isRunning()) {
            result = res;
            loop->quit();
        }
    });

    loop->exec();
    delete loop;

    return result;
}
