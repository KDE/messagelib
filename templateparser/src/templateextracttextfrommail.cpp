/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateextracttextfrommail.h"
#include "templateparser_debug.h"
#include "templatewebenginepage.h"

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

TemplateExtractTextFromMail::TemplateExtractTextFromMail(QObject *parent)
    : QObject(parent)
    , mPage(new TemplateWebEnginePage(this))
{
    connect(mPage, &TemplateWebEnginePage::loadFinished, this, &TemplateExtractTextFromMail::slotLoadFinished);
}

TemplateExtractTextFromMail::~TemplateExtractTextFromMail() = default;

void TemplateExtractTextFromMail::setHtmlContent(const QString &html)
{
    mExtractedPlainText.clear();
    mPage->setHtml(html);
}

void TemplateExtractTextFromMail::slotLoadFinished(bool ok)
{
    if (ok) {
        mPage->toPlainText(invoke(this, &TemplateExtractTextFromMail::setPlainText));
    } else {
        qCWarning(TEMPLATEPARSER_LOG) << "Loading page failed";
        Q_EMIT loadContentDone(false);
    }
}

void TemplateExtractTextFromMail::setPlainText(const QString &plainText)
{
    mExtractedPlainText = plainText;
    Q_EMIT loadContentDone(true);
}

QString TemplateExtractTextFromMail::plainText() const
{
    return mExtractedPlainText;
}

#include "moc_templateextracttextfrommail.cpp"
