/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateextracttextfrommailtest.h"
using namespace Qt::Literals::StringLiterals;

#include "templateextracttextfrommail.h"
#include <QSignalSpy>
#include <QTest>

TemplateExtractTextFromMailTest::TemplateExtractTextFromMailTest(QObject *parent)
    : QObject(parent)
{
}

TemplateExtractTextFromMailTest::~TemplateExtractTextFromMailTest() = default;

void TemplateExtractTextFromMailTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplateExtractTextFromMail w;
    QVERIFY(w.plainText().isEmpty());
}

void TemplateExtractTextFromMailTest::shouldExtractHtml()
{
    TemplateParser::TemplateExtractTextFromMail w;
    QVERIFY(w.plainText().isEmpty());
    QSignalSpy spy(&w, &TemplateParser::TemplateExtractTextFromMail::loadContentDone);
    w.setHtmlContent(u"<html><head></head><body>HTML Text</body></html>"_s);
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    const bool result = spy.at(0).at(0).toBool();
    QVERIFY(result);
    QCOMPARE(w.plainText(), u"HTML Text"_s);
}

QTEST_MAIN(TemplateExtractTextFromMailTest)

#include "moc_templateextracttextfrommailtest.cpp"
