/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidgettest.h"
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
    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);

    auto mListFiltersWidget = w.findChild<QListWidget *>(QStringLiteral("mListFiltersWidget"));
    QVERIFY(mListFiltersWidget);
    QCOMPARE(mListFiltersWidget->count(), 0);
}
