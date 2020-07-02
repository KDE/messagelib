/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
