/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentcompressjobtest.h"
#include "qtest_messagecore.h"

#include "messagecore_debug.h"
#include <KZip>
#include <QBuffer>
#include <QTest>

#include <MessageCore/AttachmentCompressJob>
using namespace MessageCore;

QTEST_MAIN(AttachmentCompressJobTest)

void AttachmentCompressJobTest::testCompress()
{
    // Some data.
    QByteArray data;
    for (int i = 0; i < 100; ++i) {
        data += "This is some highly compressible text...\n";
    }
    const QString name = QStringLiteral("name");
    const QString fileName = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    AttachmentPart::Ptr origPart = AttachmentPart::Ptr(new AttachmentPart);
    origPart->setName(name);
    origPart->setFileName(fileName);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    QVERIFY(!origPart->isAutoEncoding());
    origPart->setData(data);
    QVERIFY(!origPart->isCompressed());

    // Compress the part and verify it.
    auto cjob = new AttachmentCompressJob(origPart, this);
    VERIFYEXEC(cjob);
    QCOMPARE(cjob->originalPart(), origPart);
    AttachmentPart::Ptr zipPart = cjob->compressedPart();
    // qCDebug(MESSAGECORE_LOG) << data;
    // qCDebug(MESSAGECORE_LOG) << zipPart->data();
    QVERIFY(zipPart->isAutoEncoding());
    QVERIFY(zipPart->isCompressed());
    QCOMPARE(zipPart->name(), QString(name + QString::fromLatin1(".zip")));
    QCOMPARE(zipPart->fileName(), QString(fileName + QString::fromLatin1(".zip")));
    QCOMPARE(zipPart->description(), description);
    QCOMPARE(zipPart->mimeType(), QByteArrayLiteral("application/zip"));

    // Uncompress the data and verify it.
    // (Stuff below is stolen from KMail code.)
    QByteArray zipData = zipPart->data();
    QBuffer buffer(&zipData);
    KZip zip(&buffer);
    QVERIFY(zip.open(QIODevice::ReadOnly));
    const KArchiveDirectory *dir = zip.directory();
    QCOMPARE(dir->entries().count(), 1);
    const KZipFileEntry *entry = (KZipFileEntry *)dir->entry(dir->entries().at(0));
    QCOMPARE(entry->data(), data);
    QCOMPARE(entry->name(), name);
    zip.close();
}

void AttachmentCompressJobTest::testCompressedSizeLarger()
{
    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    AttachmentPart::Ptr origPart = AttachmentPart::Ptr(new AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    QVERIFY(!origPart->isAutoEncoding());
    origPart->setData(data);
    QVERIFY(!origPart->isCompressed());

    // Compress the part and verify that it is aware of its folly.
    auto cjob = new AttachmentCompressJob(origPart, this);
    VERIFYEXEC(cjob);
    QVERIFY(cjob->isCompressedPartLarger());
}

#include "moc_attachmentcompressjobtest.cpp"
