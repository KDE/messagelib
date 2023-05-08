/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentjobtest.h"
#include "qtest_messagecomposer.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
#include <KMime/Headers>
using namespace KMime;

#include <MessageComposer/AttachmentJob>
#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
using namespace MessageComposer;

// #include "messagecore/attachmentfromfolderjob.h"
#include <MessageCore/AttachmentPart>
using namespace MessageCore;

#define PATH_ATTACHMENTS QLatin1String(KDESRCDIR "/attachments/")

QTEST_MAIN(AttachmentJobTest)

void AttachmentJobTest::testAttachment()
{
    const QString name = QStringLiteral("name");
    const QString fileName = QStringLiteral("filename");
    const QString description = QStringLiteral("long long long description...");
    const QByteArray mimeType("x-some/x-type");
    const QByteArray data("la la la");
    const QMap<QString, QString> params{{QStringLiteral("method"), QStringLiteral("request")}};

    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    part->setName(name);
    part->setFileName(fileName);
    part->setDescription(description);
    part->setMimeType(mimeType);
    part->setData(data);
    part->setContentTypeParams(params);

    Composer composer;
    composer.globalPart()->setFallbackCharsetEnabled(true);
    auto ajob = new AttachmentJob(part, &composer);
    QVERIFY(ajob->exec());
    Content *result = ajob->content();
    result->assemble();
    qDebug() << result->encodedContent();

    QCOMPARE(result->contentType(false)->name(), name);
    QCOMPARE(result->contentDisposition(false)->filename(), fileName);
    QCOMPARE(result->contentDescription(false)->asUnicodeString(), description);
    QCOMPARE(result->contentType(false)->mimeType(), mimeType);
    for (auto it = params.cbegin(), end = params.cend(); it != end; ++it) {
        QVERIFY(result->contentType(false)->hasParameter(it.key()));
        QCOMPARE(result->contentType(false)->parameter(it.key()), it.value());
    }
    QCOMPARE(result->body(), data);
    QVERIFY(result->contentDisposition(false)->disposition() == Headers::CDattachment);
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
    composer->globalPart()->setFallbackCharsetEnabled(true);
    AttachmentJob *ajob = new AttachmentJob(part, composer);
    VERIFYEXEC(ajob);
    Content *result = ajob->content();
    delete ajob;
    ajob = 0;
    result->assemble();
    qDebug() << result->encodedContent();

    QCOMPARE(result->contentType(false)->charset(), charset);
}

#endif
