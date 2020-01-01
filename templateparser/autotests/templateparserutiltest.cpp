/*
  Copyright (c) 2019-2020 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    QTest::newRow("newlinetabatbeginwithspace4") << QStringLiteral("    ddd        \n     \n\n\n\t\t  foo") << QStringLiteral("    ddd        \n     \n\n\n\t\t  foo");
}

void TemplateParserUtilTest::shouldRemoveSpaceAtBegin()
{
    QFETCH(const QString, selection);
    QFETCH(const QString, cleanedString);
    QCOMPARE(TemplateParser::Util::removeSpaceAtBegin(selection), cleanedString);
}
