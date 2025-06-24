/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterinterfacewidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "../src/templateparseremailaddressrequesterinterfacewidget.h"
#include <QHBoxLayout>
#include <QTest>
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>

TemplateParserEmailAddressRequesterInterfaceWidgetTest::TemplateParserEmailAddressRequesterInterfaceWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateParserEmailAddressRequesterInterfaceWidgetTest::shouldHaveDefaultValues()
{
    TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget w;
    auto mainLayout = w.findChild<QHBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mTemplateParserEmailBase = w.findChild<TemplateParser::TemplateParserEmailAddressRequesterBase *>(u"templateparseremailbase"_s);
    QVERIFY(mTemplateParserEmailBase);
    QVERIFY(mTemplateParserEmailBase->text().isEmpty());
}

void TemplateParserEmailAddressRequesterInterfaceWidgetTest::shouldAssignValues()
{
    TemplateParser::TemplateParserEmailAddressRequesterInterfaceWidget w;

    auto mTemplateParserEmailBase = w.findChild<TemplateParser::TemplateParserEmailAddressRequesterBase *>(u"templateparseremailbase"_s);
    const QString value{u"foo"_s};
    w.setText(value);
    QCOMPARE(w.text(), value);
    QCOMPARE(mTemplateParserEmailBase->text(), value);
    w.clear();
    QVERIFY(w.text().isEmpty());
    QVERIFY(mTemplateParserEmailBase->text().isEmpty());
}

QTEST_MAIN(TemplateParserEmailAddressRequesterInterfaceWidgetTest)

#include "moc_templateparseremailaddressrequesterinterfacewidgettest.cpp"
