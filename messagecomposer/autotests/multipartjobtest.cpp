/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartjobtest.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
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

#include "moc_multipartjobtest.cpp"
