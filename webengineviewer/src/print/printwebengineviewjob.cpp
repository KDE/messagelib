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

#include "printwebengineviewjob.h"
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


PrintWebEngineViewJob::PrintWebEngineViewJob(QObject *parent)
    : QObject(parent),
      mEngineView(Q_NULLPTR)
{

}

PrintWebEngineViewJob::~PrintWebEngineViewJob()
{

}

void PrintWebEngineViewJob::start()
{
    if (mEngineView) {
        if (!mTemporaryFile.open()) {
            Q_EMIT failed();
            deleteLater();
        } else {
            mTemporaryFile.setAutoRemove(false);
#if QT_VERSION >= 0x050700
            //Print to pdf
            //mEngineView->page()->printToPdf(invoke(this, &PrintWebEngineViewJob::slotHandlePdfPrinted), printer.pageLayout());
#endif
        }
    } else {
        Q_EMIT failed();
        deleteLater();
    }
}

void PrintWebEngineViewJob::slotHandlePdfPrinted(const QByteArray& result)
{
    //TODO
}

QWebEngineView *PrintWebEngineViewJob::engineView() const
{
    return mEngineView;
}

void PrintWebEngineViewJob::setEngineView(QWebEngineView *engineView)
{
    mEngineView = engineView;
}
