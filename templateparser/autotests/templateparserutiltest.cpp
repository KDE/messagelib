/*
  SPDX-FileCopyrightText: 2019-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templateparserutiltest.h"
#include "templatesutil.h"
#include "templatesutil_p.h"
#include <QTest>
QTEST_GUILESS_MAIN(TemplateParserUtilTest)
TemplateParserUtilTest::TemplateParserUtilTest(QObject *parent)
    : QObject(parent)
{
}

void TemplateParserUtilTest::shouldRemoveSpaceAtBegin_data()
{
    QTest::addColumn<QString>("selection");
    QTest::addColumn<QString>("cleanedString");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("tabatbegin") << QStringLiteral("\t\t  foo") << QStringLiteral("\t\t  foo");
    QTest::newRow("newlinetabatbegin") << QStringLiteral("\n\n\n\n\t\t  foo") << QStringLiteral("\n\t\t  foo");
    QTest::newRow("newlinetabatbeginwithspace") << QStringLiteral("        \n\n\n\n\t\t  foo") << QStringLiteral("\n\t\t  foo");
    QTest::newRow("newlinetabatbeginwithspace2") << QStringLiteral("        \n     \n\n\n\t\t  foo") << QStringLiteral("\n\t\t  foo");
    QTest::newRow("newlinetabatbeginwithspace3") << QStringLiteral("ddd        \n     \n\n\n\t\t  foo") << QStringLiteral("ddd        \n     \n\n\n\t\t  foo");
    QTest::newRow("newlinetabatbeginwithspace4") << QStringLiteral("    ddd        \n     \n\n\n\t\t  foo")
                                                 << QStringLiteral("    ddd        \n     \n\n\n\t\t  foo");
}

void TemplateParserUtilTest::shouldRemoveSpaceAtBegin()
{
    QFETCH(const QString, selection);
    QFETCH(const QString, cleanedString);
    QCOMPARE(TemplateParser::Util::removeSpaceAtBegin(selection), cleanedString);
}

void TemplateParserUtilTest::shouldHasNotEmptyKeywordsWithArgs()
{
    QVERIFY(!TemplateParser::Util::keywordsWithArgs().isEmpty());
}

void TemplateParserUtilTest::shouldHasNotEmptyKeywords()
{
    QVERIFY(!TemplateParser::Util::keywords().isEmpty());
}

void TemplateParserUtilTest::shouldGetFirstNameFromEmail_data()
{
    QTest::addColumn<QString>("email");
    QTest::addColumn<QString>("firstName");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("test1") << QStringLiteral("foo@kde.org") << QStringLiteral("foo");
    QTest::newRow("test2") << QStringLiteral("bla, bli <foo@kde.org>") << QStringLiteral("bli");
    QTest::newRow("test3") << QStringLiteral("\"bla, bli\" <foo@kde.org>") << QStringLiteral("bli");
    QTest::newRow("test4") << QStringLiteral("\"bla bli\" <foo@kde.org>") << QStringLiteral("bla");
    QTest::newRow("test5") << QStringLiteral("\'bla bli\' <foo@kde.org>") << QStringLiteral("bla");
    QTest::newRow("test6") << QStringLiteral("bla bli") << QStringLiteral("bla");
    QTest::newRow("test7") << QStringLiteral("bla, bli") << QStringLiteral("bli");
}

void TemplateParserUtilTest::shouldGetFirstNameFromEmail()
{
    QFETCH(const QString, email);
    QFETCH(const QString, firstName);
    QCOMPARE(TemplateParser::Util::getFirstNameFromEmail(email), firstName);
}

void TemplateParserUtilTest::shouldGetLastNameFromEmail_data()
{
    QTest::addColumn<QString>("email");
    QTest::addColumn<QString>("firstName");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("test1") << QStringLiteral("foo@kde.org") << QString();
    QTest::newRow("test2") << QStringLiteral("bla, bli <foo@kde.org>") << QStringLiteral("bla");
    QTest::newRow("test3") << QStringLiteral("\"bla, bli\" <foo@kde.org>") << QStringLiteral("bla");
    QTest::newRow("test4") << QStringLiteral("\"bla bli\" <foo@kde.org>") << QStringLiteral("bli");
    QTest::newRow("test5") << QStringLiteral("\'bla bli\' <foo@kde.org>") << QStringLiteral("bli");
    QTest::newRow("test6") << QStringLiteral("\'bla, bli\'") << QStringLiteral("bla");
}

void TemplateParserUtilTest::shouldGetLastNameFromEmail()
{
    QFETCH(const QString, email);
    QFETCH(const QString, firstName);
    QCOMPARE(TemplateParser::Util::getLastNameFromEmail(email), firstName);
}
