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

#include "templateextracthtmlelementfrommailtest.h"
#include "templateextracthtmlelementfrommail.h"
#include <QTest>
#include <QSignalSpy>

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
