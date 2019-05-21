/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#include "templatewebengineview.h"
#include "templatewebenginepage.h"
#include "templateparser_debug.h"

using namespace TemplateParser;

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

TemplateWebEngineView::TemplateWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{
    mPage = new TemplateWebEnginePage(this);
    setPage(mPage);
    connect(mPage, &TemplateWebEnginePage::loadFinished, this, &TemplateWebEngineView::slotLoadFinished);
}

TemplateWebEngineView::~TemplateWebEngineView()
{
}

void TemplateWebEngineView::setHtmlContent(const QString &html)
{
    mExtractedPlainText.clear();
    setHtml(html);
}

void TemplateWebEngineView::slotLoadFinished(bool ok)
{
    if (ok) {
        mPage->toPlainText(invoke(this, &TemplateWebEngineView::setPlainText));
    } else {
        qCWarning(TEMPLATEPARSER_LOG) << "Loading page failed";
        Q_EMIT loadContentDone(false);
    }
}

void TemplateWebEngineView::setPlainText(const QString &plainText)
{
    mExtractedPlainText = plainText;
    Q_EMIT loadContentDone(true);
}

QString TemplateWebEngineView::plainText() const
{
    return mExtractedPlainText;
}
