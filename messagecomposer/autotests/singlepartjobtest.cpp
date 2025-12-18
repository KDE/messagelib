/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "singlepartjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include <QDebug>
#include <QTest>

#include <KMime/Content>

#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/SinglepartJob>

using namespace KMime;
using namespace MessageComposer;

QTEST_MAIN(SinglepartJobTest)

void SinglepartJobTest::testContent()
{
    ComposerJob composerJob;
    auto cjob = new SinglepartJob(&composerJob);
    QByteArray data("birds came flying from the underground");
    cjob->setData(data);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QCOMPARE(result->contentDisposition(KMime::CreatePolicy::DontCreate), nullptr); // Not created unless demanded.
    QCOMPARE(result->contentType(KMime::CreatePolicy::DontCreate), nullptr); // Not created unless demanded.
    QVERIFY(result->contentTransferEncoding(KMime::CreatePolicy::DontCreate)); // KMime gives it a default one (7bit).
    delete cjob;
}

void SinglepartJobTest::testContentDisposition()
{
    ComposerJob composerJob;
    auto cjob = new SinglepartJob(&composerJob);
    QByteArray data("birds came flying from the underground");
    cjob->setData(data);
    QString filename = u"test_ăîşţâ.txt"_s;
    cjob->contentDisposition()->setDisposition(Headers::CDattachment);
    cjob->contentDisposition()->setFilename(filename);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentDisposition(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentDisposition()->disposition(), Headers::CDattachment);
    QCOMPARE(result->contentDisposition()->filename(), filename);
    delete cjob;
}

void SinglepartJobTest::testContentID()
{
    ComposerJob composerJob;
    auto cjob = new SinglepartJob(&composerJob);
    QByteArray data("birds came flying from the underground");
    QByteArray id("play@cold");
    cjob->setData(data);
    cjob->contentID()->setIdentifier(id);
    QVERIFY(cjob->exec());
    Content *result = cjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentID(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentID(KMime::CreatePolicy::DontCreate)->identifier(), id);
    delete cjob;
}

void SinglepartJobTest::testContentType()
{
    ComposerJob composerJob;
    auto cjob = new SinglepartJob(&composerJob);
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
    QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentType()->mimeType(), mimeType);
    QCOMPARE(result->contentType()->charset(), charset);
    delete cjob;
}

void SinglepartJobTest::testContentTransferEncoding()
{
    ComposerJob composerJob;

    // 7bit if possible.
    {
        auto cjob = new SinglepartJob(&composerJob);
        QByteArray data("and the sun will set for you...");
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(KMime::CreatePolicy::DontCreate));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CE7Bit);
        QCOMPARE(result->body(), data);
        delete cjob;
    }

    // quoted-printable if text doesn't fit in 7bit.
    {
        auto cjob = new SinglepartJob(&composerJob);
        QByteArray data("some long text to make qupr more compact than base64 [ăîşţâ]"); // utf-8
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(KMime::CreatePolicy::DontCreate));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CEquPr);
        QCOMPARE(result->body(), data);
        delete cjob;
    }

    // base64 if it's shorter than quoted-printable
    {
        auto cjob = new SinglepartJob(&composerJob);
        QByteArray data("[ăîşţâ]"); // utf-8
        cjob->setData(data);
        QVERIFY(cjob->exec());
        Content *result = cjob->content();
        result->assemble();
        qDebug() << result->encodedContent();
        QVERIFY(result->contentTransferEncoding(KMime::CreatePolicy::DontCreate));
        QCOMPARE(result->contentTransferEncoding()->encoding(), Headers::CEbase64);
        QCOMPARE(result->body(), data);
        delete cjob;
    }
}

#include "moc_singlepartjobtest.cpp"
