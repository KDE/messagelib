/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentfromurljobtest.h"
#include "qtest_messagecore.h"

#include <QTest>

#include "attachment/attachmentfromurljob.h"
#include <MessageCore/StringUtil>
using namespace MessageCore;

QTEST_MAIN(AttachmentFromUrlJobTest)

#define PATH_ATTACHMENTS QLatin1StringView(KDESRCDIR "/attachments/")

void AttachmentFromUrlJobTest::testAttachments_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("mimetype");

    // PATH_ATTACHMENTS is defined by CMake.
    QTest::newRow("png image") << QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("image.png")) << QStringLiteral("image.png")
                               << QByteArray("image/png");
    QTest::newRow("pdf doc") << QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("doc.pdf")) << QStringLiteral("doc.pdf")
                             << QByteArray("application/pdf");
    QTest::newRow("text file") << QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt")) << QStringLiteral("file.txt")
                               << QByteArray("text/plain");
}

void AttachmentFromUrlJobTest::testAttachments()
{
    QFETCH(QUrl, url);
    QFETCH(QString, filename);
    QFETCH(QByteArray, mimetype);

    QFile file(url.path());
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    file.close();

    auto ljob = new AttachmentFromUrlJob(url, this);
    VERIFYEXEC(ljob);
    AttachmentPart::Ptr part = ljob->attachmentPart();
    delete ljob;
    ljob = nullptr;

    QCOMPARE(part->name(), filename);
    QCOMPARE(part->fileName(), filename);
    QVERIFY(!part->isInline());
    QCOMPARE(part->mimeType(), mimetype);
    QCOMPARE(part->data(), data);
}

void AttachmentFromUrlJobTest::testAttachmentTooBig()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("doc.pdf"));

    auto ljob = new AttachmentFromUrlJob(url, this);
    ljob->setMaximumAllowedSize(1024); // 1KiB, whereas the file is >9KiB.
    QVERIFY(!ljob->exec());
}

void AttachmentFromUrlJobTest::testAttachmentCharset()
{
    const QByteArray charset("iso-8859-2");
    const QString filename = QStringLiteral("file.txt");
    QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + filename);
    MessageCore::StringUtil::setEncodingFile(url, QString::fromLatin1(charset));

    auto ljob = new AttachmentFromUrlJob(url, this);
    VERIFYEXEC(ljob);
    AttachmentPart::Ptr part = ljob->attachmentPart();
    delete ljob;
    ljob = nullptr;

    QCOMPARE(part->name(), filename);
    QCOMPARE(part->fileName(), filename);
    QCOMPARE(part->charset(), charset);
}

#include "moc_attachmentfromurljobtest.cpp"
