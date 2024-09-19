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
    QVERIFY(command.searchLineInfo().isEmpty());

    MessageList::Core::SearchLineCommand::SearchLineInfo info;
    QVERIFY(!info.isValid());
    QVERIFY(info.argument.isEmpty());
    QCOMPARE(info.type, MessageList::Core::SearchLineCommand::SearchLineType::Unknown);
}

void SearchLineCommandTest::shouldParseInfo_data()
{
    QTest::addColumn<QString>("line");
    QTest::addColumn<QList<MessageList::Core::SearchLineCommand::SearchLineInfo>>("info");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<bool>("hasUnreadBanner");
}

void SearchLineCommandTest::shouldParseInfo()
{
    QFETCH(QString, line);
    QFETCH(QList<MessageList::Core::SearchLineCommand::SearchLineInfo>, infos);
    QFETCH(bool, valid);
    MessageList::Core::SearchLineCommand command;
    command.parseSearchLineCommand(line);
    QCOMPARE(command.searchLineInfo(), infos);
}
