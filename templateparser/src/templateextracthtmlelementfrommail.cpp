/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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

#include "templateextracthtmlelementfrommail.h"
#include "templateparser_debug.h"
#include "templatewebenginepage.h"
#include <QWebEngineScript>

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

using namespace TemplateParser;

TemplateExtractHtmlElementFromMail::TemplateExtractHtmlElementFromMail(QObject *parent)
    : QObject(parent)
{
    mPage = new TemplateWebEnginePage(this);
    connect(mPage, &TemplateWebEnginePage::loadFinished, this, &TemplateExtractHtmlElementFromMail::slotLoadFinished);
}

TemplateExtractHtmlElementFromMail::~TemplateExtractHtmlElementFromMail()
{
}

void TemplateExtractHtmlElementFromMail::clear()
{
    mBodyElement.clear();
    mHeaderElement.clear();
    mHtmlElement.clear();
}

void TemplateExtractHtmlElementFromMail::setHtmlContent(const QString &html)
{
    clear();
    mHtmlElement = html;
    mPage->setHtml(html);
}

QString extractHeaderBodyScript()
{
    const QString source = QStringLiteral("(function() {"
                                          "var res = {"
                                          "    body: document.getElementsByTagName('body')[0].innerHTML,"
                                          "    header: document.getElementsByTagName('head')[0].innerHTML"
                                          "};"
                                          "return res;"
                                          "})()");
    return source;
}

void TemplateExtractHtmlElementFromMail::slotLoadFinished(bool success)
{
    if (success) {
        mPage->runJavaScript(extractHeaderBodyScript(),
                             (QWebEngineScript::UserWorld + 2),
                             invoke(this, &TemplateExtractHtmlElementFromMail::handleHtmlInfo));
    } else {
        Q_EMIT loadContentDone(false);
    }
}

void TemplateExtractHtmlElementFromMail::handleHtmlInfo(const QVariant &result)
{
    if (result.isValid()) {
        const QVariantMap map = result.toMap();
        mBodyElement = map.value(QStringLiteral("body")).toString();
        mHeaderElement = map.value(QStringLiteral("header")).toString();
        Q_EMIT loadContentDone(true);
    } else {
        qCWarning(TEMPLATEPARSER_LOG) << "Impossible to get value";
        Q_EMIT loadContentDone(false);
    }
}

QString TemplateExtractHtmlElementFromMail::htmlElement() const
{
    return mHtmlElement;
}

QString TemplateExtractHtmlElementFromMail::headerElement() const
{
    return mHeaderElement;
}

QString TemplateExtractHtmlElementFromMail::bodyElement() const
{
    return mBodyElement;
}
