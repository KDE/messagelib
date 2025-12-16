/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "qtest_messagecomposer.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
#include <KMime/Headers>

#include <MessageComposer/AttachmentJob>
#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>

#include <MessageCore/AttachmentPart>

using namespace KMime;
using namespace MessageComposer;
using namespace MessageCore;

#define PATH_ATTACHMENTS QLatin1StringView(KDESRCDIR "/attachments/")

QTEST_MAIN(AttachmentJobTest)

void AttachmentJobTest::testAttachment()
{
    const QString name = u"name"_s;
    const QString fileName = u"filename"_s;
    const QString description = u"long long long description..."_s;
    const QByteArray mimeType("x-some/x-type");
    const QByteArray data("la la la");

    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(name);
    part->setFileName(fileName);
    part->setDescription(description);
    part->setMimeType(mimeType);
    part->setData(data);

    ComposerJob composerJob;
    auto ajob = new AttachmentJob(part, &composerJob);
    QVERIFY(ajob->exec());
    Content *result = ajob->content();
    result->assemble();
    qDebug() << result->encodedContent();

    QCOMPARE(result->contentType(DontCreate)->name(), name);
    QCOMPARE(result->contentDisposition(DontCreate)->filename(), fileName);
    QCOMPARE(result->contentDescription(DontCreate)->asUnicodeString(), description);
    QCOMPARE(result->contentType(DontCreate)->mimeType(), mimeType);
    QCOMPARE(result->body(), data);
    QCOMPARE(result->contentDisposition(DontCreate)->disposition(), Headers::CDattachment);
    delete ajob;
    ajob = nullptr;
    // delete result;
}

#if 0
// Disabled: using UTF-8 instead of trying to detect charset.

void AttachmentJobTest::testTextCharsetAutodetect_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QByteArray>("charset");

    // PATH_ATTACHMENTS is defined by CMake.
    QTest::newRow("ascii") << QUrl::fromLocalFile(PATH_ATTACHMENTS + QString::fromLatin1("ascii.txt"))
                           << QByteArray("us-ascii");
    QTest::newRow("iso8859-2") << QUrl::fromLocalFile(PATH_ATTACHMENTS + QString::fromLatin1("iso8859-2.txt"))
                               << QByteArray("iso-8859-2");
    // TODO not sure how to test utf-16.
}

void AttachmentJobTest::testTextCharsetAutodetect()
{
    QFETCH(QUrl, url);
    QFETCH(QByteArray, charset);

    AttachmentFromUrlJob *ljob = new AttachmentFromUrlJob(url);
    VERIFYEXEC(ljob);
    AttachmentPart::Ptr part = ljob->attachmentPart();
    delete ljob;
    ljob = 0;

    Composer *composer = new Composer;
    composerJob->globalPart()->setFallbackCharsetEnabled(true);
    AttachmentJob *ajob = new AttachmentJob(part, composerJob);
    VERIFYEXEC(ajob);
    Content *result = ajob->content();
    delete ajob;
    ajob = 0;
    result->assemble();
    qDebug() << result->encodedContent();

    QCOMPARE(result->contentType(KMime::CreatePolicy::DontCreate)->charset(), charset);
}

#endif

#include "moc_attachmentjobtest.cpp"
