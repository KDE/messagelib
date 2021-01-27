/*
  SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "templateparserutiltest.h"
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
