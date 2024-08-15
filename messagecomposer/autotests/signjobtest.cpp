/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signjobtest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include <QTest>

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/SignJob>
#include <MessageComposer/TransparentJob>

#include "setupenv.h"

#include <cstdlib>

QTEST_MAIN(SignJobTest)

using namespace MessageComposer;

void SignJobTest::initTestCase()
{
    Test::setupEnv();
}

void SignJobTest::testContentDirect()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    checkSignJob(sJob);
}

void SignJobTest::testContentChained()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    Composer composer;
    auto sJob = new SignJob(&composer);

    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    sJob->appendSubjob(tJob);

    checkSignJob(sJob);
}

void SignJobTest::testHeaders()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    result->assemble();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), "multipart/signed");
    QCOMPARE(result->contentType()->charset(), "UTF-8");
    QVERIFY(result->contentType()->parameter("micalg").startsWith(QLatin1StringView("pgp-sha"))); // sha1 or sha256, depending on GnuPG version
    QCOMPARE(result->contentType()->parameter("protocol"), QString::fromLocal8Bit("application/pgp-signature"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}

void SignJobTest::testRecommentationRFC3156()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QString data = QStringLiteral("=2D Magic foo\nFrom test\n\n-- quaak\nOhno");
    KMime::Headers::contentEncoding cte = KMime::Headers::CEquPr;

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    auto content = new KMime::Content;
    content->setBody(data.toUtf8());

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    result->assemble();

    QVERIFY(!result->contents().isEmpty());
    const QByteArray body = result->contents().at(0)->body();
    QCOMPARE(QString::fromUtf8(body), QStringLiteral("=3D2D Magic foo\n=46rom test\n\n=2D- quaak\nOhno"));

    ComposerTestUtil::verify(true, false, result, data.toUtf8(), Kleo::OpenPGPMIMEFormat, cte);
    delete result;
}

void SignJobTest::testMixedContent()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QString data = QStringLiteral("=2D Magic foo\nFrom test\n\n-- quaak\nOhno");

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    auto content = new KMime::Content;
    content->contentType()->setMimeType(QByteArrayLiteral("multipart/mixed"));
    content->contentType()->setBoundary(KMime::multiPartBoundary());
    auto subcontent = new KMime::Content;
    subcontent->contentType()->setMimeType(QByteArrayLiteral("text/plain"));
    subcontent->setBody(data.toUtf8());
    auto attachment = new KMime::Content;
    attachment->contentType()->setMimeType(QByteArrayLiteral("text/plain"));
    const QByteArray attachmentData("an attachment");
    attachment->setBody(attachmentData);

    content->appendContent(subcontent);
    content->appendContent(attachment);
    content->assemble();

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    result->assemble();

    QCOMPARE(result->contents().count(), 2);
    KMime::Content *firstChild = result->contents().at(0);
    QCOMPARE(firstChild->contents().count(), 2);
    QCOMPARE(firstChild->body(), QByteArray());
    QCOMPARE(firstChild->contentType()->mimeType(), QByteArrayLiteral("multipart/mixed"));
    QCOMPARE(firstChild->contents()[0]->body(), data.toUtf8());
    QCOMPARE(firstChild->contents()[1]->body(), attachmentData);

    ComposerTestUtil::verify(true, false, result, data.toUtf8(), Kleo::OpenPGPMIMEFormat, KMime::Headers::CE7Bit);
    delete result;
}

void SignJobTest::checkSignJob(SignJob *sJob)
{
    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    Q_ASSERT(result);
    result->assemble();

    ComposerTestUtil::verifySignature(result,
                                      QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8(),
                                      Kleo::OpenPGPMIMEFormat,
                                      KMime::Headers::CE7Bit);

    delete result;
}

void SignJobTest::testProtectedHeaders_data()
{
    QTest::addColumn<bool>("protectedHeaders");
    QTest::addColumn<QString>("referenceFile");

    QTest::newRow("simple-non-obvoscate") << true << QStringLiteral("protected_headers-non-obvoscate.mbox");
    QTest::newRow("non-protected_headers") << false << QStringLiteral("non-protected_headers.mbox");
}

void SignJobTest::testProtectedHeaders()
{
    QFETCH(bool, protectedHeaders);
    QFETCH(QString, referenceFile);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    const QString subject(QStringLiteral("asdfghjklö"));

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.subject(true)->fromUnicodeString(subject);

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);
    sJob->setSkeletonMessage(&skeletonMessage);
    sJob->setProtectedHeaders(protectedHeaders);

    VERIFYEXEC(sJob);

    QCOMPARE(skeletonMessage.subject()->asUnicodeString(), subject);

    KMime::Content *result = sJob->content();
    result->assemble();

    Test::compareFile(result->contents().at(0), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void SignJobTest::testProtectedHeadersOverwrite()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();
    const auto referenceFile = QStringLiteral("protected_headers-non-obvoscate.mbox");

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    const QString subject(QStringLiteral("asdfghjklö"));

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.subject(true)->fromUnicodeString(subject);

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);
    sJob->setSkeletonMessage(&skeletonMessage);
    sJob->setProtectedHeaders(true);

    VERIFYEXEC(sJob);

    QCOMPARE(skeletonMessage.subject()->asUnicodeString(), subject);
    skeletonMessage.to()->from7BitString("overwrite@example.org");
    skeletonMessage.cc()->from7BitString("cc_overwrite@example.org");
    skeletonMessage.bcc()->from7BitString("bcc_overwrite@example.org");
    skeletonMessage.subject()->fromUnicodeString(subject + QStringLiteral("_owerwrite"));

    KMime::Content *result = sJob->content();
    result->assemble();

    Test::compareFile(result->contents().at(0), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void SignJobTest::testProtectedHeadersSkipLong()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();
    const auto referenceFile = QStringLiteral("protected_headers-non-obvoscate.mbox");

    Composer composer;
    auto sJob = new SignJob(&composer);

    QVERIFY(sJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    const QString subject(QStringLiteral("asdfghjklö"));

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    auto face = new KMime::Headers::Generic("Face");
    face->from7BitString(
        "iVBORw0KGgoAAAANSUhEUgAAADAAAAAwBAMAAAClLOS0AAAAElBMVEX9/ftIS5IsNImXjKgc\n"
        " I3Dh29sdnA7JAAAB60lEQVR4nI2US5ajMAxFXeF4HtlhXlZ7AdCCBWCcORTx/rfSkgyEyqg14uii\n"
        " pw8SpnzaRmrm/8yWD0dZqn80G5WlUOVEnsCPNKp6D2w98JsWHcJuiOiCPnXGNPDIU/U7BuKPGe7G\n"
        " fAW/rNPhl+A4bqkC6IZDiwOwXb0CnKBtpjMJhgZoB96bXl5Gtftfr+AHJ4LlhRm8VOvmJlCvwM3U\n"
        " d41f9maX1dOADG4cQe1lDD3RAzVH+DXMhhUTfjOIkWexvAHRkCSiyZmuYBWlGpGfV0CihDyrW8yh\n"
        " bOYDaFU5d+8AU8YXqtQNEz5P92KskWFq54h/ONWs4OtpbDoiGCzmNatc05rmBE6AtQpKsdsOfoLD\n"
        " UBgci6JAqgqI8bI2F+AwjleAew6OwFYSWA8CVgFdrQqfCnIW8MKAdexscwWTbOML8Q0myV4y8218\n"
        " 7FIKomwvf5bO0ph+gZFbZCD7q2DZAc7nyQxv4FSLTcHjkLo5KS/Ox41lPHIExyEZRb4lP5zgR7c8\n"
        " BV822d6UTiAjiQnGVfyyIZfpYmZg+gpiTvHsg4EzHOIVsMC9lsvP0He2B+qHmHOO6fsAAehpqSc+\n"
        " yBNIF1wUgOXzCJDVBIB0Icfd2vKAN2jAubRfOvt3INsO+3XvdgGBgAH/OepfoIJ/RyuNKjhmbCAA\n"
        " AAAASUVORK5CYII=");
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.appendHeader(face);
    skeletonMessage.subject(true)->fromUnicodeString(subject);

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);
    sJob->setSkeletonMessage(&skeletonMessage);
    sJob->setProtectedHeaders(true);

    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    result->assemble();

    Test::compareFile(result->contents().at(0), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

#include "moc_signjobtest.cpp"
