/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartjobtest.h"

#include <QDebug>
#include <QTest>

#include <kmime/kmime_content.h>
using namespace KMime;

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MultipartJob>
#include <MessageComposer/SinglepartJob>
using namespace MessageComposer;

QTEST_MAIN(MultipartJobTest)

void MultipartJobTest::testMultipartMixed()
{
    Composer composer;
    auto mjob = new MultipartJob(&composer);
    mjob->setMultipartSubtype("mixed");

    QByteArray data1("one");
    QByteArray data2("two");
    QByteArray type1("text/plain");
    QByteArray type2("application/x-mors-ontologica");

    {
        auto cjob = new SinglepartJob(mjob);
        cjob->setData(data1);
        cjob->contentType()->setMimeType(type1);
    }

    {
        auto cjob = new SinglepartJob(mjob);
        cjob->setData(data2);
        cjob->contentType()->setMimeType(type2);
    }

    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType(false)->mimeType(), QByteArray("multipart/mixed"));
    QCOMPARE(result->contents().count(), 2);

    {
        Content *c = result->contents().at(0);
        QCOMPARE(c->body(), data1);
        QVERIFY(c->contentType(false));
        QCOMPARE(c->contentType(false)->mimeType(), type1);
    }

    {
        Content *c = result->contents().at(1);
        QCOMPARE(c->body(), data2);
        QVERIFY(c->contentType(false));
        QCOMPARE(c->contentType(false)->mimeType(), type2);
    }
    delete result;
}

void MultipartJobTest::test8BitPropagation()
{
    // If a subpart is 8bit, its parent must be 8bit too.

    Composer composer;
    composer.globalPart()->set8BitAllowed(true);
    auto mjob = new MultipartJob(&composer);
    mjob->setMultipartSubtype("mixed");
    auto mjob2 = new MultipartJob(mjob);
    mjob2->setMultipartSubtype("mixed");
    auto cjob = new SinglepartJob(mjob2);
    QByteArray data("time is so short and I'm sure there must be something more");
    cjob->setData(data);
    cjob->contentTransferEncoding()->setEncoding(Headers::CE8Bit);
    QVERIFY(mjob->exec());
    Content *content = mjob->content();
    content->assemble();
    qDebug() << content->encodedContent();
    QVERIFY(content->contentTransferEncoding(false));
    QCOMPARE(content->contentTransferEncoding(false)->encoding(), Headers::CE8Bit);
    delete content;
}
