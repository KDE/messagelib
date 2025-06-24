/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "rendertest.h"
using namespace Qt::Literals::StringLiterals;

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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".html"_s)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + u'/' + file + u".html"_s) << QString(file + u".out"_s) << u"smart"_s << false
                                                   << QString();
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + u'/' + file + u".inProgress.html"_s)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << QString(dir.path() + u'/' + file + u".html"_s) << QString(file + u".out"_s) << u"smart"_s << false
                                                   << QString(dir.path() + u'/' + file + u".inProgress.html"_s);
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/details/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"smart"_s << true << QString();
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/inlined/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + u'/' + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"inlined"_s << false << QString();
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/iconic/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + u'/' + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"iconic"_s << false << QString();
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/hidden/") + file + QLatin1StringView(".html");
        if (!QFile::exists(fname)) {
            fname = dir.path() + u'/' + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"hidden"_s << false << QString();
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
    const auto l = dir.entryList(QStringList(u"*.mbox"_s), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QString fname = dir.path() + QLatin1StringView("/headeronly/") + file + u".html"_s;
        if (!QFile::exists(fname)) {
            fname = dir.path() + u'/' + file + QLatin1StringView(".html");
            if (!QFile::exists(fname)) {
                continue;
            }
        }
        QTest::newRow(file.toLatin1().constData()) << file << fname << QString(file + u".out"_s) << u"headeronly"_s << false << QString();
    }
}

void RenderTest::testRenderHeaderOnly()
{
    testRender();
}

static QString renderTreeHelper(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    const QString line = u"%1 * %3\n"_s.arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    QString ret = line;

    indent += u' ';
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
    const QString treeFileName = QLatin1StringView(MAIL_DATA_DIR) + u'/' + mailFileName + u".tree"_s;
    const QString outTreeFileName = outFileName + u".tree"_s;

    {
        QFile f(outTreeFileName);
        f.open(QIODevice::WriteOnly);
        f.write(renderedTree.toUtf8());
        f.close();
    }
    // compare to reference file
    const QStringList args = QStringList() << u"-u"_s << treeFileName << outTreeFileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(u"diff"_s, args);
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

    // const QString htmlFileName = outFileName + u".html"_s;
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
    fileWriter.write(u"</body></html>"_s);
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
        fileWriter.write(u"</body></html>"_s);
        fileWriter.end();

        testRenderTree(otp.parsedPart());
    }
    Test::compareFile(outFileName, referenceFileName);
    msg.clear();
}

QTEST_MAIN(RenderTest)

#include "moc_rendertest.cpp"
