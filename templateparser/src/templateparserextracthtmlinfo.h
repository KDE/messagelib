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

#ifndef TEMPLATEPARSEREXTRACTHTMLINFO_H
#define TEMPLATEPARSEREXTRACTHTMLINFO_H

#include <QObject>
#include "templateparser_private_export.h"
#include "templateparserextracthtmlinforesult.h"

namespace TemplateParser {
class TemplateWebEngineView;
class TemplateExtractHtmlElementWebEngineView;

class TEMPLATEPARSER_TESTS_EXPORT TemplateParserExtractHtmlInfo : public QObject
{
    Q_OBJECT
public:
    explicit TemplateParserExtractHtmlInfo(QObject *parent = nullptr);
    ~TemplateParserExtractHtmlInfo();

    void setHtmlForExtractingTextPlain(const QString &html);
    void setHtmlForExtractionHeaderAndBody(const QString &html);
    void setTemplate(const QString &str);
    void start();

Q_SIGNALS:
    void finished(const TemplateParserExtractHtmlInfoResult &result);

private:
    void slotExtractHtmlElementFinished(bool success);
    void slotExtractToPlainTextFinished(bool success);

    TemplateParserExtractHtmlInfoResult mResult;

    QString mHtmlForExtractingTextPlain;
    QString mHtmlForExtractionHeaderAndBody;

    QString mTemplateStr;

    TemplateWebEngineView *mTemplateWebEngineView = nullptr;
    TemplateExtractHtmlElementWebEngineView *mExtractHtmlElementWebEngineView = nullptr;
};
}
#endif // TEMPLATEPARSEREXTRACTHTMLINFO_H
