/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparserextracthtmlinfotest.h"
#include "templateparserextracthtmlinfo.h"

#include <QTest>
#include <QSignalSpy>

TemplateParserExtractHtmlInfoTest::TemplateParserExtractHtmlInfoTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateParserExtractHtmlInfoTest::initTestCase()
{
    qRegisterMetaType<TemplateParserExtractHtmlInfoResult>();
}

void TemplateParserExtractHtmlInfoTest::shouldReturnNullResult()
{
    TemplateParser::TemplateParserExtractHtmlInfo *info = new TemplateParser::TemplateParserExtractHtmlInfo;
    QSignalSpy spy(info, &TemplateParser::TemplateParserExtractHtmlInfo::finished);
    info->start();
    QCOMPARE(spy.count(), 1);
    TemplateParserExtractHtmlInfoResult result = spy.at(0).at(0).value<TemplateParserExtractHtmlInfoResult>();
    QVERIFY(result.mBodyElement.isEmpty());
    QVERIFY(result.mHeaderElement.isEmpty());
    QVERIFY(result.mHtmlElement.isEmpty());
    QVERIFY(result.mPlainText.isEmpty());
    QVERIFY(result.mTemplate.isEmpty());
}

void TemplateParserExtractHtmlInfoTest::shouldReturnNullButWithTemplate()
{
    TemplateParser::TemplateParserExtractHtmlInfo *info = new TemplateParser::TemplateParserExtractHtmlInfo;
    const QString templateStr = QStringLiteral("foo");
    info->setTemplate(templateStr);
    QSignalSpy spy(info, &TemplateParser::TemplateParserExtractHtmlInfo::finished);
    info->start();
    QCOMPARE(spy.count(), 1);
    TemplateParserExtractHtmlInfoResult result = spy.at(0).at(0).value<TemplateParserExtractHtmlInfoResult>();
    QVERIFY(result.mBodyElement.isEmpty());
    QVERIFY(result.mHeaderElement.isEmpty());
    QVERIFY(result.mHtmlElement.isEmpty());
    QVERIFY(result.mPlainText.isEmpty());
    QCOMPARE(result.mTemplate, templateStr);
}

QTEST_MAIN(TemplateParserExtractHtmlInfoTest)
