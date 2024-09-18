/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandtest.h"
#include "core/widgets/searchlinecommand.h"

#include <QTest>
QTEST_MAIN(SearchLineCommandTest)
SearchLineCommandTest::SearchLineCommandTest(QObject *parent)
    : QObject{parent}
{
}

void SearchLineCommandTest::shouldHaveDefaultValues()
{
    MessageList::Core::SearchLineCommand command;
    // TODO
}
