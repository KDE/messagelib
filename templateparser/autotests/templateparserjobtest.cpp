/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparserjobtest.h"

#ifndef _WIN32
#define private public
#endif
#include "templateparserjob.h"
#include "templateparserjob_p.h"
#ifndef _WIN32
#undef protected
#endif
#include <KIdentityManagement/Identity>
#include <KIdentityManagement/IdentityManager>
#include <MimeTreeParser/ObjectTreeParser>

#include <QDir>
#include <QLocale>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

using namespace MimeTreeParser;

TemplateParserJobTest::TemplateParserJobTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

TemplateParserJobTest::~TemplateParserJobTest()
{
    // Workaround QTestLib not flushing deleteLater()s on exit, which leads to WebEngine asserts (view not deleted)
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

void TemplateParserJobTest::test_convertedHtml_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("plain*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file)
                                                   << QString(dir.path() + QLatin1Char('/') + file + QLatin1String(".html"));
    }
}

void TemplateParserJobTest::test_convertedHtml()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();
    QCOMPARE(origMsg->subject()->as7BitString(false).constData(), "Plain Message Test");
    QCOMPARE(origMsg->contents().size(), 0);

    // load expected result
    QFile referenceFile(referenceFileName);
    QVERIFY(referenceFile.open(QIODevice::ReadOnly));
    const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
    const QString referenceData = QString::fromLatin1(referenceRawData);
    QVERIFY(!referenceData.isEmpty());

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::NewMessage);
    auto identMan = new KIdentityManagement::IdentityManager;
    parser->setIdentityManager(identMan);

    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());
    QVERIFY(parser->d->mOtp->htmlContent().isEmpty());
    QVERIFY(!parser->d->mOtp->plainTextContent().isEmpty());

    const QString convertedHtmlContent = parser->htmlMessageText(false, TemplateParser::TemplateParserJob::NoSelectionAllowed);
    QVERIFY(!convertedHtmlContent.isEmpty());

    QCOMPARE(convertedHtmlContent, referenceData);
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_replyHtml_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        const QString expectedFile = dir.path() + QLatin1Char('/') + file + QStringLiteral(".html.reply");
        if (!QFile::exists(expectedFile)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << expectedFile;
    }
}

void TemplateParserJobTest::test_replyHtml()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    // load expected result
    QFile referenceFile(referenceFileName);
    QVERIFY(referenceFile.open(QIODevice::ReadOnly));
    const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
    const QString referenceData = QString::fromLatin1(referenceRawData);
    QVERIFY(!referenceData.isEmpty());

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::NewMessage);
    auto identMan = new KIdentityManagement::IdentityManager;
    parser->setIdentityManager(identMan);

    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());

    QString convertedHtmlContent = parser->htmlMessageText(false, TemplateParser::TemplateParserJob::NoSelectionAllowed);
    QVERIFY(!convertedHtmlContent.isEmpty());

    // referenceData is read from a file and most text editors add a \n at the end of the last line
    if (!convertedHtmlContent.endsWith(QLatin1Char('\n'))) {
        convertedHtmlContent += QStringLiteral("\n");
    }

    QCOMPARE(convertedHtmlContent, referenceData);
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_replyPlain_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        const QString expectedFile = dir.path() + QLatin1Char('/') + file + QStringLiteral(".plain.reply");
        if (!QFile::exists(expectedFile)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << expectedFile;
    }
}

void TemplateParserJobTest::test_replyPlain()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    // load expected result
    QFile referenceFile(referenceFileName);
    QVERIFY(referenceFile.open(QIODevice::ReadOnly));
    const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
    const QString referenceData = QString::fromLatin1(referenceRawData);
    QVERIFY(!referenceData.isEmpty());

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Reply);
    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());

    const QString convertedPlainTextContent = parser->plainMessageText(false, TemplateParser::TemplateParserJob::NoSelectionAllowed);

    QCOMPARE(convertedPlainTextContent, referenceData);
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_forwardPlain_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        const QString expectedFile = dir.path() + QLatin1Char('/') + file + QStringLiteral(".plain.reply");
        if (!QFile::exists(expectedFile)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << expectedFile;
    }
}

void TemplateParserJobTest::test_forwardPlain()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    // load expected result
    QFile referenceFile(referenceFileName);
    QVERIFY(referenceFile.open(QIODevice::ReadOnly));
    const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
    const QString referenceData = QString::fromLatin1(referenceRawData);
    QVERIFY(!referenceData.isEmpty());

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Forward);
    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());

    const QString convertedPlainTextContent = parser->plainMessageText(false, TemplateParser::TemplateParserJob::NoSelectionAllowed);

    QCOMPARE(convertedPlainTextContent, referenceData);
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_forwardHtml_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        const QString expectedFile = dir.path() + QLatin1Char('/') + file + QStringLiteral(".html.reply");
        if (!QFile::exists(expectedFile)) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << expectedFile;
    }
}

void TemplateParserJobTest::test_forwardHtml()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    // load expected result
    QFile referenceFile(referenceFileName);
    QVERIFY(referenceFile.open(QIODevice::ReadOnly));
    const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
    const QString referenceData = QString::fromLatin1(referenceRawData);
    QVERIFY(!referenceData.isEmpty());

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Forward);
    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());

    QString convertedHtmlContent = parser->htmlMessageText(false, TemplateParser::TemplateParserJob::NoSelectionAllowed);
    // referenceData is read from a file and most text editors add a \n at the end of the last line
    if (!convertedHtmlContent.endsWith(QLatin1Char('\n'))) {
        convertedHtmlContent += QStringLiteral("\n");
    }

    QCOMPARE(convertedHtmlContent, referenceData);
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_forwardedAttachments_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("referenceFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".html.reply"))) {
            continue;
        }
        QString expectedFile = dir.path() + QLatin1Char('/') + file + QStringLiteral(".forwarded.mbox");
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << expectedFile;
    }
}

void TemplateParserJobTest::test_forwardedAttachments()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);

    // load input mail
    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    bool referenceExists = QFile::exists(referenceFileName);

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Forward);
    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(QString());
    QVERIFY(spy.wait());

    if (referenceExists) {
        QFile referenceFile(referenceFileName);
        QVERIFY(referenceFile.open(QIODevice::ReadOnly));
        const QByteArray referenceRawData = KMime::CRLFtoLF(referenceFile.readAll());
        KMime::Message::Ptr referenceMsg(new KMime::Message);
        referenceMsg->setContent(referenceRawData);
        referenceMsg->parse();

        QCOMPARE(msg->contents().size(), referenceMsg->contents().size());
        for (int i = 1; i < msg->contents().size(); i++) {
            QCOMPARE(msg->contents()[i]->encodedContent(), referenceMsg->contents()[i]->encodedContent());
        }
        referenceMsg.clear();
    } else {
        QCOMPARE(msg->contents().size(), 0);
    }
    msg.clear();
    origMsg.clear();
    delete parser;
}

void TemplateParserJobTest::test_processWithTemplatesForBody_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<QString>("selection");

    QTest::newRow("%OTEXT-plain") << "%OTEXT"
                                  << "Original text.\nLine two."
                                  << "Original text.\nLine two."
                                  << "";
    QTest::newRow("%OTEXT-encrypted") << "%OTEXT"
                                      << "-----BEGIN PGP MESSAGE-----\nVersion: GnuPG v1.4.12 (GNU/Linux)\n"
                                         "\n"
                                         "hQEMAwzOQ1qnzNo7AQgA1345CrnOBTGf2eo4ABR6wkOdasI9SELRBKA1fNkFcq+Z\n"
                                         "Qg0gWB5RLapU+VFRc5hK1zPOZ1dY6j3+uPHO4RhjfUgfiZ8T7oaWav15yP+07u21\n"
                                         "EI9W9sk+eQU9GZSOayURucmZa/mbBz9hrsmePpORxD+C3uNTYa6ePTFlQP6wEZOI\n"
                                         "7E53DrtJnF0EzIsCBIVep6CyuYfuSSwQ5gMgyPzfBqiGHNw96w2i/eayErc6lquL\n"
                                         "JPFhIcMMq8w9Yo9+vXCAbkns6dtBAzlnAzuV86VFUZ/MnHTlCNk2yHyGLP6BS6hG\n"
                                         "kFEUmgdHrGRizdz1sjo1tSmOLu+Gyjlv1Ir/Sqr8etJQAeTq3heKslAfhtotAMMt\n"
                                         "R3tk228Su13Q3CAP/rktAyuGMDFtH8klW09zFdsZBDu8svE6d9e2nZ541NGspFVI\n"
                                         "6XTZHUMMdlgnTBcu3aPc0ow=\n"
                                         "=0xtc\n"
                                         "-----END PGP MESSAGE-----"
                                      << "Crypted line.\nCrypted line two."
                                      << "";
    QTest::newRow("%QUOTE") << "%QUOTE"
                            << "Quoted text.\nLine two."
                            << "> Quoted text.\n> Line two."
                            << "";
}

void TemplateParserJobTest::test_processWithTemplatesForBody()
{
    QFETCH(QString, command);
    QFETCH(QString, text);
    QFETCH(QString, expected);
    QFETCH(QString, selection);

    KMime::Message::Ptr msg(new KMime::Message());
    KMime::Message::Ptr origMsg(new KMime::Message());
    origMsg->setBody(text.toLocal8Bit());
    origMsg->parse();
    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Reply);
    parser->setSelection(selection);
    auto identMan = new KIdentityManagement::IdentityManager;
    parser->setIdentityManager(identMan);
    parser->setAllowDecryption(true);
    parser->d->mOrigMsg = origMsg;

    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(command);
    QVERIFY(spy.wait());

    identMan->deleteLater();
    QCOMPARE(QString::fromLatin1(msg->encodedBody()), expected);
    QCOMPARE(spy.count(), 1);
    delete parser;
}

void TemplateParserJobTest::test_processWithTemplatesForContent_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("expectedBody");
    QTest::addColumn<bool>("hasDictionary");
    qputenv("TZ", "Europe/Paris");
    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const QString file = QStringLiteral("plain-message.mbox");
    const QString fileName = QString(dir.path() + QLatin1Char('/') + file);
    QTest::newRow("%OTIME") << "%OTIME" << fileName << QLocale().toString(QTime(8, 0, 27), QLocale::ShortFormat) << false;
    QTest::newRow("%OTIMELONG") << "%OTIMELONG" << fileName << QLocale().toString(QTime(8, 0, 27), QLocale::LongFormat) << false;
    QTest::newRow("%OTIMELONGEN") << "%OTIMELONGEN" << fileName << QLocale(QLocale::C).toString(QTime(8, 0, 27), QLocale::LongFormat) << false;
    QTest::newRow("%ODATE") << "%ODATE" << fileName << QLocale().toString(QDate(2011, 8, 7), QLocale::LongFormat) << false;
    QTest::newRow("%ODATESHORT") << "%ODATESHORT" << fileName << QLocale().toString(QDate(2011, 8, 7), QLocale::ShortFormat) << false;
    QTest::newRow("%ODATEEN") << "%ODATEEN" << fileName << QLocale::c().toString(QDate(2011, 8, 7), QLocale::LongFormat) << false;
    QTest::newRow("%OFULLSUBJ") << "%OFULLSUBJ" << fileName << "Plain Message Test" << false;
    QTest::newRow("%OFULLSUBJECT") << "%OFULLSUBJECT" << fileName << "Plain Message Test" << false;
    QTest::newRow("%OFROMFNAME") << "%OFROMFNAME" << fileName << "Sudhendu" << false;
    QTest::newRow("%OFROMLNAME") << "%OFROMLNAME" << fileName << "Kumar" << false;
    QTest::newRow("%OFROMNAME") << "%OFROMNAME" << fileName << "Sudhendu Kumar" << false;
    QTest::newRow("%OFROMADDR") << "%OFROMADDR" << fileName << "Sudhendu Kumar <dontspamme@yoohoo.com>" << false;
    QTest::newRow("%OTOADDR") << "%OTOADDR" << fileName << "kde <foo@yoohoo.org>" << false;
    QTest::newRow("%OTOFNAME") << "%OTOFNAME" << fileName << "kde" << false;
    QTest::newRow("%OTONAME") << "%OTONAME" << fileName << "kde" << false;
    QTest::newRow("%OTOLNAME") << "%OTOLNAME" << fileName << "" << false;
    QTest::newRow("%OTOLIST") << "%OTOLIST" << fileName << "kde <foo@yoohoo.org>" << false;
    QTest::newRow("%ODOW") << "%ODOW" << fileName << QLocale().dayName(7, QLocale::LongFormat) << false;
    QTest::newRow("%BLANK") << "%BLANK" << fileName << "" << false;
    QTest::newRow("%NOP") << "%NOP" << fileName << "" << false;
    QTest::newRow("%DICTIONARYLANGUAGE=\"en\"") << "%DICTIONARYLANGUAGE=\"en\"" << fileName << "" << true;
    QTest::newRow("%DICTIONARYLANGUAGE=\"\"") << "%DICTIONARYLANGUAGE=\"\"" << fileName << "" << false;
    QTest::newRow("%OTIMELONG %OFULLSUBJECT") << "%OTIMELONG %OFULLSUBJECT" << fileName
                                              << QLocale().toString(QTime(8, 0, 27), QLocale::LongFormat) + QStringLiteral(" Plain Message Test") << false;
    QTest::newRow("%OTIMELONG\n%OFULLSUBJECT") << "%OTIMELONG\n%OFULLSUBJECT" << fileName
                                               << QLocale().toString(QTime(8, 0, 27), QLocale::LongFormat) + QStringLiteral("\nPlain Message Test") << false;
    QTest::newRow("%REM=\"sdfsfsdsdfsdf\"") << "%REM=\"sdfsfsdsdfsdf\"" << fileName << "" << false;
    QTest::newRow("%CLEAR") << "%CLEAR" << fileName << "" << false;
    QTest::newRow("FOO foo") << "FOO foo" << fileName << "FOO foo" << false;
    const QString insertFileName = QString(dir.path() + QLatin1Char('/') + QLatin1String("insert-file.txt"));
    QString insertFileNameCommand = QStringLiteral("%INSERT=\"%1\"").arg(insertFileName);
    QTest::newRow("%INSERT") << insertFileNameCommand << fileName << "test insert file!\n" << false;
    insertFileNameCommand = QStringLiteral("%PUT=\"%1\"").arg(insertFileName);
    QTest::newRow("%PUT") << insertFileNameCommand << fileName << "test insert file!\n" << false;
    QTest::newRow("%OMSGID") << "%OMSGID" << fileName << "<20150@foo.kde.org>" << false;
    QTest::newRow("%SYSTEM") << "%SYSTEM=\"echo foo\"" << fileName << "foo\n" << false;
    QTest::newRow("%DEBUG") << "%DEBUG" << fileName << "" << false;
    QTest::newRow("%DEBUGOFF") << "%DEBUGOFF" << fileName << "" << false;
    QTest::newRow("%HEADER=\"Reply-To\"") << "%HEADER=\"Reply-To\"" << fileName << "bla@yoohoo.org" << false;
    // Header doesn't exist => don't add value
    QTest::newRow("%OHEADER=\"SSS\"") << "%HEADER=\"SSS\"" << fileName << "" << false;
    QTest::newRow("%OHEADER=\"To\"") << "%OHEADER=\"To\"" << fileName << "kde <foo@yoohoo.org>" << false;
    QTest::newRow("%HEADER( To )") << "%HEADER( To )" << fileName << "kde <foo@yoohoo.org>" << false;
    QTest::newRow("%HEADER( To ) second test ") << "foo %HEADER( To )" << fileName << "foo kde <foo@yoohoo.org>" << false;
    // Unknown command
    QTest::newRow("unknown command") << "%GGGGG" << fileName << "%GGGGG" << false;

    // Test bug 308444
    const QString file2 = QStringLiteral("plain-message-timezone.mbox");
    const QString fileName2 = QString(dir.path() + QLatin1Char('/') + file2);
    QTest::newRow("bug308444-%OTIMELONG") << "%OTIMELONG" << fileName2 << QLocale::system().toString(QTime(20, 31, 25), QLocale::LongFormat) << false;
}

void TemplateParserJobTest::test_processWithTemplatesForContent()
{
    QFETCH(QString, command);
    QFETCH(QString, mailFileName);
    QFETCH(QString, expectedBody);
    QFETCH(bool, hasDictionary);

    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Reply);
    auto identMan = new KIdentityManagement::IdentityManager;
    parser->setIdentityManager(identMan);
    parser->setAllowDecryption(false);
    parser->d->mOrigMsg = origMsg;
    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(command);
    QVERIFY(spy.wait());
    QCOMPARE(msg->hasHeader("X-KMail-Dictionary"), hasDictionary);

    identMan->deleteLater();
    QCOMPARE(QString::fromUtf8(msg->encodedBody()), expectedBody);
    QCOMPARE(spy.count(), 1);
}

void TemplateParserJobTest::test_processWithTemplatesForContentOtherTimeZone_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("expectedBody");
    QTest::addColumn<bool>("hasDictionary");
    qputenv("TZ", "America/New_York");
    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    // Test bug 308444
    const QString file2 = QStringLiteral("plain-message-timezone.mbox");
    const QString fileName2 = QString(dir.path() + QLatin1Char('/') + file2);
    QTest::newRow("bug308444-%OTIMELONG") << "%OTIMELONG" << fileName2 << QLocale::system().toString(QTime(14, 31, 25), QLocale::LongFormat) << false;
}

void TemplateParserJobTest::test_processWithTemplatesForContentOtherTimeZone()
{
    QFETCH(QString, command);
    QFETCH(QString, mailFileName);
    QFETCH(QString, expectedBody);
    QFETCH(bool, hasDictionary);

    QFile mailFile(mailFileName);
    QVERIFY(mailFile.open(QIODevice::ReadOnly));
    const QByteArray mailData = KMime::CRLFtoLF(mailFile.readAll());
    QVERIFY(!mailData.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Message::Ptr origMsg(new KMime::Message);
    origMsg->setContent(mailData);
    origMsg->parse();

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Reply);
    auto identMan = new KIdentityManagement::IdentityManager;
    parser->setIdentityManager(identMan);
    parser->setAllowDecryption(false);
    parser->d->mOrigMsg = origMsg;
    QSignalSpy spy(parser, &TemplateParser::TemplateParserJob::parsingDone);
    parser->processWithTemplate(command);
    QVERIFY(spy.wait());
    QCOMPARE(msg->hasHeader("X-KMail-Dictionary"), hasDictionary);

    identMan->deleteLater();
    QCOMPARE(QString::fromUtf8(msg->encodedBody()), expectedBody);
    QCOMPARE(spy.count(), 1);
}

void TemplateParserJobTest::test_makeValidHtml_data()
{
    QTest::addColumn<QString>("message");
    QTest::addColumn<QString>("expected");

    QTest::newRow("plain text") << QStringLiteral("Some text\n-- \nSignature")
                                << QStringLiteral(
                                       "<html><head></head><body>Some text\n"
                                       "-- \nSignature<br/></body></html>");

    QTest::newRow("existing HTML tag") << QStringLiteral("<html><body>Some text\n-- \nSignature</body></html>")
                                       << QStringLiteral("<html><body>Some text\n-- \nSignature</body></html>");

    QTest::newRow("existing body tag, no html") << QStringLiteral("<body>Some text\n-- \nSignature</body>")
                                                << QStringLiteral(
                                                       "<html><head></head><body>Some text\n"
                                                       "-- \nSignature</body></html>");
}

void TemplateParserJobTest::test_makeValidHtml()
{
    QFETCH(QString, message);
    QFETCH(QString, expected);

    KMime::Message::Ptr msg(new KMime::Message);

    auto parser = new TemplateParser::TemplateParserJob(msg, TemplateParser::TemplateParserJob::Reply);
    QString result = message;
    parser->makeValidHtml(result);

    QCOMPARE(result, expected);
}

QTEST_MAIN(TemplateParserJobTest)
