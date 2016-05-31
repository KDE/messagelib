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

#include "webengineexporthtmlpagejob.h"
#include "webengineviewer/config-webengineviewer.h"
#include <QTemporaryFile>
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
    QTemporaryFile temporaryFile;
    temporaryFile.setAutoRemove(false);
    if (!temporaryFile.open()) {
        Q_EMIT failed();
        deleteLater();
        return;
    }
    QTextStream stream(&temporaryFile);
    stream << text;
    temporaryFile.close();
    //We need to remove this temporary file
    Q_EMIT success(temporaryFile.fileName());
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
