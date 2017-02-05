/*
   Copyright (C) 2017 Laurent Montel <montel@kde.org>

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

#include "templateextracthtmlelementwebengineview.h"
#include "templatewebenginepage.h"

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
using namespace TemplateParser;

TemplateExtractHtmlElementWebEngineView::TemplateExtractHtmlElementWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{
    mPage = new TemplateWebEnginePage(this);
    setPage(mPage);
    connect(mPage, &TemplateWebEnginePage::loadFinished, this, &TemplateExtractHtmlElementWebEngineView::slotLoadFinished);
}

TemplateExtractHtmlElementWebEngineView::~TemplateExtractHtmlElementWebEngineView()
{

}

void TemplateExtractHtmlElementWebEngineView::clear()
{
    mBodyElement.clear();
    mHeaderElement.clear();
    mHtmlElement.clear();
}

void TemplateExtractHtmlElementWebEngineView::setHtmlContent(const QString &html)
{
    clear();
    setHtml(html);
}


QString extractHeaderBodyScript()
{
#if 0
    mBodyElement = page.currentFrame()->evaluateJavaScript(
                QStringLiteral("document.getElementsByTagName('body')[0].innerHTML")).toString();

    mHeaderElement = page.currentFrame()->evaluateJavaScript(
                QStringLiteral("document.getElementsByTagName('head')[0].innerHTML")).toString();
#endif
    //TODO
    return {};
}

void TemplateExtractHtmlElementWebEngineView::slotLoadFinished(bool success)
{
    if (success) {
#if QT_VERSION >= 0x050700
//        page->runJavaScript(extractHeaderBodyScript(),
//                            (QWebEngineScript::UserWorld + 2),
//                            invoke(this, &TemplateExtractHtmlElementWebEngineView::handleHtmlInfo));
#else
//        page->runJavaScript(extractHeaderBodyScript(), invoke(this, &TemplateExtractHtmlElementWebEngineView::handleHtmlInfo));
#endif
    } else {
        Q_EMIT loadContentDone(false);
    }
}

void TemplateExtractHtmlElementWebEngineView::handleHtmlInfo(const QVariant &result)
{

}

QString TemplateExtractHtmlElementWebEngineView::htmlElement() const
{
    return mHtmlElement;
}

QString TemplateExtractHtmlElementWebEngineView::headerElement() const
{
    return mHeaderElement;
}

QString TemplateExtractHtmlElementWebEngineView::bodyElement() const
{
    return mBodyElement;
}
