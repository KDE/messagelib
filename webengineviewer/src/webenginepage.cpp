/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webenginepage.h"
#include "webhittest.h"
#include "webhittestresult.h"

#include <QEventLoop>
#include <QWebEngineSettings>
#include <QPointer>
#include <QTimer>

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
}

WebEnginePage::~WebEnginePage()
{
    delete d;
}

WebEngineViewer::WebHitTest *WebEnginePage::hitTestContent(const QPoint &pos)
{
    return new WebHitTest(this, pos);
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

