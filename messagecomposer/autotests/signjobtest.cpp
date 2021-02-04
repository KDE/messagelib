/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signjobtest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include <QTest>

#include <kmime/kmime_content.h>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/SignJob>
#include <MessageComposer/TransparentJob>

#include <MessageCore/NodeHelper>
#include <setupenv.h>

#include <stdlib.h>

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
    QCOMPARE(result->contentType()->charset(), "ISO-8859-1");
    QVERIFY(
        result->contentType()->parameter(QString::fromLocal8Bit("micalg")).startsWith(QLatin1String("pgp-sha"))); // sha1 or sha256, depending on GnuPG version
    QCOMPARE(result->contentType()->parameter(QString::fromLocal8Bit("protocol")), QString::fromLocal8Bit("application/pgp-signature"));
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

    const QByteArray body = MessageCore::NodeHelper::firstChild(result)->body();
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

    content->addContent(subcontent);
    content->addContent(attachment);
    content->assemble();

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);

    VERIFYEXEC(sJob);

    KMime::Content *result = sJob->content();
    result->assemble();

    KMime::Content *firstChild = MessageCore::NodeHelper::firstChild(result);
    QCOMPARE(result->contents().count(), 2);
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
    skeletonMessage.subject(true)->fromUnicodeString(subject, "utf-8");

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
    skeletonMessage.subject(true)->fromUnicodeString(subject, "utf-8");

    sJob->setContent(content);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    sJob->setSigningKeys(keys);
    sJob->setSkeletonMessage(&skeletonMessage);
    sJob->setProtectedHeaders(true);

    VERIFYEXEC(sJob);

    QCOMPARE(skeletonMessage.subject()->asUnicodeString(), subject);
    skeletonMessage.to()->from7BitString("owerwrite@example.org");
    skeletonMessage.cc()->from7BitString("cc_overwrite@example.org");
    skeletonMessage.bcc()->from7BitString("bcc_overwrite@example.org");
    skeletonMessage.subject()->fromUnicodeString(subject + QStringLiteral("_owerwrite"), "utf-8");

    KMime::Content *result = sJob->content();
    result->assemble();

    Test::compareFile(result->contents().at(0), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}
