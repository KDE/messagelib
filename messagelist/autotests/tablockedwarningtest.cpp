/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "tablockedwarningtest.h"
#include "core/widgets/tablockedwarning.h"
#include <QTest>
QTEST_MAIN(TabLockedWarningTest)
TabLockedWarningTest::TabLockedWarningTest(QObject *parent)
    : QObject(parent)
{
}

void TabLockedWarningTest::shouldHaveDefaultValues()
{
    MessageList::Core::TabLockedWarning w;
    QVERIFY(!w.isVisible());
    QVERIFY(w.wordWrap());
    QVERIFY(!w.isCloseButtonVisible());
    QCOMPARE(w.messageType(), KMessageWidget::Warning);
}
