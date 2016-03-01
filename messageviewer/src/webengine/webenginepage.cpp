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
#include "webhittestresult.h"

#include <QEventLoop>
#include <QPointer>
#include <QTimer>

using namespace MessageViewer;

class MessageViewer::WebEnginePagePrivate
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

MessageViewer::WebHitTestResult WebEnginePage::hitTestContent(const QPoint &pos)
{
    return WebHitTestResult(this, pos);
}

QVariant WebEnginePage::execJavaScript(const QString &scriptSource, int timeout)
{
    QPointer<QEventLoop> loop = new QEventLoop;
    QVariant result;
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
