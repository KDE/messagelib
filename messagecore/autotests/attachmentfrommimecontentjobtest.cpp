/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentfrommimecontentjobtest.h"
#include "qtest_messagecore.h"

#include "messagecore_debug.h"
#include <QTest>

#include <KMime/Content>
#include <KMime/Headers>
using namespace KMime;

#include "attachment/attachmentfrommimecontentjob.h"
using namespace MessageCore;

QTEST_MAIN(AttachmentFromMimeContentJobTest)

void AttachmentFromMimeContentJobTest::testAttachment()
{
    const QByteArray mimeType("x-some/x-type");
    const QString name = QStringLiteral("name abcd");
    const QString description = QStringLiteral("description");
    const QByteArray charset("utf-8");
    const QString fileName = QStringLiteral("filename abcd");
    const Headers::contentEncoding encoding = Headers::CEquPr;
    const Headers::contentDisposition disposition = Headers::CDinline;
    const QByteArray data("ocean soul");

    auto content = new Content;
    content->contentType()->setMimeType(mimeType);
    content->contentType()->setName(name);
    content->contentType()->setCharset(charset);
    content->contentTransferEncoding()->setEncoding(encoding);
    content->contentDisposition()->setDisposition(disposition);
    content->contentDisposition()->setFilename(fileName);
    content->contentDescription()->fromUnicodeString(description);
    content->setBody(data);
    content->assemble();
    // qCDebug(MESSAGECORE_LOG) << "Encoded content:" << content->encodedContent();
    // qCDebug(MESSAGECORE_LOG) << "Decoded content:" << content->decodedContent();

    auto job = new AttachmentFromMimeContentJob(content, this);
    QVERIFY(job->uiDelegate() == nullptr); // No GUI thankyouverymuch.
    VERIFYEXEC(job);
    delete content;
    content = nullptr;
    AttachmentPart::Ptr part = job->attachmentPart();
    delete job;
    job = nullptr;

    QCOMPARE(part->mimeType(), mimeType);
    QCOMPARE(part->name(), name);
    QCOMPARE(part->description(), description);
    // QCOMPARE( part->charset(), charset ); // TODO will probably need charsets in AttachmentPart :(
    QCOMPARE(part->fileName(), fileName);
    QVERIFY(part->encoding() == encoding);
    QVERIFY(part->isInline());
    QCOMPARE(part->data(), data);
}

#include "moc_attachmentfrommimecontentjobtest.cpp"
