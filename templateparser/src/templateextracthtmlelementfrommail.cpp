/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateextracthtmlelementfrommail.h"
using namespace Qt::Literals::StringLiterals;

#include "templateparser_debug.h"
#include "templatewebenginepage.h"
#include <QWebEngineScript>

template<typename Arg, typename R, typename C>
struct InvokeWrapperFunction {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
static InvokeWrapperFunction<Arg, R, C> invokeFunction(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapperFunction<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}

using namespace TemplateParser;

TemplateExtractHtmlElementFromMail::TemplateExtractHtmlElementFromMail(QObject *parent)
    : QObject(parent)
    , mPage(new TemplateWebEnginePage(this))
{
    connect(mPage, &TemplateWebEnginePage::loadFinished, this, &TemplateExtractHtmlElementFromMail::slotLoadFinished);
}

TemplateExtractHtmlElementFromMail::~TemplateExtractHtmlElementFromMail() = default;

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

static QString extractHeaderBodyScript()
{
    const QString source = QStringLiteral(
        "(function() {"
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
                             invokeFunction(this, &TemplateExtractHtmlElementFromMail::handleHtmlInfo));
    } else {
        Q_EMIT loadContentDone(false);
    }
}

void TemplateExtractHtmlElementFromMail::handleHtmlInfo(const QVariant &result)
{
    if (result.isValid()) {
        const QVariantMap map = result.toMap();
        mBodyElement = map.value(u"body"_s).toString();
        mHeaderElement = map.value(u"header"_s).toString();
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

#include "moc_templateextracthtmlelementfrommail.cpp"
