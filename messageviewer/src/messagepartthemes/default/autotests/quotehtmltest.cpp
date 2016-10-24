/* Copyright 2009 Thomas McGuire <mcguire@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "quotehtmltest.h"
#include "../defaultrenderer.h"
#include "util.h"
#include "setupenv.h"

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/HtmlWriter>
#include <MessageViewer/CSSHelperBase>
#include <MimeTreeParser/MessagePart>

#include <QTest>

using namespace MessageViewer;

QTEST_GUILESS_MAIN(QuoteHtmlTest)

void QuoteHtmlTest::initTestCase()
{
    MessageViewer::Test::setupEnv();
}

void QuoteHtmlTest::testQuoteHtml_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("showExpandQuotesMark");
    QTest::addColumn<int>("quotelevel");
    //No Expand Quotes
    QTest::newRow("simpletext") << QStringLiteral("http") << QStringLiteral("<div class=\"noquote\"><div dir=\"ltr\">http</div></div>") << false << 1;
    QTest::newRow("simplequote") << QStringLiteral(">") << QStringLiteral("<blockquote><div class=\"quotelevel1\"><div dir=\"ltr\"><span class=\"quotemarks\">></span></div></div></blockquote>") << false << 1;
    QTest::newRow("doublequotewithtext") << QStringLiteral(">> sddf") << QStringLiteral("<blockquote><blockquote><div class=\"quotelevel2\"><div dir=\"ltr\"><span class=\"quotemarks\">>> </span><font color=\"#007000\">sddf</font></div></div></blockquote></blockquote>") << false << 1;
    QTest::newRow("doublequote") << QStringLiteral(">>") << QStringLiteral("<blockquote><blockquote><div class=\"quotelevel2\"><div dir=\"ltr\"><span class=\"quotemarks\">>></span></div></div></blockquote></blockquote>") << false << 1;
    QTest::newRow("simplespace") << QStringLiteral(" ") << QStringLiteral("<div class=\"noquote\"><div dir=\"ltr\">&nbsp;</div></div>") << false << 1;
    QTest::newRow("multispace") << QStringLiteral("            Bug ID: 358324") << QStringLiteral("<div class=\"noquote\"><div dir=\"ltr\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Bug ID: 358324</div></div>") << false << 1;

    //Show Expand Quotes

}

void QuoteHtmlTest::testQuoteHtml()
{
    QFETCH(QString, data);
    QFETCH(QString, result);
    QFETCH(bool, showExpandQuotesMark);
    QFETCH(int, quotelevel);
    Test::HtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    emptySource.setShowExpandQuotesMark(showExpandQuotesMark);
    emptySource.setLevelQuote(quotelevel);

    MimeTreeParser::ObjectTreeParser otp(&emptySource);
    MimeTreeParser::MessagePart::Ptr part(new MimeTreeParser::MessagePart(&otp, data));

    DefaultRenderer renderer(part, &testCSSHelper);
    QCOMPARE(renderer.html(), result);
}
