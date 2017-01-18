/*
  Copyright (c) 2010 Volker Krause <vkrause@kde.org>
  Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "rendertest.h"
#include "setupenv.h"
#include "testcsshelper.h"
#include "util.h"

#include <MimeTreeParser/FileHtmlWriter>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/MessagePart>

#include <KMime/Message>

#include <QDir>
#include <QProcess>
#include <QTest>

using namespace MessageViewer;

void RenderTest::initTestCase()
{
    Test::setupEnv();
}

void RenderTest::testRenderSmart_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html")) << QString(file + QStringLiteral(".out")) << QStringLiteral("smart") << false << QString();
    }
}

void RenderTest::testRenderSmart()
{
    testRender();
}

void RenderTest::testRenderSmartAsync_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));

    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".inProgress.html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html")) << QString(file + QStringLiteral(".out")) << QStringLiteral("smart") << false << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".inProgress.html"));
    }
}

void RenderTest::testRenderSmartAsync()
{
    testRender();
}

void RenderTest::testRenderSmartDetails_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/details/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("smart") << true << QString();
    }
}

void RenderTest::testRenderSmartDetails()
{
    testRender();
}

void RenderTest::testRenderInlined_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/inlined/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("inlined") << false << QString();
    }
}

void RenderTest::testRenderInlined()
{
    testRender();
}

void RenderTest::testRenderIconic_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/iconic/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("iconic") << false << QString();
    }
}

void RenderTest::testRenderIconic()
{
    testRender();
}

void RenderTest::testRenderHidden_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/hidden/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("hidden") << false << QString();
    }
}

void RenderTest::testRenderHidden()
{
    testRender();
}

void RenderTest::testRenderHeaderOnly_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");
    QTest::addColumn<bool>("showSignatureDetails");
    QTest::addColumn<QString>("asyncFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/headeronly/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("headeronly") << false << QString();
    }
}

void RenderTest::testRenderHeaderOnly()
{
    testRender();
}

QString renderTreeHelper(const MimeTreeParser::Interface::MessagePart::Ptr &messagePart, QString indent)
{
    QString ret;
    const QString line = QStringLiteral("%1 * %3\n").arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    ret += line;

    indent += QStringLiteral(" ");
    const auto m = messagePart.dynamicCast<MimeTreeParser::MessagePart>();
    if (m) {
        foreach (const auto &subPart, m->subParts()) {
            ret += renderTreeHelper(subPart, indent);
        }
    }
    return ret;
}

void RenderTest::testRenderTree(const MimeTreeParser::MessagePart::Ptr &messagePart)
{
    QString renderedTree = renderTreeHelper(messagePart, QString());

    QFETCH(QString, mailFileName);
    QFETCH(QString, outFileName);
    const QString treeFileName = QLatin1String(MAIL_DATA_DIR) + QLatin1Char('/') + mailFileName + QStringLiteral(".tree");
    const QString outTreeFileName = outFileName + QStringLiteral(".tree");

    {
        QFile f(outTreeFileName);
        f.open(QIODevice::WriteOnly);
        f.write(renderedTree.toUtf8());
        f.close();
    }
    // compare to reference file
    const QStringList args = QStringList()
                             << QStringLiteral("-u")
                             << treeFileName
                             << outTreeFileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}

void RenderTest::testRender()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);
    QFETCH(QString, outFileName);
    QFETCH(QString, attachmentStrategy);
    QFETCH(bool, showSignatureDetails);
    QFETCH(QString, asyncFileName);

    const QString htmlFileName = outFileName + QStringLiteral(".html");
    const bool bAsync = !asyncFileName.isEmpty();

    // load input mail
    const KMime::Message::Ptr msg(Test::readAndParseMail(mailFileName));

    // render the mail
    MimeTreeParser::FileHtmlWriter fileWriter(outFileName);
    QImage paintDevice;
    Test::TestCSSHelper cssHelper(&paintDevice);
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource testSource(&fileWriter, &cssHelper);
    testSource.setAllowDecryption(true);
    testSource.setAttachmentStrategy(attachmentStrategy);
    testSource.setShowSignatureDetails(showSignatureDetails);

    QEventLoop loop;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    connect(&nodeHelper, &MimeTreeParser::NodeHelper::update, &loop, &QEventLoop::quit);
    otp.setAllowAsync(bAsync);

    fileWriter.begin(QString());
    fileWriter.queue(cssHelper.htmlHead(false));

    otp.parseObjectTree(msg.data());

    fileWriter.queue(QStringLiteral("</body></html>"));
    fileWriter.flush();
    fileWriter.end();

    if (!bAsync) {
        testRenderTree(otp.parsedPart());
    } else {
        compareFile(outFileName, asyncFileName);
        loop.exec();

        MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
        otp.setAllowAsync(bAsync);

        fileWriter.begin(QString());
        fileWriter.queue(cssHelper.htmlHead(false));

        otp.parseObjectTree(msg.data());

        fileWriter.queue(QStringLiteral("</body></html>"));
        fileWriter.flush();
        fileWriter.end();

        testRenderTree(otp.parsedPart());
    }
    compareFile(outFileName, referenceFileName);
}

void RenderTest::compareFile(const QString& outFile, const QString& referenceFile)
{
    QVERIFY(QFile::exists(outFile));

    const QString htmlFile = outFile + QStringLiteral(".html");
    // remove tailing newlines and spaces and make htmlmore uniform (breaks pre tags)
    {
        QFile f(outFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegExp(QStringLiteral("[\t ]+")), QStringLiteral(" "));
        content.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*")), QStringLiteral("\n"));
        content.replace(QRegExp(QStringLiteral("([\n\t ])\\1+")), QStringLiteral("\\1"));
        content.replace(QRegExp(QStringLiteral(">\n+[\t ]*")), QStringLiteral(">"));
        content.replace(QRegExp(QStringLiteral("[\t ]*\n+[\t ]*<")), QStringLiteral("<"));
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    // validate xml and pretty-print for comparisson
    // TODO add proper cmake check for xmllint and diff
    QStringList args = QStringList()
                       << QStringLiteral("--format")
                       << QStringLiteral("--encode")
                       << QStringLiteral("UTF8")
                       << QStringLiteral("--output")
                       << htmlFile
                       << outFile;
    QCOMPARE(QProcess::execute(QStringLiteral("xmllint"), args),  0);

    // get rid of system dependent or random paths
    {
        QFile f(htmlFile);
        QVERIFY(f.open(QIODevice::ReadOnly));
        QString content = QString::fromUtf8(f.readAll());
        f.close();
        content.replace(QRegExp(QStringLiteral("\"file:[^\"]*[/(?:%2F)]([^\"/(?:%2F)]*)\"")), QStringLiteral("\"file:\\1\""));
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
        f.write(content.toUtf8());
        f.close();
    }

    // compare to reference file
    args = QStringList()
           << QStringLiteral("-u")
           << referenceFile
           << htmlFile;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}


QTEST_MAIN(RenderTest)
