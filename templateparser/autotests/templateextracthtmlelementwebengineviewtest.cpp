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

#include "templateextracthtmlelementwebengineviewtest.h"
#include "templateextracthtmlelementwebengineview.h"
#include <QTest>
#include <QSignalSpy>

TemplateExtractHtmlElementWebEngineViewTest::TemplateExtractHtmlElementWebEngineViewTest(QObject *parent)
    : QObject(parent)
{

}

void TemplateExtractHtmlElementWebEngineViewTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplateExtractHtmlElementWebEngineView w;
    QVERIFY(w.bodyElement().isEmpty());
    QVERIFY(w.headerElement().isEmpty());
    QVERIFY(w.htmlElement().isEmpty());
}

void TemplateExtractHtmlElementWebEngineViewTest::shouldExtractHtml()
{
    TemplateParser::TemplateExtractHtmlElementWebEngineView w;
    QVERIFY(w.htmlElement().isEmpty());
    QSignalSpy spy(&w, &TemplateParser::TemplateExtractHtmlElementWebEngineView::loadContentDone);
    const QString htmlStr = QStringLiteral("<html><head></head><body>HTML Text</body></html>");
    w.setHtmlContent(htmlStr);
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    const bool result = spy.at(0).at(0).toBool();
    QVERIFY(result);
    QCOMPARE(w.htmlElement(), htmlStr);
    QCOMPARE(w.headerElement(), QString());
    QCOMPARE(w.bodyElement(), QStringLiteral("HTML Text"));
}


QTEST_MAIN(TemplateExtractHtmlElementWebEngineViewTest)
