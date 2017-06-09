/*
  Copyright (c) 2016-2017 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webengineparthtmlwritertest.h"
#include "../webengineparthtmlwriter.h"

#include <QTest>

WebEnginePartHtmlWriterTest::WebEnginePartHtmlWriterTest(QObject *parent)
    : QObject(parent)
{
}

WebEnginePartHtmlWriterTest::~WebEnginePartHtmlWriterTest()
{
}

void WebEnginePartHtmlWriterTest::removeScriptInHtml_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    QTest::newRow("noscript") << QStringLiteral("<a>boo</a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("onescript") << QStringLiteral("<a>boo<script>alert(1)</script></a>")
                               << QStringLiteral("<a>boo</a>");
    QTest::newRow("onescriptwithattribute") << QStringLiteral(
        "<a>boo<script type=\"foo\">alert(1)</script></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("severalscriptwithattribute") << QStringLiteral(
        "<p>foo</p><script>a</script><a>boo<script type=\"foo\">alert(1)</script></a>")
                                                << QStringLiteral("<p>foo</p><a>boo</a>");
    QTest::newRow("scriptwithspace") << QStringLiteral(
        "<a>boo<script type=\"foo\" >alert(1)</script ></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("scriptwithremoveaccess") << QStringLiteral(
        "<a>boo<script src=\"http://foo\"/></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("empty") << QString() << QString();

    //MultiLine
    QTest::newRow("multiline") << QStringLiteral("<a>boo<script>\nalert(1)</script></a>")
                               << QStringLiteral("<a>boo</a>");
    QTest::newRow("multiline-scriptwithspace") << QStringLiteral(
        "<a>boo<script type=\"foo\" >\nalert(1)\n</script ></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("multiline-severalscriptwithattribute") << QStringLiteral(
        "<p>foo</p><script>\na\n</script><a>boo<script type=\"foo\">\nalert(1)</script></a>")
                                                          << QStringLiteral("<p>foo</p><a>boo</a>");
    QTest::newRow("multiline-scriptwithspace") << QStringLiteral(
        "<a>boo<script type=\"foo\" >\nalert(1)\nbla\nsl</script ></a>") << QStringLiteral(
        "<a>boo</a>");

    //Insensitive case
    QTest::newRow("onescript-insensitive")
        << QStringLiteral("<a>boo<SCRIPT>alert(1)</script></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("onescriptwithattribute-insensitive") << QStringLiteral(
        "<a>boo<SCRIPt type=\"foo\">alert(1)</SCRIPT></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("severalscriptwithattribute-insensitive") << QStringLiteral(
        "<p>foo</p><script>a</SCRIPT><a>boo<SCRIPT type=\"foo\">alert(1)</script></a>")
                                                            << QStringLiteral("<p>foo</p><a>boo</a>");
    QTest::newRow("scriptwithspace-insensitive") << QStringLiteral(
        "<a>boo<SCRIPT type=\"foo\" >alert(1)</SCRIPT ></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("scriptwithremoveaccess-insensitive") << QStringLiteral(
        "<a>boo<SCRIPT src=\"http://foo\"/></a>") << QStringLiteral("<a>boo</a>");

    //MultiLine insensitive
    QTest::newRow("multiline-insensitive")
        << QStringLiteral("<a>boo<sCript>\nalert(1)</Script></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("multiline-scriptwithspace-insensitive") << QStringLiteral(
        "<a>boo<SCRipT type=\"foo\" >\nalert(1)\n</script ></a>") << QStringLiteral("<a>boo</a>");
    QTest::newRow("multiline-severalscriptwithattribute-insensitive") << QStringLiteral(
        "<p>foo</p><SCRIPT>\na\n</script><a>boo<script type=\"foo\">\nalert(1)</script></a>")
                                                                      << QStringLiteral(
        "<p>foo</p><a>boo</a>");
    QTest::newRow("multiline-scriptwithspace-insensitive") << QStringLiteral(
        "<a>boo<SCRIPT type=\"foo\" >\nalert(1)\nbla\nsl</script ></a>") << QStringLiteral(
        "<a>boo</a>");
}

void WebEnginePartHtmlWriterTest::removeScriptInHtml()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    QCOMPARE(MessageViewer::WebEnginePartHtmlWriter::removeJscripts(input), output);
}

QTEST_MAIN(WebEnginePartHtmlWriterTest)
