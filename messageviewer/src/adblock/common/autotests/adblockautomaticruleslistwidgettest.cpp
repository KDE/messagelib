/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "adblockautomaticruleslistwidgettest.h"
#include "../adblockautomaticruleslistwidget.h"
#include <QTest>

AdBlockAutomaticRulesListWidgetTest::AdBlockAutomaticRulesListWidgetTest(QObject *parent)
    : QObject(parent)
{

}

AdBlockAutomaticRulesListWidgetTest::~AdBlockAutomaticRulesListWidgetTest()
{

}

void AdBlockAutomaticRulesListWidgetTest::shouldHaveDefaultValue()
{
    MessageViewer::AdBlockAutomaticRulesListWidget w;
    QCOMPARE(w.count(), 0);
}

void AdBlockAutomaticRulesListWidgetTest::shouldCreateRules_data()
{
    QTest::addColumn<QString>("rules");
    QTest::addColumn<int>("numberOfItem");
    QTest::newRow("notest") <<  QString() << 0;
}

void AdBlockAutomaticRulesListWidgetTest::shouldCreateRules()
{
    QFETCH(QString, rules);
    QFETCH(int, numberOfItem);

    MessageViewer::AdBlockAutomaticRulesListWidget w;
    w.setRules(rules);
    QCOMPARE(w.count(), numberOfItem);
}

QTEST_MAIN(AdBlockAutomaticRulesListWidgetTest)
