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

#include "templateparseremailaddressrequesterinterfacewidgettest.h"
#include "../src/templateparseremailaddressrequesterinterfacewidget.h"
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>
#include <QHBoxLayout>
#include <QTest>

TemplateParserEmailAddressRequesterInterfaceWidgetTest::TemplateParserEmailAddressRequesterInterfaceWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateParserEmailAddressRequesterInterfaceWidgetTest::shouldHaveDefaultValues()
{
    TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget w;
    QHBoxLayout *mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    TemplateParser::TemplateParserEmailAddressRequesterBase *mTemplateParserEmailBase
        = w.findChild<TemplateParser::TemplateParserEmailAddressRequesterBase *>(QStringLiteral("templateparseremailbase"));
    QVERIFY(mTemplateParserEmailBase);
    QVERIFY(mTemplateParserEmailBase->text().isEmpty());
}

void TemplateParserEmailAddressRequesterInterfaceWidgetTest::shouldAssignValues()
{
    TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget w;

    TemplateParser::TemplateParserEmailAddressRequesterBase *mTemplateParserEmailBase
        = w.findChild<TemplateParser::TemplateParserEmailAddressRequesterBase *>(QStringLiteral("templateparseremailbase"));
    const QString value{
        QStringLiteral("foo")
    };
    w.setText(value);
    QCOMPARE(w.text(), value);
    QCOMPARE(mTemplateParserEmailBase->text(), value);
    w.clear();
    QVERIFY(w.text().isEmpty());
    QVERIFY(mTemplateParserEmailBase->text().isEmpty());
}

QTEST_MAIN(TemplateParserEmailAddressRequesterInterfaceWidgetTest)
