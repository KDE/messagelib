/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "rendertest.h"
#include "setupenv.h"
#include "testcsshelper.h"
#include "util.h"

#include <MessageViewer/FileHtmlWriter>
#include <MimeTreeParser/ObjectTreeParser>

#include <KMime/Message>

#include <QDir>
#include <QProcess>
#include <QTest>

using namespace MessageViewer;

void RenderTest::initMain()
{
    Test::setupEnv();
#ifndef Q_OS_WIN
    qputenv("LC_ALL", "en_US.UTF-8");
#endif
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))
                                                   << QString(file + QStringLiteral(".out")) << QStringLiteral("smart") << false << QString();
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".inProgress.html"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html"))
                                                   << QString(file + QStringLiteral(".out")) << QStringLiteral("smart") << false
                                                   << QString(dir.path() + QLatin1Char('/') + file + QStringLiteral(".inProgress.html"));
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/details/") + file + QLatin1StringView(".html");
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/inlined/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QLatin1Char('/') + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("inlined") << false
                                                   << QString();
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/iconic/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QLatin1Char('/') + file + QLatin1StringView(".html");
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/hidden/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QLatin1Char('/') + file + QLatin1StringView(".html");
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
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/headeronly/") + file + QStringLiteral(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + QLatin1Char('/') + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + QStringLiteral(".out")) << QStringLiteral("headeronly") << false
                                                   << QString();
    }
}

void RenderTest::testRenderHeaderOnly()
{
    testRender();
}

QString renderTreeHelper(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    const QString line = QStringLiteral("%1 * %3\n").arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    QString ret = line;

    indent += QLatin1Char(' ');
    const auto subParts = messagePart->subParts();
    for (const auto &subPart : subParts) {
        ret += renderTreeHelper(subPart, indent);
    }
    return ret;
}

void RenderTest::testRenderTree(const MimeTreeParser::MessagePart::Ptr &messagePart)
{
    QString renderedTree = renderTreeHelper(messagePart, QString());

    QFETCH(QString, mailFileName);
    QFETCH(QString, outFileName);
    const QString treeFileName = QLatin1StringView(MAIL_DATA_DIR) + QLatin1Char('/') + mailFileName + QStringLiteral(".tree");
    const QString outTreeFileName = outFileName + QStringLiteral(".tree");

    {
        QFile f(outTreeFileName);
        f.open(QIODevice::WriteOnly);
        f.write(renderedTree.toUtf8());
        f.close();
    }
    // compare to reference file
    const QStringList args = QStringList() << QStringLiteral("-u") << treeFileName << outTreeFileName;
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

    // const QString htmlFileName = outFileName + QStringLiteral(".html");
    const bool bAsync = !asyncFileName.isEmpty();

    // load input mail
    KMime::Message::Ptr msg(Test::readAndParseMail(mailFileName));

    // render the mail
    FileHtmlWriter fileWriter(outFileName);
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

    otp.parseObjectTree(msg.data());

    fileWriter.begin();
    CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
    fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
    testSource.render(otp.parsedPart(), false);
    fileWriter.write(QStringLiteral("</body></html>"));
    fileWriter.end();

    if (!bAsync) {
        testRenderTree(otp.parsedPart());
    } else {
        Test::compareFile(outFileName, asyncFileName);
        loop.exec();

        MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
        otp.setAllowAsync(bAsync);

        otp.parseObjectTree(msg.data());

        fileWriter.begin();
        CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
        fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
        testSource.render(otp.parsedPart(), false);
        fileWriter.write(QStringLiteral("</body></html>"));
        fileWriter.end();

        testRenderTree(otp.parsedPart());
    }
    Test::compareFile(outFileName, referenceFileName);
    msg.clear();
}

QTEST_MAIN(RenderTest)

#include "moc_rendertest.cpp"
