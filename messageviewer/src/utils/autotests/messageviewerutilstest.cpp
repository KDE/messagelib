/*
  SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "messageviewerutilstest.h"
#include "utils/messageviewerutil.h"
#include <QTest>
QTEST_GUILESS_MAIN(MessageViewerUtilsTest)

MessageViewerUtilsTest::MessageViewerUtilsTest(QObject *parent)
    : QObject(parent)
{
}

void MessageViewerUtilsTest::shouldExcludeHeader_data()
{
    QTest::addColumn<QString>("header");
    QTest::addColumn<bool>("exclude");
    QTest::newRow("emptylist") << QString() << false;
    QTest::newRow("div1") << QStringLiteral("<div><p>ff</p></div></head>") << true;
    QTest::newRow("body1") << QStringLiteral("<style>\nbody > div:nth-child(2) {\ndisplay: none !important;\n}\n</style>") << true;
}

void MessageViewerUtilsTest::shouldExcludeHeader()
{
    QFETCH(QString, header);
    QFETCH(bool, exclude);
    QCOMPARE(MessageViewer::Util::excludeExtraHeader(header), exclude);
}

void MessageViewerUtilsTest::shouldContainsExternalReferences_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("extraHead");
    QTest::addColumn<bool>("hasExternalReference");
    QTest::newRow("noimage.txt") << QStringLiteral("noimage.txt") << QString() << false;
    QTest::newRow("image.txt") << QStringLiteral("image.txt") << QString() << true;
    QTest::newRow("image2.txt") << QStringLiteral("image2.txt") << QString() << true;
    QTest::newRow("noimage2.txt") << QStringLiteral("noimage2.txt") << QString() << false;
    QTest::newRow("noimage3.txt") << QStringLiteral("noimage3.txt") << QString() << false;
    //    before
    //      PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(noimage.txt)
    //      RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage.txt":
    //      0.0015 msecs per iteration (total: 52, iterations: 32768)
    //      PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(image.txt)
    //      RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"image.txt":
    //      0.0018 msecs per iteration (total: 60, iterations: 32768)
    //      PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(image2.txt)
    //      RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"image2.txt":
    //      0.058 msecs per iteration (total: 60, iterations: 1024)
    //      PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(noimage2.txt)
    //      RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage2.txt":
    //      0.060 msecs per iteration (total: 62, iterations: 1024)
    //      PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(noimage3.txt)
    //      RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage3.txt":

    // AFTER
    // RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage.txt":
    //     0.0014 msecs per iteration (total: 98, iterations: 65536)
    // PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(image.txt)
    // RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"image.txt":
    //     0.0017 msecs per iteration (total: 57, iterations: 32768)
    // PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(image2.txt)
    // RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"image2.txt":
    //     0.0073 msecs per iteration (total: 60, iterations: 8192)
    // PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(noimage2.txt)
    // RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage2.txt":
    //     0.0023 msecs per iteration (total: 78, iterations: 32768)
    // PASS   : MessageViewerUtilsTest::shouldContainsExternalReferences(noimage3.txt)
    // RESULT : MessageViewerUtilsTest::shouldContainsExternalReferences():"noimage3.txt":
    //     0.0023 msecs per iteration (total: 78, iterations: 32768)
}

void MessageViewerUtilsTest::shouldContainsExternalReferences()
{
    QFETCH(QString, filename);
    QFETCH(QString, extraHead);
    QFETCH(bool, hasExternalReference);
    const QString curPath = QStringLiteral(MESSAGEVIEWER_UTIL_DATA_DIR "/");
    QFile file(curPath + filename);
    QVERIFY(file.open(QIODevice::ReadOnly));
    const QString html = QString::fromLatin1(file.readAll());
    QBENCHMARK {
        QCOMPARE(MessageViewer::Util::containsExternalReferences(html, extraHead), hasExternalReference);
    }
}

void MessageViewerUtilsTest::shouldExtractHtml()
{
    QFETCH(QString, input);
    QFETCH(MessageViewer::Util::HtmlMessageInfo, output);
    const MessageViewer::Util::HtmlMessageInfo processHtml = MessageViewer::Util::processHtml(input);
    bool equal = processHtml == output;
    if (!equal) {
        qDebug() << " processed " << processHtml;
        qDebug() << " ref " << output;
    }
    QVERIFY(equal);
}

void MessageViewerUtilsTest::shouldExtractHtml_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<MessageViewer::Util::HtmlMessageInfo>("output");
    QTest::newRow("empty") << QString() << MessageViewer::Util::HtmlMessageInfo();
    {
        const QString input = QStringLiteral("<html><head></head><body>foo</body></html>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral("foo");
        output.bodyStyle = QStringLiteral("<body>");
        QTest::newRow("test1") << input << output;
    }
    {
        const QString input = QStringLiteral("<html><head></head><body>foo</body></html></div>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral("foo");
        output.bodyStyle = QStringLiteral("<body>");
        QTest::newRow("test2") << input << output;
    }
    {
        const QString input = QStringLiteral(
            "That's interesting. I don't see new commits or anything relevant to it on the author's releases. I don't actually know why the author uses the "
            "other library as they do seem to have similar data... Maybe some other functions that are easier to use.<br><br><br>All the "
            "best,<br><br>C<br><br><br>-------- Original Message --------<br>On Mar 3, 2020, 09:56, foo wrote:<blockquote "
            "class=\"protonmail_quote\"><br><!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
            "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\r\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\r\np, li { "
            "white-space: pre-wrap; }\r\n</style></head><body>\r\n<p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your "
            "PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent installation I had an issue with the Orthanc-Module, during initialization of the "
            "database:</p>\r\n<p><span style=\" font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" "
            "font-family:'monospace','Noto Sans';\"><br />'Something' as changed in the setup of timezone data (between December and now so to "
            "say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">To make the story short, this module pytzdata comes from the "
            "pypi-package pytzdata and contains basically the same stuff as pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto "
            "Sans';\">Except that pendulum and pytzdata is from the same author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you "
            "have an idea why he not uses pytz, as everybody else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: "
            "@coogor</p>\r\n<p>Matrix: @docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA "
            "290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p></body></html></div>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.extraHead = QStringLiteral("<meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\r\np, li { white-space: pre-wrap; }\r\n</style>");
        output.bodyStyle = QStringLiteral("<body>");
        output.htmlSource = QStringLiteral(
            "That's interesting. I don't see new commits or anything relevant to it on the author's releases. I don't actually know why the author uses the "
            "other library as they do seem to have similar data... Maybe some other functions that are easier to use.<br><br><br>All the "
            "best,<br><br>C<br><br><br>-------- Original Message --------<br>On Mar 3, 2020, 09:56, foo wrote:<blockquote "
            "class=\"protonmail_quote\"><br><p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent "
            "installation I had an issue with the Orthanc-Module, during initialization of the database:</p>\r\n<p><span style=\" "
            "font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" font-family:'monospace','Noto Sans';\"><br />'Something' "
            "as changed in the setup of timezone data (between December and now so to say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto "
            "Sans';\">To make the story short, this module pytzdata comes from the pypi-package pytzdata and contains basically the same stuff as "
            "pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Except that pendulum and pytzdata is from the same "
            "author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you have an idea why he not uses pytz, as everybody "
            "else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: @coogor</p>\r\n<p>Matrix: "
            "@docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA "
            "290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p>");
        QTest::newRow("bug418482") << input << output;
    }
    {
        const QString input = QStringLiteral(
            "HTML REPLY<br>\nSECOND LINE<br>\n-- <br>\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/plain; charset=utf-8\" /></head><body  "
            "style=\"overflow-wrap:break-word; word-break: break-word;white-space:pre-wrap;\"><div>You wrote:<blockquote style=\"margin: 0.8ex 0pt 0pt 0.8ex; "
            "border-left: 1px solid rgb(204, 204, 204); padding-left: 1ex;\">HTML QUOTE\n\nSECOND LINE\n</blockquote></div></body></html>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral(
            "HTML REPLY<br>\nSECOND LINE<br>\n-- <br>\n<div>You wrote:<blockquote style=\"margin: 0.8ex 0pt 0pt 0.8ex; border-left: 1px solid rgb(204, 204, "
            "204); padding-left: 1ex;\">HTML QUOTE\n\nSECOND LINE\n</blockquote></div>");
        output.bodyStyle = QStringLiteral("<body  style=\"overflow-wrap:break-word; word-break: break-word;white-space:pre-wrap;\">");
        output.extraHead = QStringLiteral("<meta http-equiv=\"Content-Type\" content=\"text/plain; charset=utf-8\" />");
        QTest::newRow("bug419949") << input << output;
    }
    {
        // Bug head + div
        const QString input = QStringLiteral(
            "<html style=\"background: #5555ff;min-height:500px;\">\n<head>\n<style>\nbody div div {display: "
            "none;}\n</style>\n<div>\n<p><b>goo<\b></p></div><p>ff</p></head><body></body></html>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QString();
        output.bodyStyle = QStringLiteral("<body>");
        output.extraHead = QString();
        QTest::newRow("headdiv") << input << output;
    }
}

void MessageViewerUtilsTest::shouldExtractBodyStyle()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    QCOMPARE(MessageViewer::Util::parseBodyStyle(input), output);
}

void MessageViewerUtilsTest::shouldExtractBodyStyle_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("test1") << QStringLiteral("<body  style=\"overflow-wrap:break-word; word-break: break-word;white-space:pre-wrap;\">")
                           << QStringLiteral(" style=\"overflow-wrap:break-word;word-break: break-word;\"");
}

void MessageViewerUtilsTest::shouldExtractHtmlBenchmark()
{
    QFETCH(QString, input);
    QFETCH(MessageViewer::Util::HtmlMessageInfo, output);
    QBENCHMARK {
        const MessageViewer::Util::HtmlMessageInfo processHtml = MessageViewer::Util::processHtml(input);
        bool equal = processHtml == output;
        if (!equal) {
            qDebug() << " processed " << processHtml;
            qDebug() << " ref " << output;
        }
        QVERIFY(equal);
    }
}

void MessageViewerUtilsTest::shouldExtractHtmlBenchmark_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<MessageViewer::Util::HtmlMessageInfo>("output");
    QTest::newRow("empty") << QString() << MessageViewer::Util::HtmlMessageInfo();
    {
        const QString input = QStringLiteral("<html><head></head><body>foo</body></html>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral("foo");
        output.bodyStyle = QStringLiteral("<body>");
        QTest::newRow("test1") << input << output;
    }
    {
        const QString input = QStringLiteral("<html><head></head><body>foo</body></html></div>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral("foo");
        output.bodyStyle = QStringLiteral("<body>");
        QTest::newRow("test2") << input << output;
    }
    {
        const QString input = QStringLiteral(
            "That's interesting. I don't see new commits or anything relevant to it on the author's releases. I don't actually know why the author uses the "
            "other library as they do seem to have similar data... Maybe some other functions that are easier to use.<br><br><br>All the "
            "best,<br><br>C<br><br><br>-------- Original Message --------<br>On Mar 3, 2020, 09:56, foo wrote:<blockquote "
            "class=\"protonmail_quote\"><br><!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" "
            "\"http://www.w3.org/TR/REC-html40/strict.dtd\">\r\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\r\np, li { "
            "white-space: pre-wrap; }\r\n</style></head><body>\r\n<p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your "
            "PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent installation I had an issue with the Orthanc-Module, during initialization of the "
            "database:</p>\r\n<p><span style=\" font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" "
            "font-family:'monospace','Noto Sans';\"><br />'Something' as changed in the setup of timezone data (between December and now so to "
            "say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">To make the story short, this module pytzdata comes from the "
            "pypi-package pytzdata and contains basically the same stuff as pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto "
            "Sans';\">Except that pendulum and pytzdata is from the same author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you "
            "have an idea why he not uses pytz, as everybody else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: "
            "@coogor</p>\r\n<p>Matrix: @docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA "
            "290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p></body></html></div>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.extraHead = QStringLiteral("<meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\r\np, li { white-space: pre-wrap; }\r\n</style>");
        output.bodyStyle = QStringLiteral("<body>");

        output.htmlSource = QStringLiteral(
            "That's interesting. I don't see new commits or anything relevant to it on the author's releases. I don't actually know why the author uses the "
            "other library as they do seem to have similar data... Maybe some other functions that are easier to use.<br><br><br>All the "
            "best,<br><br>C<br><br><br>-------- Original Message --------<br>On Mar 3, 2020, 09:56, foo wrote:<blockquote "
            "class=\"protonmail_quote\"><br><p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent "
            "installation I had an issue with the Orthanc-Module, during initialization of the database:</p>\r\n<p><span style=\" "
            "font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" font-family:'monospace','Noto Sans';\"><br />'Something' "
            "as changed in the setup of timezone data (between December and now so to say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto "
            "Sans';\">To make the story short, this module pytzdata comes from the pypi-package pytzdata and contains basically the same stuff as "
            "pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Except that pendulum and pytzdata is from the same "
            "author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you have an idea why he not uses pytz, as everybody "
            "else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: @coogor</p>\r\n<p>Matrix: "
            "@docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA "
            "290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p>");
        QTest::newRow("bug418482") << input << output;
    }
    {
        const QString input = QStringLiteral(
            "HTML REPLY<br>\nSECOND LINE<br>\n-- <br>\n<html><head><meta http-equiv=\"Content-Type\" content=\"text/plain; charset=utf-8\" /></head><body  "
            "style=\"overflow-wrap:break-word; word-break: break-word;white-space:pre-wrap;\"><div>You wrote:<blockquote style=\"margin: 0.8ex 0pt 0pt 0.8ex; "
            "border-left: 1px solid rgb(204, 204, 204); padding-left: 1ex;\">HTML QUOTE\n\nSECOND LINE\n</blockquote></div></body></html>");
        MessageViewer::Util::HtmlMessageInfo output;
        output.htmlSource = QStringLiteral(
            "HTML REPLY<br>\nSECOND LINE<br>\n-- <br>\n<div>You wrote:<blockquote style=\"margin: 0.8ex 0pt 0pt 0.8ex; border-left: 1px solid rgb(204, 204, "
            "204); padding-left: 1ex;\">HTML QUOTE\n\nSECOND LINE\n</blockquote></div>");
        output.bodyStyle = QStringLiteral("<body  style=\"overflow-wrap:break-word; word-break: break-word;white-space:pre-wrap;\">");
        output.extraHead = QStringLiteral("<meta http-equiv=\"Content-Type\" content=\"text/plain; charset=utf-8\" />");
        QTest::newRow("bug419949") << input << output;
    }
    // Before
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"empty":
    //     0.099 msecs per iteration (total: 51, iterations: 512)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(test1)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"test1":
    //     0.10 msecs per iteration (total: 54, iterations: 512)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(bug418482)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"bug418482":
    //     0.11 msecs per iteration (total: 58, iterations: 512)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(bug419949)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"bug419949":
    //     0.10 msecs per iteration (total: 54, iterations: 512)

    // After
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"empty":
    //     0.0014 msecs per iteration (total: 95, iterations: 65536)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(test1)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"test1":
    //     0.0031 msecs per iteration (total: 51, iterations: 16384)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(test2)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"test2":
    //     0.0031 msecs per iteration (total: 51, iterations: 16384)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(bug418482)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"bug418482":
    //     0.0095 msecs per iteration (total: 78, iterations: 8192)
    // PASS   : MessageViewerUtilsTest::shouldExtractHtml(bug419949)
    // RESULT : MessageViewerUtilsTest::shouldExtractHtml():"bug419949":
    //     0.0046 msecs per iteration (total: 76, iterations: 16384)
}

#include "moc_messageviewerutilstest.cpp"
