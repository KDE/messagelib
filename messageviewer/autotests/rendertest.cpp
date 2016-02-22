/*
  Copyright (c) 2010 Volker Krause <vkrause@kde.org>

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
#include "util.h"
#include "testcsshelper.h"
#include "setupenv.h"

#include "htmlwriter/filehtmlwriter.h"
#include "viewer/objecttreeparser.h"

#include <KMime/Message>

#include <QDir>
#include <QObject>
#include <QProcess>
#include <QTest>

using namespace MessageViewer;

void RenderTest::initTestCase()
{
    MessageViewer::Test::setupEnv();
}

void RenderTest::testRenderSmart_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1()) << file << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html")) << QString(file + QStringLiteral(".out")) << QStringLiteral("smart");
    }
}

void RenderTest::testRenderSmart()
{
    testRender();
}

void RenderTest::testRenderInlined_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");
    QTest::addColumn<QString>("outFileName");
    QTest::addColumn<QString>("attachmentStrategy");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/inlined/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("inlined");
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

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/iconic/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("iconic");
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

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/hidden/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("hidden");
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

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    foreach (const QString &file, dir.entryList(QStringList(QLatin1String("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks)) {
        QString fname = dir.path() + QStringLiteral("/headeronly/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QStringLiteral("/") + file + QStringLiteral(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("headeronly");
    }
}

void RenderTest::testRenderHeaderOnly()
{
    testRender();
}

void RenderTest::testRender()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);
    QFETCH(QString, outFileName);
    QFETCH(QString, attachmentStrategy);

    const QString htmlFileName = outFileName + QStringLiteral(".html");

    // load input mail
    const KMime::Message::Ptr msg(readAndParseMail(mailFileName));

    // render the mail
    FileHtmlWriter fileWriter(outFileName);
    QImage paintDevice;
    MessageViewer::TestCSSHelper cssHelper(&paintDevice);
    NodeHelper nodeHelper;
    MessageViewer::Test::TestObjectTreeSource testSource(&fileWriter, &cssHelper);
    testSource.setAllowDecryption(true);
    testSource.setAttachmentStrategy(attachmentStrategy);
    ObjectTreeParser otp(&testSource, &nodeHelper);

    fileWriter.begin(QString());
    fileWriter.queue(cssHelper.htmlHead(false));

    otp.parseObjectTree(msg.data());

    fileWriter.queue(QStringLiteral("</body></html>"));
    fileWriter.flush();
    fileWriter.end();

    QVERIFY(QFile::exists(outFileName));

    // validate xml and pretty-print for comparisson
    // TODO add proper cmake check for xmllint and diff
    QStringList args = QStringList()
                       << QStringLiteral("--format")
                       << QStringLiteral("--encode")
                       << QStringLiteral("UTF8")
                       << QStringLiteral("--output")
                       << htmlFileName
                       << outFileName;
    QCOMPARE(QProcess::execute(QStringLiteral("xmllint"), args),  0);

    // get rid of system dependent or random paths
    {
        QFile f(htmlFileName);
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
           << referenceFileName
           << htmlFileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}

QTEST_MAIN(RenderTest)