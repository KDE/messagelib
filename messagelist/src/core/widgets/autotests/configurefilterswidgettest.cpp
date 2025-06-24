/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "core/widgets/configurefilterswidget.h"
#include <QListWidget>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(ConfigureFiltersWidgetTest)
ConfigureFiltersWidgetTest::ConfigureFiltersWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void ConfigureFiltersWidgetTest::shouldHaveDefaultValues()
{
    MessageList::Core::ConfigureFiltersWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);

    auto mListFiltersWidget = w.findChild<QListWidget *>(u"mListFiltersWidget"_s);
    QVERIFY(mListFiltersWidget);
    QCOMPARE(mListFiltersWidget->count(), 0);
}

#include "moc_configurefilterswidgettest.cpp"
