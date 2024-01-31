/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/
#include "attachmentupdatejobtest.h"
#include "attachment/attachmentupdatejob.h"
#include "qtest_messagecore.h"
#include <MessageCore/AttachmentPart>
#include <QTest>

#define PATH_ATTACHMENTS QLatin1StringView(KDESRCDIR "/attachments/")

AttachmentUpdateJobTest::AttachmentUpdateJobTest(QObject *parent)
    : QObject(parent)
{
}

AttachmentUpdateJobTest::~AttachmentUpdateJobTest() = default;

void AttachmentUpdateJobTest::shouldHaveDefaultValue()
{
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);
    QCOMPARE(origPart, job->originalPart());
    QVERIFY(!job->updatedPart());
    delete job;
}

void AttachmentUpdateJobTest::shouldUpdateAttachment()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    origPart->setData(data);
    origPart->setUrl(url);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QVERIFY(origPart->size() != job->updatedPart()->size());
    QVERIFY(origPart->data() != job->updatedPart()->data());
}

void AttachmentUpdateJobTest::shouldHaveSameNameDescriptionAfterUpdate()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");
    const QString filename = QStringLiteral("filename");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setFileName(filename);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    origPart->setData(data);
    origPart->setUrl(url);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QCOMPARE(origPart->name(), job->updatedPart()->name());
    QCOMPARE(origPart->description(), job->updatedPart()->description());
    QCOMPARE(origPart->fileName(), job->updatedPart()->fileName());
}

void AttachmentUpdateJobTest::shouldHaveSameCryptoSignStatusAfterUpdate()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    origPart->setData(data);
    origPart->setUrl(url);
    origPart->setSigned(true);
    origPart->setEncrypted(true);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QCOMPARE(origPart->isSigned(), job->updatedPart()->isSigned());
    QCOMPARE(origPart->isEncrypted(), job->updatedPart()->isEncrypted());
}

void AttachmentUpdateJobTest::shouldHaveSameEncodingAfterUpdate()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/pdf");
    origPart->setEncoding(KMime::Headers::CE8Bit);
    origPart->setData(data);
    origPart->setUrl(url);
    origPart->setSigned(true);
    origPart->setEncrypted(true);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QCOMPARE(origPart->encoding(), job->updatedPart()->encoding());
}

void AttachmentUpdateJobTest::shouldHaveSameMimetypeAfterUpdate()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/pdf");
    origPart->setEncoding(KMime::Headers::CE8Bit);
    origPart->setData(data);
    origPart->setUrl(url);
    origPart->setSigned(true);
    origPart->setEncrypted(true);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QCOMPARE(origPart->mimeType(), job->updatedPart()->mimeType());
}

void AttachmentUpdateJobTest::shouldNotUpdateWhenUrlIsEmpty()
{
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/plain");
    origPart->setEncoding(KMime::Headers::CE7Bit);
    origPart->setData(data);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);
    job->exec();
    QVERIFY(!job->updatedPart());
}

void AttachmentUpdateJobTest::shouldHaveSameInlineStatus()
{
    const QUrl url = QUrl::fromLocalFile(PATH_ATTACHMENTS + QLatin1StringView("file.txt"));

    // Some data.
    QByteArray data("This is short enough that compressing it is not efficient.");
    const QString name = QStringLiteral("name.txt");
    const QString description = QStringLiteral("description");

    // Create the original part.
    MessageCore::AttachmentPart::Ptr origPart = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart);
    origPart->setName(name);
    origPart->setDescription(description);
    origPart->setMimeType("text/pdf");
    origPart->setEncoding(KMime::Headers::CE8Bit);
    origPart->setData(data);
    origPart->setUrl(url);
    origPart->setSigned(true);
    origPart->setEncrypted(true);
    origPart->setInline(true);

    auto job = new MessageCore::AttachmentUpdateJob(origPart, this);

    VERIFYEXEC(job);
    QCOMPARE(origPart->isInline(), job->updatedPart()->isInline());
}

QTEST_MAIN(AttachmentUpdateJobTest)

#include "moc_attachmentupdatejobtest.cpp"
