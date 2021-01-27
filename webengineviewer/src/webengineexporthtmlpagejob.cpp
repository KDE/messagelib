/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineexporthtmlpagejob.h"
#include <QTemporaryFile>
#include <QWebEngineView>

using namespace WebEngineViewer;
template<typename Arg, typename R, typename C> struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C> InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

WebEngineExportHtmlPageJob::WebEngineExportHtmlPageJob(QObject *parent)
    : QObject(parent)
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
    stream.setCodec("UTF-8");
    QString newText = text;
    newText.replace(QLatin1String("<head>"), QLatin1String("<head><meta charset=\"UTF-8\">"));
    stream << newText;
    temporaryFile.close();
    // We need to remove this temporary file
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
