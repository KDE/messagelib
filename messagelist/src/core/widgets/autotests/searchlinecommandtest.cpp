/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandtest.h"
using namespace Qt::Literals::StringLiterals;

#include "core/widgets/searchlinecommand.h"

#include <QTest>
QTEST_MAIN(SearchLineCommandTest)
static void initLocale()
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
    QVERIFY(command.isEmpty());
    QVERIFY(!command.hasOnlyOneLiteralCommand());
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
        const QString str{u"subject:foo"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = u"foo"_s;
        lstInfo.append(info);
        const QString convertStr = u"Subject contains foo"_s;
        QTest::newRow("test1") << str << lstInfo << 1 << convertStr;
    }
    {
        const QString str{u"subject:foo from:bli"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"foo"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"bli"_s;
            lstInfo.append(info);
        }
        const QString convertStr{u"Subject contains foo AND From contains bli"_s};
        QTest::newRow("test2") << str << lstInfo << 2 << convertStr;
    }
    {
        const QString str{u"is:important"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::IsImportant;
        lstInfo.append(info);
        const QString convertStr{u"Mail is important"_s};
        QTest::newRow("test is important") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{u"has:attachment subject:bla"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"bla"_s;
            lstInfo.append(info);
        }
        const QString convertStr{u"Mail has attachment AND Subject contains bla"_s};
        QTest::newRow("test several1") << str << lstInfo << 2 << convertStr;
    }

    {
        const QString str{u" bli bli bli"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
        info.argument = u" bli bli bli"_s;
        lstInfo.append(info);
        const QString convertStr{u"Literal string  bli bli bli"_s};
        QTest::newRow("literal1") << str << lstInfo << 1 << convertStr;
    }
    {
        const QString str{u"subject:(goo bla)"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = u"goo bla"_s;
        lstInfo.append(info);
        const QString convertStr{u"Subject contains goo bla"_s};
        QTest::newRow("test parenthese1") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{u"subject:(goo bla) from:(dddd ddd)"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"goo bla"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"dddd ddd"_s;
            lstInfo.append(info);
        }
        const QString convertStr{u"Subject contains goo bla AND From contains dddd ddd"_s};
        QTest::newRow("test parenthese2") << str << lstInfo << 2 << convertStr;
    }
    {
        const QString str{u"subject:(goo (bla))"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
        info.argument = u"goo (bla)"_s;
        lstInfo.append(info);
        const QString convertStr{u"Subject contains goo (bla)"_s};
        QTest::newRow("test parenthese3") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{u"subject: bla"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
            info.argument = u"bla"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Literal string bla"_s};
        QTest::newRow("extra space") << str << lstInfo << 1 << convertStr;
    }
    {
        const QString str{u"size:1M"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Size;
            info.argument = u"1M"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Size is 1M"_s};
        QTest::newRow("size") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{u"subject:ddd ffff"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Subject contains ddd ffff"_s};
        QTest::newRow("subject with space") << str << lstInfo << 1 << convertStr;
    }

    {
        const QString str{u"subject:ddd ffff from:laurent <foo@kde.org> cc:test@kde.org"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"laurent <foo@kde.org>"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Cc;
            info.argument = u"test@kde.org"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Subject contains ddd ffff AND From contains laurent <foo@kde.org> AND CC contains test@kde.org"_s};
        QTest::newRow("multiple elements") << str << lstInfo << 3 << convertStr;
    }

    {
        const QString str{u"subject:ddd ffff from:laurent <foo@kde.org> cc:test@kde.org has:attachment literal"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"laurent <foo@kde.org>"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Cc;
            info.argument = u"test@kde.org"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
            info.argument = u"literal"_s;
            lstInfo.append(info);
        }

        const QString convertStr{
            QStringLiteral("Subject contains ddd ffff AND From contains laurent <foo@kde.org> AND CC contains test@kde.org AND Mail has attachment AND Literal "
                           "string literal")};
        QTest::newRow("multiple elements2") << str << lstInfo << 5 << convertStr;
    }

    {
        const QString str{u"has:attachment subject:ddd ffff has:attachment from:laurent <foo@kde.org> cc:test@kde.org"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"laurent <foo@kde.org>"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Cc;
            info.argument = u"test@kde.org"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Mail has attachment AND Subject contains ddd ffff AND From contains laurent <foo@kde.org> AND CC contains test@kde.org"_s};
        QTest::newRow("multiple elements duplicate") << str << lstInfo << 4 << convertStr;
    }

    {
        const QString str{u"has:attachment has:attachment subject:ddd ffff has:attachment from:laurent <foo@kde.org> cc:test@kde.org"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }

        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::From;
            info.argument = u"laurent <foo@kde.org>"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Cc;
            info.argument = u"test@kde.org"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Mail has attachment AND Subject contains ddd ffff AND From contains laurent <foo@kde.org> AND CC contains test@kde.org"_s};
        QTest::newRow("multiple elements duplicate2") << str << lstInfo << 4 << convertStr;
    }
    {
        const QString str{u"subject:ddd ffff has:attachment"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }

        const QString convertStr{u"Subject contains ddd ffff AND Mail has attachment"_s};
        QTest::newRow("subject with space2") << str << lstInfo << 2 << convertStr;
    }
    {
        const QString str{u"voiture subject:ddd ffff has:attachment"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
            info.argument = u"voiture"_s;
            lstInfo.append(info);
        }

        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }

        const QString convertStr{u"Literal string voiture AND Subject contains ddd ffff AND Mail has attachment"_s};
        QTest::newRow("literal2") << str << lstInfo << 3 << convertStr;
    }

    {
        const QString str{u"voiture subject:ddd ffff has:attachment category:testcategory"_s};
        QList<MessageList::Core::SearchLineCommand::SearchLineInfo> lstInfo;
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Literal;
            info.argument = u"voiture"_s;
            lstInfo.append(info);
        }

        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Subject;
            info.argument = u"ddd ffff"_s;
            lstInfo.append(info);
        }
        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::HasAttachment;
            lstInfo.append(info);
        }

        {
            MessageList::Core::SearchLineCommand::SearchLineInfo info;
            info.type = MessageList::Core::SearchLineCommand::SearchLineType::Category;
            info.argument = u"testcategory"_s;
            lstInfo.append(info);
        }

        const QString convertStr{u"Literal string voiture AND Subject contains ddd ffff AND Mail has attachment AND Mail has tag testcategory"_s};
        QTest::newRow("literal2") << str << lstInfo << 4 << convertStr;
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
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(u"has"_s));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(u"is"_s));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(u"foo"_s));

    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Bcc));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::To));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Cc));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Size));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Date));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::From));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::HasAttachment));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsHam));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsEncrypted));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsForwarded));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsQueued));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsRead));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsReplied));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsIgnored));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsImportant));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::IsWatched));
    QVERIFY(!MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::HasInvitation));

    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Larger));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Smaller));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::OlderThan));
    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::NewerThan));

    QVERIFY(MessageList::Core::SearchLineCommand::hasSubType(MessageList::Core::SearchLineCommand::Category));
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
        info.argument = u"ddd"_s;
        QVERIFY(info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Subject;
        QVERIFY(!info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Category;
        QVERIFY(!info.isValid());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Category;
        info.argument = u"ddd"_s;
        QVERIFY(info.isValid());
    }
}

void SearchLineCommandTest::mustBeUnique()
{
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Subject;
        QVERIFY(!info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::IsForwarded;
        QVERIFY(info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::HasAttachment;
        QVERIFY(info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Cc;
        QVERIFY(!info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::IsHam;
        QVERIFY(info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::IsImportant;
        QVERIFY(info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::IsIgnored;
        QVERIFY(info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Category;
        QVERIFY(!info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::Bcc;
        QVERIFY(!info.mustBeUnique());
    }
    {
        MessageList::Core::SearchLineCommand::SearchLineInfo info;
        info.type = MessageList::Core::SearchLineCommand::From;
        QVERIFY(!info.mustBeUnique());
    }
}
#include "moc_searchlinecommandtest.cpp"
