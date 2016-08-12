/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

using namespace WebEngineViewer;

class WebEngineViewer::WebEnginePagePrivate
{
public:
    WebEnginePagePrivate()
    {

    }
};

WebEnginePage::WebEnginePage(QObject *parent)
    : QWebEnginePage(parent),
      d(new WebEnginePagePrivate)
{
    init();
}

WebEnginePage::WebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent),
      d(new WebEnginePagePrivate)
{
    init();
}

WebEnginePage::~WebEnginePage()
{
    delete d;
}

void WebEnginePage::init()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    connect(profile(), &QWebEngineProfile::downloadRequested, this, &WebEnginePage::saveHtml);
#endif
}

WebEngineViewer::WebHitTest *WebEnginePage::hitTestContent(const QPoint &pos)
{
    return new WebHitTest(this, pos);
}

void WebEnginePage::saveHtml(QWebEngineDownloadItem *download)
{
#if QT_VERSION >= 0x050700
    const QString fileName = QFileDialog::getSaveFileName(view(), i18n("Save HTML Page"));
    if (!fileName.isEmpty()) {
        download->setSavePageFormat(QWebEngineDownloadItem::SingleHtmlSaveFormat);
        download->setPath(fileName);
        download->accept();
    }
#else
    Q_UNUSED(download);
#endif
}


QVariant WebEnginePage::execJavaScript(const QString &scriptSource, int timeout)
{
    QVariant result;
    QPointer<QEventLoop> loop = new QEventLoop;
    QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

    runJavaScript(scriptSource, [loop, &result](const QVariant & res) {
        if (loop && loop->isRunning()) {
            result = res;
            loop->quit();
        }
    });
    loop->exec();
    delete loop;
    return result;
}

bool WebEnginePage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    if (isMainFrame && type == NavigationTypeLinkClicked) {
        Q_EMIT urlClicked(url);
        return false;
    }
    return true;
}

