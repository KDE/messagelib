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

#include "printwebengineviewjob.h"
#include "webengineviewer_debug.h"
#include <QWebEngineView>

using namespace WebEngineViewer;

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

PrintWebEngineViewJob::PrintWebEngineViewJob(QObject *parent)
    : QObject(parent),
      mEngineView(Q_NULLPTR)
{

}

PrintWebEngineViewJob::~PrintWebEngineViewJob()
{

}

void PrintWebEngineViewJob::unableToPrint()
{
    Q_EMIT failed();
    deleteLater();
}

void PrintWebEngineViewJob::start()
{
    if (mEngineView) {
        if (!mTemporaryFile.open()) {
            unableToPrint();
        } else {
            if (mPageLayout.isValid()) {
                mTemporaryFile.setAutoRemove(false);
#if QT_VERSION >= 0x050700
                mEngineView->page()->printToPdf(invoke(this, &PrintWebEngineViewJob::slotHandlePdfPrinted), mPageLayout);
#else
                unableToPrint();
#endif
            } else {
                unableToPrint();
            }
        }
    } else {
        unableToPrint();
    }
}

void PrintWebEngineViewJob::slotHandlePdfPrinted(const QByteArray &result)
{
    if (result.isEmpty()) {
        return;
    }

    QFile file(mTemporaryFile.fileName());
    if (!file.open(QFile::WriteOnly)) {
        qCDebug(WEBENGINEVIEWER_LOG) << "unable to open temporary file";
        unableToPrint();
        return;
    }

    file.write(result.data(), result.size());
    file.close();
    Q_EMIT success(mTemporaryFile.fileName());
    deleteLater();
}

QPageLayout PrintWebEngineViewJob::pageLayout() const
{
    return mPageLayout;
}

QWebEngineView *PrintWebEngineViewJob::engineView() const
{
    return mEngineView;
}

void PrintWebEngineViewJob::setEngineView(QWebEngineView *engineView)
{
    mEngineView = engineView;
}

void PrintWebEngineViewJob::setPageLayout(const QPageLayout &pageLayout)
{
    mPageLayout = pageLayout;
}
