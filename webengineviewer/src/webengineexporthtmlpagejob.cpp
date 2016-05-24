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

#include "webengineexporthtmlpagejob.h"
#include "config-webengineviewer.h"
#include <QWebEngineView>

using namespace WebEngineViewer;
template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result) {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

WebEngineExportHtmlPageJob::WebEngineExportHtmlPageJob(QObject *parent)
    : QObject(parent),
      mEngineView(Q_NULLPTR)
{

}

WebEngineExportHtmlPageJob::~WebEngineExportHtmlPageJob()
{

}

void WebEngineExportHtmlPageJob::start()
{
    if (!mEngineView) {
        Q_EMIT failed();
        deleteLater();
        return;
    }
    mEngineView->page()->toHtml(invoke(this, &WebEngineExportHtmlPageJob::slotSaveHtmlToPage));
}

void WebEngineExportHtmlPageJob::slotSaveHtmlToPage(const QString &text)
{
    //TODO
    deleteLater();
}

QWebEngineView *WebEngineExportHtmlPageJob::engineView() const
{
    return mEngineView;
}

void WebEngineExportHtmlPageJob::setEngineView(QWebEngineView *engineView)
{
    mEngineView = engineView;
}
