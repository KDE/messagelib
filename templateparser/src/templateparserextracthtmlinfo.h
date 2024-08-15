/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "templateparser_private_export.h"
#include "templateparserextracthtmlinforesult.h"
#include <QObject>

namespace TemplateParser
{
class TemplateExtractTextFromMail;
class TemplateExtractHtmlElementFromMail;

class TEMPLATEPARSER_TESTS_EXPORT TemplateParserExtractHtmlInfo : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserExtractHtmlInfo(QObject *parent = nullptr);
    ~TemplateParserExtractHtmlInfo() override;

    void setHtmlForExtractingTextPlain(const QString &html);
    void setHtmlForExtractionHeaderAndBody(const QString &html);
    void setTemplate(const QString &str);
    void start();

Q_SIGNALS:
    void finished(const TemplateParserExtractHtmlInfoResult &result);

private:
    TEMPLATEPARSER_NO_EXPORT void slotExtractHtmlElementFinished(bool success);
    TEMPLATEPARSER_NO_EXPORT void slotExtractToPlainTextFinished(bool success);

    TemplateParserExtractHtmlInfoResult mResult;

    QString mHtmlForExtractingTextPlain;
    QString mHtmlForExtractionHeaderAndBody;

    QString mTemplateStr;

    TemplateExtractTextFromMail *mTemplateWebEngineView = nullptr;
    TemplateExtractHtmlElementFromMail *mExtractHtmlElementWebEngineView = nullptr;
};
}
