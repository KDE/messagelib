/*
  Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

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
    QTest::newRow("REFRESH1") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\'REFRESH\'></head>") << true;
    QTest::newRow("REFRESH2") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"REFRESH\"></head>") << true;
    QTest::newRow("REFRESH3") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"refresh\"></head>") << true;
    QTest::newRow("REFRESH4") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\"&#82;EFRESH\"></head>") << true;
    QTest::newRow("REFRESH5") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv=\'&#82;EFRESH\'></head>") << true;
    QTest::newRow("REFRESH6") << QStringLiteral("<meta content=\"0;URL=http://www.kde.org\" http-equiv= \"REFRESH\"></head>") << true;
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
    QCOMPARE(MessageViewer::Util::containsExternalReferences(html, extraHead), hasExternalReference);
}


void MessageViewerUtilsTest::shouldExtractHtml()
{
    QFETCH(QString, input);
    QFETCH(QString, output);
    QString header;
    const QString processHtml = MessageViewer::Util::processHtml(input, header);
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
    QTest::addColumn<QString>("output");
    QTest::newRow("empty") << QString() << QString();
    QString input = QStringLiteral("<html><head></head><body>foo</body></html>");
    QString output = QStringLiteral("foo");
    QTest::newRow("test1") << input << output;

    input = QStringLiteral("That's interesting. I don't see new commits or anything relevant to it on the author's releases. I don't actually know why the author uses the other library as they do seem to have similar data... Maybe some other functions that are easier to use.<br><br><br>All the best,<br><br>C<br><br><br>-------- Original Message --------<br>On Mar 3, 2020, 09:56, foo wrote:<blockquote class=\"protonmail_quote\"><br><!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\r\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\r\np, li { white-space: pre-wrap; }\r\n</style></head><body>\r\n<p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent installation I had an issue with the Orthanc-Module, during initialization of the database:</p>\r\n<p><span style=\" font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" font-family:'monospace','Noto Sans';\"><br />'Something' as changed in the setup of timezone data (between December and now so to say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">To make the story short, this module pytzdata comes from the pypi-package pytzdata and contains basically the same stuff as pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Except that pendulum and pytzdata is from the same author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you have an idea why he not uses pytz, as everybody else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: @coogor</p>\r\n<p>Matrix: @docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA 290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p></body></html></div>");
    output = QStringLiteral("<p>Hey bla,</p>\r\n<p>&nbsp;</p>\r\n<p>how are things going? Done your PhD?</p>\r\n<p>&nbsp;</p>\r\n<p>On a recent installation I had an issue with the Orthanc-Module, during initialization of the database:</p>\r\n<p><span style=\" font-family:'monospace';\"><br />   from .datetime import DateTime </span><span style=\" font-family:'monospace','Noto Sans';\"><br />'Something' as changed in the setup of timezone data (between December and now so to say).</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">To make the story short, this module pytzdata comes from the pypi-package pytzdata and contains basically the same stuff as pytz.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Except that pendulum and pytzdata is from the same author.</span></p>\r\n<p><span style=\" font-family:'monospace','Noto Sans';\">Do you have an idea why he not uses pytz, as everybody else?</span></p>\r\n<p>&nbsp;</p>\r\n<p>Thanks</p>\r\n<p>&nbsp;</p>\r\n<p>-- </p>\r\n<p>T: @coogor</p>\r\n<p>Matrix: @docb:matrix.org</p>\r\n<p>PGP Fingerprint: 2E7F 3A19 A4A4 844A 3D09 7656 822D EB64 A3BA 290D</p>\r\n<p>&nbsp;</p>\r\n<p>http://gnuhealth.ghf2020.org</p></body></html></div>");
    QTest::newRow("bug418482") << input << output;
}
