/*
  SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templateparserutiltest.h"
using namespace Qt::Literals::StringLiterals;

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
    QTest::newRow("tabatbegin") << u"\t\t  foo"_s << u"\t\t  foo"_s;
    QTest::newRow("newlinetabatbegin") << u"\n\n\n\n\t\t  foo"_s << u"\n\t\t  foo"_s;
    QTest::newRow("newlinetabatbeginwithspace") << u"        \n\n\n\n\t\t  foo"_s << u"\n\t\t  foo"_s;
    QTest::newRow("newlinetabatbeginwithspace2") << u"        \n     \n\n\n\t\t  foo"_s << u"\n\t\t  foo"_s;
    QTest::newRow("newlinetabatbeginwithspace3") << u"ddd        \n     \n\n\n\t\t  foo"_s << u"ddd        \n     \n\n\n\t\t  foo"_s;
    QTest::newRow("newlinetabatbeginwithspace4") << u"    ddd        \n     \n\n\n\t\t  foo"_s << u"    ddd        \n     \n\n\n\t\t  foo"_s;
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
    QTest::newRow("test1") << u"foo.bar-baz@kde.org"_s << u"foo.bar-baz"_s;
    QTest::newRow("test2") << u"bla, bli <foo@kde.org>"_s << u"bli"_s;
    QTest::newRow("test3") << u"\"bla, bli\" <foo@kde.org>"_s << u"bli"_s;
    QTest::newRow("test4") << u"\"bla bli\" <foo@kde.org>"_s << u"bla"_s;
    QTest::newRow("test5") << u"bla bli"_s << u"bla"_s;
    QTest::newRow("test6") << u"bla, bli"_s << u"bli"_s;
    QTest::newRow("test7") << u"\"bla, bli-blo\" <foo@kde.org>"_s << u"bli-blo"_s;
    QTest::newRow("test8") << u"\"bla-blo bli\" <foo@kde.org>"_s << u"bla-blo"_s;
    QTest::newRow("test9") << u"bla-blo bli"_s << u"bla-blo"_s;
    QTest::newRow("test10") << u"bla, bli-blo"_s << u"bli-blo"_s;
    QTest::newRow("test11") << u"\"bla, bli blo\" <foo@kde.org>"_s << u"bli blo"_s;
    QTest::newRow("test12") << u"bla blo bli"_s << u"bla"_s;
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
    QTest::addColumn<QString>("lastName");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("test1") << u"foo@kde.org"_s << QString();
    QTest::newRow("test2") << u"bla, bli <foo@kde.org>"_s << u"bli"_s; // ',' without quote is not allowed
    QTest::newRow("test3") << u"\"bla, bli\" <foo@kde.org>"_s << u"bla"_s;
    QTest::newRow("test4") << u"\"bla bli\" <foo@kde.org>"_s << u"bli"_s;
    QTest::newRow("test5") << u"bla-blo, bli <foo@kde.org>"_s << u"bli"_s; // ',' without quote is not allowed
    QTest::newRow("test6") << u"\"bla-blo, bli\" <foo@kde.org>"_s << u"bla-blo"_s;
    QTest::newRow("test7") << u"\"bla bli-blo\" <foo@kde.org>"_s << u"bli-blo"_s;
}

void TemplateParserUtilTest::shouldGetLastNameFromEmail()
{
    QFETCH(const QString, email);
    QFETCH(const QString, lastName);
    QCOMPARE(TemplateParser::Util::getLastNameFromEmail(email), lastName);
}

#include "moc_templateparserutiltest.cpp"
