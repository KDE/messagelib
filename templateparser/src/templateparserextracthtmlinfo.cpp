/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparserextracthtmlinfo.h"
#include "templateextracthtmlelementfrommail.h"
#include "templateextracttextfrommail.h"
#include "templateparser_debug.h"

using namespace TemplateParser;

TemplateParserExtractHtmlInfo::TemplateParserExtractHtmlInfo(QObject *parent)
    : QObject(parent)
{
}

TemplateParserExtractHtmlInfo::~TemplateParserExtractHtmlInfo()
{
    if (mTemplateWebEngineView) {
        mTemplateWebEngineView->deleteLater();
    }
    if (mExtractHtmlElementWebEngineView) {
        mExtractHtmlElementWebEngineView->deleteLater();
    }
}

void TemplateParserExtractHtmlInfo::setHtmlForExtractingTextPlain(const QString &html)
{
    mHtmlForExtractingTextPlain = html;
}

void TemplateParserExtractHtmlInfo::setHtmlForExtractionHeaderAndBody(const QString &html)
{
    mHtmlForExtractionHeaderAndBody = html;
}

void TemplateParserExtractHtmlInfo::setTemplate(const QString &str)
{
    mTemplateStr = str;
}

void TemplateParserExtractHtmlInfo::start()
{
    mResult.clear();
    mResult.mTemplate = mTemplateStr;
    if (!mHtmlForExtractingTextPlain.isEmpty()) {
        mTemplateWebEngineView = new TemplateExtractTextFromMail;
        connect(mTemplateWebEngineView, &TemplateExtractTextFromMail::loadContentDone, this, &TemplateParserExtractHtmlInfo::slotExtractToPlainTextFinished);
        mTemplateWebEngineView->setHtmlContent(mHtmlForExtractingTextPlain);
    } else {
        qCDebug(TEMPLATEPARSER_LOG) << "html string is empty for extracting to plainText";
        slotExtractToPlainTextFinished(false);
    }
}

void TemplateParserExtractHtmlInfo::slotExtractToPlainTextFinished(bool success)
{
    if (success) {
        mResult.mPlainText = mTemplateWebEngineView->plainText();
    } else {
        qCDebug(TEMPLATEPARSER_LOG) << "Impossible to extract plaintext";
    }
    if (!mHtmlForExtractionHeaderAndBody.isEmpty()) {
        mExtractHtmlElementWebEngineView = new TemplateExtractHtmlElementFromMail;
        connect(mExtractHtmlElementWebEngineView,
                &TemplateExtractHtmlElementFromMail::loadContentDone,
                this,
                &TemplateParserExtractHtmlInfo::slotExtractHtmlElementFinished);
        mExtractHtmlElementWebEngineView->setHtmlContent(mHtmlForExtractionHeaderAndBody);
    } else {
        qCDebug(TEMPLATEPARSER_LOG) << "html string is empty for extracting to header and body";
        slotExtractHtmlElementFinished(false);
    }
}

void TemplateParserExtractHtmlInfo::slotExtractHtmlElementFinished(bool success)
{
    if (success) {
        mResult.mBodyElement = mExtractHtmlElementWebEngineView->bodyElement();
        mResult.mHeaderElement = mExtractHtmlElementWebEngineView->headerElement();
        mResult.mHtmlElement = mExtractHtmlElementWebEngineView->htmlElement();
    } else {
        qCDebug(TEMPLATEPARSER_LOG) << "Impossible to extract html element";
    }
    Q_EMIT finished(mResult);
    deleteLater();
}

#include "moc_templateparserextracthtmlinfo.cpp"
