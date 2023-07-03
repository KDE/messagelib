/*
   SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

*/

#include "showonlymimeparttest.h"

#include "setupenv.h"
#include "testcsshelper.h"
#include "util.h"

#include <MessageViewer/FileHtmlWriter>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/ObjectTreeParser>

#include <QString>
#include <QTest>

using namespace MessageViewer;

void ShowOnlyMimePartTest::initTestCase()
{
    Test::setupEnv();
}

void ShowOnlyMimePartTest::testDrawFrame_data()
{
    QTest::addColumn<int>("content");
    QTest::addColumn<bool>("showOnlyMimePart");

    QTest::newRow("only first part") << 0 << true;
    QTest::newRow("only first part - render complete") << 0 << false;
    QTest::newRow("only second part") << 1 << true;
    QTest::newRow("only second part - render complete") << 1 << false;
    QTest::newRow("only third part") << 2 << true;
    QTest::newRow("only third part - render complete") << 2 << false;
}

void ShowOnlyMimePartTest::testDrawFrame()
{
    QFETCH(int, content);
    QFETCH(bool, showOnlyMimePart);

    QString commonName(QStringLiteral("frametest.mbox.html.content.") + QString::number(content)
                       + ((showOnlyMimePart) ? QStringLiteral(".single") : QStringLiteral(".full")));

    QString outFileName(commonName);

    QString referenceFileName(QStringLiteral(MAIL_DATA_DIR) + QLatin1Char('/') + commonName);
    // load input mail
    KMime::Message::Ptr msg(Test::readAndParseMail(QStringLiteral("frametest.mbox")));

    // render the mail
    FileHtmlWriter fileWriter(outFileName);
    QImage paintDevice;
    Test::TestCSSHelper cssHelper(&paintDevice);
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource testSource(&fileWriter, &cssHelper);

    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);

    QVERIFY(msg->contents().size() > content);

    otp.parseObjectTree(msg->contents().at(content), showOnlyMimePart);

    fileWriter.begin();
    CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
    fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
    testSource.render(otp.parsedPart(), showOnlyMimePart);
    fileWriter.write(QStringLiteral("</body></html>"));
    fileWriter.end();

    Test::compareFile(outFileName, referenceFileName);
    msg.clear();
}

void ShowOnlyMimePartTest::testRelated_data()
{
    QTest::addColumn<int>("content");
    QTest::addColumn<bool>("showOnlyMimePart");

    QTest::newRow("only html") << 0 << true;
    QTest::newRow("only html - render complete") << 0 << false;
    QTest::newRow("only image") << 1 << true;
    QTest::newRow("only image - render complete") << 1 << false;
}

void ShowOnlyMimePartTest::testRelated()
{
    QFETCH(int, content);
    QFETCH(bool, showOnlyMimePart);

    QString commonName(QStringLiteral("html-multipart-related.mbox.html.content.") + QString::number(content)
                       + ((showOnlyMimePart) ? QStringLiteral(".single") : QStringLiteral(".full")));

    QString outFileName(commonName);

    QString referenceFileName(QStringLiteral(MAIL_DATA_DIR) + QLatin1Char('/') + commonName);
    // load input mail
    KMime::Message::Ptr msg(Test::readAndParseMail(QStringLiteral("html-multipart-related.mbox")));

    // render the mail
    FileHtmlWriter fileWriter(outFileName);
    QImage paintDevice;
    Test::TestCSSHelper cssHelper(&paintDevice);
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource testSource(&fileWriter, &cssHelper);

    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);

    QVERIFY(msg->contents().size() > content);

    otp.parseObjectTree(msg->contents().at(content), showOnlyMimePart);

    fileWriter.begin();
    CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
    fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
    testSource.render(otp.parsedPart(), showOnlyMimePart);
    fileWriter.write(QStringLiteral("</body></html>"));
    fileWriter.end();

    Test::compareFile(outFileName, referenceFileName);
    msg.clear();
}

QTEST_MAIN(ShowOnlyMimePartTest)

#include "moc_showonlymimeparttest.cpp"
