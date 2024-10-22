/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandtest.h"
#include "core/widgets/searchlinecommand.h"

#include <QTest>
QTEST_MAIN(SearchLineCommandTest)
void initLocale()
{
    qputenv("LANG", "en_US.utf-8");
}

Q_CONSTRUCTOR_FUNCTION(initLocale)

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
    QTest::addColumn<QString>("convertedToString");

    {
        QString str;
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        QTest::newRow("empty") << str << lstInfo << 0 << QString();
    }
    {
        const QString str{QStringLiteral("subject:foo")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = QStringLiteral("foo");
        lstInfo.append(info);
        const QString convertStr = QStringLiteral("Subject contains foo");
        QTest::newRow("test1") << str << lstInfo << 1 << convertStr;
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
        const QString convertStr{QStringLiteral("Subject contains foo AND From contains bli")};
        QTest::newRow("test2") << str << lstInfo << 2 << convertStr;
    }

    {
        const QString str{QStringLiteral("is:important")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::IsImportant;
        lstInfo.append(info);
        const QString convertStr{QStringLiteral("Mail is important")};
        QTest::newRow("test is important") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{QStringLiteral("has:attachment subject:bla")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = QStringLiteral("bla");
            lstInfo.append(info);
        }
        const QString convertStr{QStringLiteral("Mail has attachment AND Subject contains bla")};
        QTest::newRow("test several1") << str << lstInfo << 2 << convertStr;
    }

    {
        const QString str{QStringLiteral(" bli bli bli")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
        info.argument = QStringLiteral(" bli bli bli");
        lstInfo.append(info);
        const QString convertStr{QStringLiteral(" bli bli bli")};
        QTest::newRow("literal1") << str << lstInfo << 1 << convertStr;
    }
    {
        const QString str{QStringLiteral("subject:(goo bla)")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = QStringLiteral("goo bla");
        lstInfo.append(info);
        const QString convertStr{QStringLiteral("Subject contains goo bla")};
        QTest::newRow("test parenthese1") << str << lstInfo << 1 << convertStr;
    }
    {
        const QString str{QStringLiteral("subject:(goo bla) from:(dddd ddd)")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = QStringLiteral("goo bla");
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = QStringLiteral("dddd ddd");
            lstInfo.append(info);
        }
        const QString convertStr{QStringLiteral("Subject contains goo bla AND From contains dddd ddd")};
        QTest::newRow("test parenthese2") << str << lstInfo << 2 << convertStr;
    }

    {
        const QString str{QStringLiteral("subject:(goo (bla))")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = QStringLiteral("goo (bla)");
        lstInfo.append(info);
        const QString convertStr{QStringLiteral("Subject contains goo (bla)")};
        QTest::newRow("test parenthese3") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{QStringLiteral("subject: bla")};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
            info.argument = QStringLiteral("bla");
            lstInfo.append(info);
        }

        const QString convertStr{QStringLiteral("bla")};
        QTest::newRow("extra space") << str << lstInfo << 1 << convertStr;
    }
}

void SearchLineCommandTest::shouldParseInfo()
{
    QFETCH(QString, line);
    QFETCH(QList<MessageList::Core::SearchLineCommand::SearchLineInfo>, infos);
    QFETCH(int, numberElement);
    QFETCH(QString, convertedToString);
    MessageList::Core::SearchLineCommand command;
    command.parseSearchLineCommand(line);
    QCOMPARE(command.searchLineInfo().count(), numberElement);
    QCOMPARE(command.searchLineInfo(), infos);
    QCOMPARE(command.generateCommadLineStr(), convertedToString);
}

void SearchLineCommandTest::shouldHaveSubType()
{
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(QStringLiteral("has")));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(QStringLiteral("is")));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(QStringLiteral("foo")));

    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Bcc));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::To));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Cc));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Size));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Date));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::From));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::HasAttachment));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsHam));
}

void SearchLineCommandTest::shouldBeValid()
{
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        QVERIFY(!info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::HasAttachment;
        QVERIFY(info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Subject;
        QVERIFY(!info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Subject;
        info.argument = QStringLiteral("ddd");
        QVERIFY(info.isValid());
    }
}

#include "moc_searchlinecommandtest.cpp"
