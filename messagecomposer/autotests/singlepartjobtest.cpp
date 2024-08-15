/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "singlepartjobtest.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
using namespace KMime;

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/SinglepartJob>
using namespace MessageComposer;

QTEST_MAIN(SinglepartJobTest)

void SinglepartJobTest::testContent()
{
    auto composer = new Composer;
    auto cjob = new SinglepartJob(composer);
    QByteArray data("birds came flying from the underground");
    cjob->setData(data);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentDisposition(false) == nullptr); // Not created unless demanded.
    QVERIFY(result->contentType(false) == nullptr); // Not created unless demanded.
    QVERIFY(result->contentTransferEncoding(false)); // KMime gives it a default one (7bit).
    delete cjob;
    delete composer;
}

void SinglepartJobTest::testContentDisposition()
{
    auto composer = new Composer;
    auto cjob = new SinglepartJob(composer);
    QByteArray data("birds came flying from the underground");
    cjob->setData(data);
    QString filename = QStringLiteral("test_ăîşţâ.txt");
    cjob->contentDisposition()->setDisposition(Headers::CDattachment);
    cjob->contentDisposition()->setFilename(filename);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentDisposition(false));
    QCOMPARE(result->contentDisposition()->disposition(), Headers::CDattachment);
    QCOMPARE(result->contentDisposition()->filename(), filename);
    delete cjob;
    delete composer;
}

void SinglepartJobTest::testContentID()
{
    auto composer = new Composer;
    auto cjob = new SinglepartJob(composer);
    QByteArray data("birds came flying from the underground");
    QByteArray id("play@cold");
    cjob->setData(data);
    cjob->contentID()->setIdentifier(id);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->header<Headers::ContentID>());
    QCOMPARE(result->header<Headers::ContentID>()->identifier(), id);
    delete cjob;

    delete composer;
}

void SinglepartJobTest::testContentType()
{
    auto composer = new Composer;
    auto cjob = new SinglepartJob(composer);
    QByteArray data("birds came flying from the underground");
    cjob->setData(data);
    QByteArray mimeType("text/plain");
    QByteArray charset("utf-8");
    cjob->contentType()->setMimeType(mimeType);
    cjob->contentType()->setCharset(charset);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), mimeType);
    QCOMPARE(result->contentType()->charset(), charset);
    delete cjob;

    delete composer;
}

void SinglepartJobTest::testContentTransferEncoding()
{
    auto composer = new Composer;

    // 7bit if possible.
    {
        auto cjob = new SinglepartJob(composer);
        QByteArray data("and the sun will set for you...");
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(false));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CE7Bit);
        QCOMPARE(result->body(), data);
        delete cjob;
    }

    // quoted-printable if text doesn't fit in 7bit.
    {
        auto cjob = new SinglepartJob(composer);
        QByteArray data("some long text to make qupr more compact than base64 [ăîşţâ]"); // utf-8
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(false));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CEquPr);
        QCOMPARE(result->body(), data);
        delete cjob;
    }

    // base64 if it's shorter than quoted-printable
    {
        auto cjob = new SinglepartJob(composer);
        QByteArray data("[ăîşţâ]"); // utf-8
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(false));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CEbase64);
        QCOMPARE(result->body(), data);
        delete cjob;
    }

    delete composer;
}

#include "moc_singlepartjobtest.cpp"
