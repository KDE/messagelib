/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateextracthtmlelementfrommailtest.h"
#include "templateextracthtmlelementfrommail.h"
#include <QSignalSpy>
#include <QTest>

TemplateExtractHtmlElementFromMailTest::TemplateExtractHtmlElementFromMailTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateExtractHtmlElementFromMailTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplateExtractHtmlElementFromMail w;
    QVERIFY(w.bodyElement().isEmpty());
    QVERIFY(w.headerElement().isEmpty());
    QVERIFY(w.htmlElement().isEmpty());
}

void TemplateExtractHtmlElementFromMailTest::shouldExtractHtml_data()
{
    QTest::addColumn<QString>("html");
    QTest::addColumn<QString>("body");
    QTest::addColumn<QString>("header");
    QTest::newRow("test1") << QStringLiteral("<html><head></head><body>HTML Text</body></html>") << QStringLiteral("HTML Text") << QString();
    QTest::newRow("empty") << QString() << QString() << QString();
}

void TemplateExtractHtmlElementFromMailTest::shouldExtractHtml()
{
    QFETCH(QString, html);
    QFETCH(QString, body);
    QFETCH(QString, header);
    TemplateParser::TemplateExtractHtmlElementFromMail w;
    QVERIFY(w.htmlElement().isEmpty());
    QSignalSpy spy(&w, &TemplateParser::TemplateExtractHtmlElementFromMail::loadContentDone);
    w.setHtmlContent(html);
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    const bool result = spy.at(0).at(0).toBool();
    QVERIFY(result);
    QCOMPARE(w.htmlElement(), html);
    QCOMPARE(w.headerElement(), header);
    QCOMPARE(w.bodyElement(), body);
}

QTEST_MAIN(TemplateExtractHtmlElementFromMailTest)

#include "moc_templateextracthtmlelementfrommailtest.cpp"
