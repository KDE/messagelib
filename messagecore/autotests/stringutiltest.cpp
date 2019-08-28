/* Copyright 2009 Thomas McGuire <mcguire@kde.org>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2 of the License or
   ( at your option ) version 3 or, at the discretion of KDE e.V.
   ( which shall act as a proxy as in section 14 of the GPLv3 ), any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "stringutiltest.h"

#include "../src/utils/stringutil.h"

#include "qtest.h"
#include <QUrl>
using namespace MessageCore;

QTEST_MAIN(StringUtilTest)
#define lineLength 40

void StringUtilTest::test_SmartQuote()
{
    QFETCH(QString, originalString);
    QFETCH(QString, quotedString);

    QEXPECT_FAIL("hard linebreak", "Currently no way to differentiate between hard and soft line breaks", Continue);
    const QString result = StringUtil::smartQuote(originalString, lineLength);
    const QStringList resultList = result.split(QLatin1Char('\n'));
    const QStringList expectedList = quotedString.split(QLatin1Char('\n'));
    qDebug() << "result  :" << resultList;
    qDebug() << "expected:" << expectedList;
    QCOMPARE(resultList, expectedList);
    //QCOMPARE( result, quotedString );
}

void StringUtilTest::test_SmartQuote_data()
{
    QTest::addColumn<QString>("originalString");
    QTest::addColumn<QString>("quotedString");

    QTest::newRow("") << ""
                      << "";

    QTest::newRow("1") << "Some short text"
                       << "Some short text";

    //                                                               40
    //                                                               ↓
    QTest::newRow("2") << "Some much longer text that exceeds our limit by far."
                       << "Some much longer text that exceeds our\nlimit by far.";

    QTest::newRow("3") << " Space at start."
                       << " Space at start.";

    //                                                               40
    //                                                               ↓
    QTest::newRow("4") << " Space at start, but also two lines in this long sentence."
                       << " Space at start, but also two lines in\nthis long sentence.";

    QTest::newRow("5") << " Space at start and end. "
                       << " Space at start and end.";

    QTest::newRow("6") << "Space at end of pre-broken line. \n"
                          "Yet another line of text."
                       << "Space at end of pre-broken line.\n"
        "Yet another line of text.";

    //                                                               40
    //                                                               ↓
    QTest::newRow("7") << "Long long line, followed by another line starting with a space.\n"
                          " Starts with a space."
                       << "Long long line, followed by another line\n"
        "starting with a space. Starts with a\n"
        "space.";

    //                                                               40
    //                                                               ↓
    QTest::newRow("8") << "Two lines that don't need to be\nchanged in any way."
                       << "Two lines that don't need to be\nchanged in any way.";

    //                                                               40
    //                                                               ↓
    QTest::newRow("9") << "Many lines.\n"
                          "Only one needs to be broken.\n"
                          "This is the very much too long line that needs to be broken.\n"
                          "This line is ok again."
                       << "Many lines.\n"
        "Only one needs to be broken.\n"
        "This is the very much too long line that\n"
        "needs to be broken. This line is ok\n"
        "again.";

    //                                                                40
    //                                                                ↓
    QTest::newRow("10") << "> >Very long quoted line, that is very very long"
                        << "> >Very long quoted line, that is very\n"
        "> >very long";

    //                                                                40
    //                                                                ↓
    QTest::newRow("11") << "> > Very long quoted line, that is very very long"
                        << "> > Very long quoted line, that is very\n"
        "> > very long";

    //                                                                40
    //                                                                ↓
    QTest::newRow("12") << "> > Very long quoted line, that is very very long. \n"
                           "> > Another line here."
                        << "> > Very long quoted line, that is very\n"
        "> > very long. Another line here.";

    //                                                                40
    //                                                                ↓
    QTest::newRow("13") << "> > Very long quoted line, that is very very long. \n"
                           "Unquoted line, for a change.\n"
                           "> > Another line here."
                        << "> > Very long quoted line, that is very\n"
        "> > very long.\n"
        "Unquoted line, for a change.\n"
        "\n"
        "> > Another line here.";

    //                                                                40
    //                                                                ↓
    QTest::newRow("14") << "> Quote level 1 with long long long long text, that is long.\n"
                           "> Quote level 1 still.\n"
                           "> > Quote level 2 now, also with a long long long long text.\n"
                           "> > Quote level 2 still.\n"
                           "No quotes."
                        << "> Quote level 1 with long long long long\n"
        "> text, that is long. Quote level 1\n"
        "> still.\n"
        "> \n"
        "> > Quote level 2 now, also with a long\n"
        "> > long long long text. Quote level 2\n"
        "> > still.\n"
        "\n"
        "No quotes.";

    //                                                                40
    //                                                                ↓
    QTest::newRow("15") << "Some much longer text that exceeds our limit by far.\n"
                           "\n"
                           "Line after an empty one."
                        << "Some much longer text that exceeds our\n"
        "limit by far.\n"
        "\n"
        "Line after an empty one.";

    // Make sure the "You wrote:" line is not broken, that would look strange
    //                                                                40
    //                                                                ↓
    QTest::newRow("16") << "Yesterday, Hans Martin Ulrich Franz August wrote:\n"
                           "> Bla Bla Bla Bla..."
                        << "Yesterday, Hans Martin Ulrich Franz August wrote:\n"
        "> Bla Bla Bla Bla...";

    //                                                                40
    //                                                                ↓
    QTest::newRow("17") << "Yesterday, Hans Martin Ulrich Franz August wrote:\n"
                           "\n"
                           "> Bla Bla Bla Bla..."
                        << "Yesterday, Hans Martin Ulrich Franz August wrote:\n"
        "> Bla Bla Bla Bla...";

    // This test shows a fundamental flaw when breaking lines: The table header gets broken,
    // which is ok. However, the following line is appended to the table header, which leads
    // the data columns and the table header to by not aligned anymore.
    // The problem is that the linebreak after the table header is supposed to be a hard line break,
    // but there is no way to know that, so we treat it as a soft line break instead.
    //
    // We can't treat every linebreak as hard linebreaks, as quoting natural text would look strange then.
    // The problem here is that the sender word-wraps the email with hard linebreaks.
    // This is only possible to fix correctly by supporting "flowed" text, as per RFC 2646
    //
    // (this is marked as QEXPECT_FAIL above)
    //
    // The solution would be to let the caller remove soft linebreaks manually (as only the caller
    // can now), and let smartQuote() treat all linebreaks as hard linebreaks, which would fix this.
    //                                                                           40
    //                                                                            ↓
    QTest::newRow("hard linebreak") << "==== Date ======== Amount ======= Type ======\n"
                                       "   12.12.09          5            Car        \n"
                                    << "==== Date ======== Amount ======= Type\n"
        "======\n"
        "   12.12.09          5            Car        \n";
}

void StringUtilTest::test_signatureStripping()
{
    //QStringList tests;
    const QString test1 = QStringLiteral(
        "text1\n"
        "-- \n"
        "Signature Block1\n"
        "Signature Block1\n\n"
        "> text2\n"
        "> -- \n"
        "> Signature Block 2\n"
        "> Signature Block 2\n"
        ">> text3 -- not a signature block\n"
        ">> text3\n"
        ">>> text4\n"
        "> -- \n"
        "> Signature Block 4\n"
        "> Signature Block 4\n"
        ">>-------------\n"
        ">>-- text5 --\n"
        ">>-------------------\n"
        ">>-- \n"
        ">>\n"
        ">> Signature Block 5\n"
        "text6\n"
        "-- \n"
        "Signature Block 6\n");

    const QString test1Result = QStringLiteral(
        "text1\n"
        "> text2\n"
        ">> text3 -- not a signature block\n"
        ">> text3\n"
        ">>> text4\n"
        ">>-------------\n"
        ">>-- text5 --\n"
        ">>-------------------\n"
        "text6\n");

    QCOMPARE(StringUtil::stripSignature(test1), test1Result);

    const QString test2 = QStringLiteral(
        "text1\n"
        "> text2\n"
        ">> text3 -- not a signature block\n"
        ">> text3\n"
        ">>> text4\n"
        ">>-------------\n"
        ">>-- text5 --\n"
        ">>-------------------\n"
        "text6\n");

    // No actual signature - should stay the same
    QCOMPARE(StringUtil::stripSignature(test2), test2);

    const QString test3 = QStringLiteral(
        "text1\n"
        "-- \n"
        "Signature Block1\n"
        "Signature Block1\n\n"
        ">text2\n"
        ">-- \n"
        ">Signature Block 2\n"
        ">Signature Block 2\n"
        "> >text3\n"
        "> >text3\n"
        "> >-- \n"
        ">>Not Signature Block 3\n"
        "> > Not Signature Block 3\n"
        ">text4\n"
        ">-- \n"
        ">Signature Block 4\n"
        ">Signature Block 4\n"
        "text5\n"
        "-- \n"
        "Signature Block 5");

    const QString test3Result = QStringLiteral(
        "text1\n"
        ">text2\n"
        "> >text3\n"
        "> >text3\n"
        ">>Not Signature Block 3\n"
        "> > Not Signature Block 3\n"
        ">text4\n"
        "text5\n");

    QCOMPARE(StringUtil::stripSignature(test3), test3Result);

    const QString test4 = QStringLiteral(
        "Text 1\n"
        "-- \n"
        "First sign\n\n\n"
        "> From: bla\n"
        "> Texto 2\n\n"
        "> Aqui algo de texto.\n\n"
        ">> --\n"
        ">> Not Signature Block 2\n\n"
        "> Adios\n\n"
        ">> Texto 3\n\n"
        ">> --\n"
        ">> Not Signature block 3\n");

    const QString test4Result = QStringLiteral(
        "Text 1\n"
        "> From: bla\n"
        "> Texto 2\n\n"
        "> Aqui algo de texto.\n\n"
        ">> --\n"
        ">> Not Signature Block 2\n\n"
        "> Adios\n\n"
        ">> Texto 3\n\n"
        ">> --\n"
        ">> Not Signature block 3\n");

    QCOMPARE(StringUtil::stripSignature(test4), test4Result);

    const QString test5 = QStringLiteral(
        "-- \n"
        "-- ACME, Inc\n"
        "-- Joe User\n"
        "-- PHB\n"
        "-- Tel.: 555 1234\n"
        "--");

    QCOMPARE(StringUtil::stripSignature(test5), QString());

    const QString test6 = QStringLiteral(
        "Text 1\n\n\n\n"
        "> From: bla\n"
        "> Texto 2\n\n"
        "> Aqui algo de texto.\n\n"
        ">> Not Signature Block 2\n\n"
        "> Adios\n\n"
        ">> Texto 3\n\n"
        ">> --\n"
        ">> Not Signature block 3\n");

    // Again, no actual signature in here
    QCOMPARE(StringUtil::stripSignature(test6), test6);
}

void StringUtilTest::test_stripOffMessagePrefixBenchmark()
{
    const QString subject = QStringLiteral("Fwd: Hello World Subject");
    QBENCHMARK {
        QString result = StringUtil::stripOffPrefixes(subject);
        Q_UNUSED(result);
    }
}

void StringUtilTest::test_parseMailtoUrl_data()
{
    QTest::addColumn<QString>("mailToUrl");
    QTest::addColumn<bool>("toIsNotEmpty");
    QTest::addColumn<int>("numberOfTo");
    QTest::addColumn<int>("numberElement");

    QTest::newRow("1 mailto") << QStringLiteral("mailto:foo@kde.org") << true << 1 << 1;
    QTest::newRow("invalid (not mailto)") << QStringLiteral("http://www.kde.org") << false << 0 << 0;
    QTest::newRow("invalid (no email address") << QStringLiteral("mailto:") << false << 0 << 0;
    QTest::newRow("2 address") << QStringLiteral("mailto:foo@kde.org?to=foo2@kde.org") << true << 2 << 1;
    QTest::newRow("invalid") << QStringLiteral("fookde.org") << false << 0 << 0;
}

void StringUtilTest::test_parseMailtoUrl()
{
    QFETCH(QString, mailToUrl);
    QFETCH(bool, toIsNotEmpty);
    QFETCH(int, numberOfTo);
    QFETCH(int, numberElement);

    QUrl url(mailToUrl);
    auto list = StringUtil::parseMailtoUrl(url);
    //qDebug() << " list "<< list;
    QCOMPARE(list.count(), numberElement);
    if (numberOfTo > 0) {
        QCOMPARE(!list.at(0).second.isEmpty(), toIsNotEmpty);
        QCOMPARE(list.at(0).second.split(QLatin1String(", "), QString::SkipEmptyParts).count(), numberOfTo);
    }
}

void StringUtilTest::test_parseMailtoUrlExtra()
{
    const QByteArray ba("mailto:someone@example.com?subject=This%20is%20the%20subject&cc=someone_else@example.com&body=This%20is%20the%20body");
    QUrl url = QUrl(QUrl::fromPercentEncoding(ba));
    QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(url);
    QCOMPARE(data.size(), 4);
    for (int i = 0; i < 4; ++i) {
        if (data.at(i).first == QLatin1String("to")) {
            QCOMPARE(data.at(i).second, QLatin1String("someone@example.com"));
        } else if (data.at(i).first == QLatin1String("subject")) {
            QCOMPARE(data.at(i).second, QLatin1String("This is the subject"));
        } else if (data.at(i).first == QLatin1String("cc")) {
            QCOMPARE(data.at(i).second, QLatin1String("someone_else@example.com"));
        } else if (data.at(i).first == QLatin1String("body")) {
            QCOMPARE(data.at(i).second, QLatin1String("This is the body"));
        }
    }
}

void StringUtilTest::test_parseMailToBug366981()
{
    const QString ba(QStringLiteral("mailto:test@test.com?subject=test&body=line1%0D%0Aline2"));
    QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
    QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(urlDecoded);
    QCOMPARE(data.size(), 3);
    for (int i = 0; i < 3; ++i) {
        if (data.at(i).first == QLatin1String("to")) {
            QCOMPARE(i, 0);
            QCOMPARE(data.at(i).second, QLatin1String("test@test.com"));
        } else if (data.at(i).first == QLatin1String("subject")) {
            QCOMPARE(i, 1);
            QCOMPARE(data.at(i).second, QLatin1String("test"));
        } else if (data.at(i).first == QLatin1String("body")) {
            QCOMPARE(i, 2);
            QCOMPARE(data.at(i).second, QLatin1String("line1\r\nline2"));
        }
    }
}

void StringUtilTest::test_parseDuplicateQueryItems()
{
    const QString ba(QStringLiteral("mailto:test@test.com?subject=test&body=line1%0D%0Aline2&cc=someone_else@example.com&cc=someone_else2@example.com"));
    QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
    QList<QPair<QString, QString> > values = StringUtil::parseMailtoUrl(urlDecoded);
    QCOMPARE(values.size(), 5);
    int valueCC = 0;
    for (int i = 0; i < values.size(); ++i) {
        if (values.at(i).first == QLatin1String("to")) {
            QCOMPARE(values.at(i).second, QLatin1String("test@test.com"));
        } else if (values.at(i).first == QLatin1String("subject")) {
            QCOMPARE(values.at(i).second, QLatin1String("test"));
        } else if (values.at(i).first == QLatin1String("body")) {
            QCOMPARE(values.at(i).second, QLatin1String("line1\r\nline2"));
        } else if (values.at(i).first == QLatin1String("cc")) {
            const QString ccVal = values.at(i).second;
            valueCC++;
            if ((ccVal != QLatin1String("someone_else@example.com")) && (ccVal != QLatin1String("someone_else2@example.com"))) {
                QVERIFY(false);
            }
        }
    }
    QCOMPARE(valueCC, 2);
}

void StringUtilTest::test_parseMAilToBug402378()
{
    const QString ba(QStringLiteral(
                         "mailto:?body=infotbm.com https://www.infotbm.com/fr/routes/id=-0.624162|44.849958&type=address/datetime=20181226T143038&id=stop_area:TBT:SA:HTLEV&time_type=arrival&type=stop_area/0&subject=Votre itineraire avec TBM"));
    QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
    QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(urlDecoded);
    QCOMPARE(data.size(), 6);
    QCOMPARE(data.at(0).first, QLatin1String("body"));
    QCOMPARE(data.at(0).second, QLatin1String(
                 "infotbm.com https://www.infotbm.com/fr/routes/id=-0.624162|44.849958"));
    QCOMPARE(data.at(1).first, QLatin1String("type"));
    QCOMPARE(data.at(1).second, QLatin1String("address/datetime=20181226T143038"));
    QCOMPARE(data.at(2).first, QLatin1String("id"));
    QCOMPARE(data.at(2).second, QLatin1String("stop_area:TBT:SA:HTLEV"));
    QCOMPARE(data.at(3).first, QLatin1String("time_type"));
    QCOMPARE(data.at(3).second, QLatin1String("arrival"));
    QCOMPARE(data.at(4).first, QLatin1String("type"));
    QCOMPARE(data.at(4).second, QLatin1String("stop_area/0"));
    QCOMPARE(data.at(5).first, QLatin1String("subject"));
    QCOMPARE(data.at(5).second, QLatin1String("Votre itineraire avec TBM"));
}

void StringUtilTest::test_parseMailToBug406208()
{
    {
        QString ba(QStringLiteral(
                       "mailto:?body=http%3A%2F%2Fwww.lecourrierdelarchitecte.com%2Farticle_8428&subject=Le%20Courrier%20l'effet%20%23metoo%20%3F"));
        QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
        qDebug() << " urlDecoded" << urlDecoded.authority(QUrl::FullyDecoded);
        QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(urlDecoded);
        QCOMPARE(data.size(), 2);
        QCOMPARE(data.at(0).first, QLatin1String("body"));
        QCOMPARE(data.at(0).second, QLatin1String(
                     "http://www.lecourrierdelarchitecte.com/article_8428"));
        QCOMPARE(data.at(1).first, QLatin1String("subject"));
        QCOMPARE(data.at(1).second, QLatin1String("Le Courrier l'effet #metoo ?"));
    }
    {
        QString ba(QStringLiteral(
                       "mailto:?body=http%3A%2F%2Fwww.lecourrierdelarchitecte.com%2Farticle_8428%20%23%23bla&subject=Le%20Courrier%20l'effet%20%23metoo%20%3F"));
        QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
        qDebug() << " urlDecoded" << urlDecoded.authority(QUrl::FullyDecoded);
        QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(urlDecoded);
        QCOMPARE(data.size(), 2);
        QCOMPARE(data.at(0).first, QLatin1String("body"));
        QCOMPARE(data.at(0).second, QLatin1String(
                     "http://www.lecourrierdelarchitecte.com/article_8428 ##bla"));
        QCOMPARE(data.at(1).first, QLatin1String("subject"));
        QCOMPARE(data.at(1).second, QLatin1String("Le Courrier l'effet #metoo ?"));
    }
}

void StringUtilTest::test_parseMailToBug832795()
{
    const QString ba(QStringLiteral(
                         "mailto:832795@bugs.debian.org?In-Reply-To=%3C146974194340.26747.4814466130640572267.reportbug%40portux.lan.naturalnet.de%3E&subject=Re%3A%20kmail%3A%20unescaping%20mailto%3A%20links%20broken&body=On%20Thu%2C%2028%20Jul%202016References=%3C146974194340.26747.4814466130640572267.reportbug%40portux.lan.naturalnet.de%3Ebody=On%20Thu%2C%2028%20Jul%202016%2023%3A39%3A03%20%2B0200%20Dominik%20George%20%3Cnik%40naturalnet.de%3E%20wrote%3A%0A%3E%20Package%3A%20kmail%0A%3E%20Version%3A%204%3A16.04.3-1%0A"));
    QUrl urlDecoded(QUrl::fromPercentEncoding(ba.toUtf8()));
    QList<QPair<QString, QString> > data = StringUtil::parseMailtoUrl(urlDecoded);
    QCOMPARE(data.size(), 4);
    QCOMPARE(data.at(0).first, QLatin1String("to"));
    QCOMPARE(data.at(0).second, QLatin1String("832795@bugs.debian.org"));
    QCOMPARE(data.at(1).first, QLatin1String("In-Reply-To"));
    QCOMPARE(data.at(1).second, QLatin1String("<146974194340.26747.4814466130640572267.reportbug@portux.lan.naturalnet.de>"));
    QCOMPARE(data.at(2).first, QLatin1String("subject"));
    QCOMPARE(data.at(2).second, QLatin1String("Re: kmail: unescaping mailto: links broken"));
    QCOMPARE(data.at(3).first, QLatin1String("body"));
    QCOMPARE(data.at(3).second, QLatin1String(
                 "On Thu, 28 Jul 2016References=<146974194340.26747.4814466130640572267.reportbug@portux.lan.naturalnet.de>body=On Thu, 28 Jul 2016 23:39:03 +0200 Dominik George <nik@naturalnet.de> wrote:\n> Package: kmail\n> Version: 4:16.04.3-1\n"));
}

void StringUtilTest::test_stripOffMessagePrefix_data()
{
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QString>("result");
    QTest::newRow("no strip needed") << QStringLiteral("Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("No default reply forward") << QStringLiteral("AA: Hello World Subject") << QStringLiteral("AA: Hello World Subject");
    QTest::newRow("Default Reply Re:") << QStringLiteral("Re: Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("Default Forward FW:") << QStringLiteral("FW: Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("Default Forward Fwd:") << QStringLiteral("Fwd: Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("Default Reply Re  :") << QStringLiteral("Re  : Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("Default Reply Re1:") << QStringLiteral("Re1: Hello World Subject") << QStringLiteral("Hello World Subject");
    QTest::newRow("Default Reply Re[2]:") << QStringLiteral("Re[2]: Hello World Subject") << QStringLiteral("Hello World Subject");
}

void StringUtilTest::test_stripOffMessagePrefix()
{
    QFETCH(QString, subject);
    QFETCH(QString, result);
    const QString subjectAfterStrip = StringUtil::stripOffPrefixes(subject);
    QCOMPARE(subjectAfterStrip, result);
}

void StringUtilTest::test_formatQuotePrefix_data()
{
    QTest::addColumn<QString>("quotePattern");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("result");

    QTest::newRow("empty") << QString() << QString() << QString();
    QTest::newRow("default") << QStringLiteral("> ") << QStringLiteral("Jon Doe") << QStringLiteral("> ");
    QTest::newRow("initials") << QStringLiteral("| %f |") << QStringLiteral("Jon Doe") << QStringLiteral("| JD |");
    QTest::newRow("initials one name") << QStringLiteral("| %f |") << QStringLiteral("Jon") << QStringLiteral("| Jo |");
    QTest::newRow("initials one letter") << QStringLiteral("| %f |") << QStringLiteral("J") << QStringLiteral("| J |");
    QTest::newRow("initials empty name") << QStringLiteral("| %f |") << QString() << QStringLiteral("|  |");
    QTest::newRow("percent") << QStringLiteral("%% %_ %a") << QString() << QStringLiteral("%   %a");
}

void StringUtilTest::test_formatQuotePrefix()
{
    QFETCH(QString, quotePattern);
    QFETCH(QString, from);
    QFETCH(QString, result);
    QCOMPARE(MessageCore::StringUtil::formatQuotePrefix(quotePattern, from), result);
}
