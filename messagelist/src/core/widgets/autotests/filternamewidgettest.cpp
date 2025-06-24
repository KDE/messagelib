/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamewidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "core/widgets/filternamewidget.h"
#include <KIconButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTest>
QTEST_MAIN(FilterNameWidgetTest)
FilterNameWidgetTest::FilterNameWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void FilterNameWidgetTest::shouldHaveDefaultValues()
{
    MessageList::Core::FilterNameWidget w;
    auto mainLayout = w.findChild<QHBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mName = w.findChild<QLineEdit *>(u"mName"_s);
    QVERIFY(mName);
    QVERIFY(mName->text().isEmpty());
    QVERIFY(mName->isClearButtonEnabled());

    auto mIconButton = w.findChild<KIconButton *>(u"mIconButton"_s);
    QVERIFY(mIconButton);
}

#include "moc_filternamewidgettest.cpp"
