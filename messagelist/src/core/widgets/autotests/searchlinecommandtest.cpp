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

void SearchLineCommandTest::initTestCase()
{
    qRegisterMetaType<QList<MessageList::Core::SearchLineCommand::SearchLineInfo>>();
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
    QTest::addColumn<QList<MessageList::Core::SearchLineCommand::SearchLineInfo>>("infos");
    QTest::addColumn<int>("numberElement");
    {
        QString str;
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        QTest::newRow("empty") << str << lstInfo << 0;
    }
    {
        const QString str{QStringLiteral("subject:foo")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = QStringLiteral("foo");
        lstInfo.append(info);
        QTest::newRow("test1") << str << lstInfo << 1;
    }
    {
        const QString str{QStringLiteral("subject:foo from:bli")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = QStringLiteral("foo");
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = QStringLiteral("bli");
            lstInfo.append(info);
        }
        QTest::newRow("test2") << str << lstInfo << 2;
    }
}

void SearchLineCommandTest::shouldParseInfo()
{
    QFETCH(QString, line);
    QFETCH(QList<MessageList::Core::SearchLineCommand::SearchLineInfo>, infos);
    QFETCH(int, numberElement);
    MessageList::Core::SearchLineCommand command;
    command.parseSearchLineCommand(line);
    QCOMPARE(command.searchLineInfo().count(), numberElement);
    QCOMPARE(command.searchLineInfo(), infos);
}
