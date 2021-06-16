/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamewidgettest.h"
#include "core/widgets/filternamewidget.h"
#include <QTest>
QTEST_MAIN(FilterNameWidgetTest)
FilterNameWidgetTest::FilterNameWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void FilterNameWidgetTest::shouldHaveDefaultValues()
{
    MessageList::Core::FilterNameWidget w;
    // TODO
}
