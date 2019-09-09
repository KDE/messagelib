/*
   Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "showonlymimeparttest.h"

#include "util.h"
#include "setupenv.h"
#include "testcsshelper.h"

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/MessagePart>
#include <MessageViewer/FileHtmlWriter>

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
    const KMime::Message::Ptr msg(Test::readAndParseMail(QStringLiteral("frametest.mbox")));

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
    fileWriter.write(cssHelper.htmlHead(false));
    testSource.render(otp.parsedPart(), showOnlyMimePart);
    fileWriter.write(QStringLiteral("</body></html>"));
    fileWriter.end();

    Test::compareFile(outFileName, referenceFileName);
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
    const KMime::Message::Ptr msg(Test::readAndParseMail(QStringLiteral("html-multipart-related.mbox")));

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
    fileWriter.write(cssHelper.htmlHead(false));
    testSource.render(otp.parsedPart(), showOnlyMimePart);
    fileWriter.write(QStringLiteral("</body></html>"));
    fileWriter.end();

    Test::compareFile(outFileName, referenceFileName);
}

QTEST_MAIN(ShowOnlyMimePartTest)
