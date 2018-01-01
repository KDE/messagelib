/*
   Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

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

#include "templatewebengineviewtest.h"
#include "templatewebengineview.h"
#include <QTest>
#include <QSignalSpy>

TemplateWebEngineViewTest::TemplateWebEngineViewTest(QObject *parent)
    : QObject(parent)
{
}

TemplateWebEngineViewTest::~TemplateWebEngineViewTest()
{
}

void TemplateWebEngineViewTest::shouldHaveDefaultValue()
{
    TemplateParser::TemplateWebEngineView w;
    QVERIFY(w.plainText().isEmpty());
}

void TemplateWebEngineViewTest::shouldExtractHtml()
{
    TemplateParser::TemplateWebEngineView w;
    QVERIFY(w.plainText().isEmpty());
    QSignalSpy spy(&w, &TemplateParser::TemplateWebEngineView::loadContentDone);
    w.setHtmlContent(QStringLiteral("<html><head></head><body>HTML Text</body></html>"));
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    const bool result = spy.at(0).at(0).toBool();
    QVERIFY(result);
    QCOMPARE(w.plainText(), QStringLiteral("HTML Text"));
}

QTEST_MAIN(TemplateWebEngineViewTest)
