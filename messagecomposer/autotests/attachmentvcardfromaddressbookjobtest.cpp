/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "attachmentvcardfromaddressbookjobtest.h"
#include <MessageComposer/AttachmentVcardFromAddressBookJob>
#include <KContacts/Addressee>
#include <QTest>

AttachmentVcardFromAddressBookJobTest::AttachmentVcardFromAddressBookJobTest(QObject *parent)
    : QObject(parent)
{
}

AttachmentVcardFromAddressBookJobTest::~AttachmentVcardFromAddressBookJobTest()
{
}

void AttachmentVcardFromAddressBookJobTest::testAttachmentVCardWithInvalidItem()
{
    Akonadi::Item item;
    auto job = new MessageComposer::AttachmentVcardFromAddressBookJob(item);
    QVERIFY(!job->exec());
    delete job;
    job = nullptr;
}

void AttachmentVcardFromAddressBookJobTest::testAttachmentVCardWithValidItem()
{
    Akonadi::Item item(42);
    item.setMimeType(KContacts::Addressee::mimeType());
    KContacts::Addressee address;
    const QString name = QStringLiteral("foo1");
    address.setName(name);
    item.setPayload<KContacts::Addressee>(address);
    auto job = new MessageComposer::AttachmentVcardFromAddressBookJob(item);
    QVERIFY(job->exec());

    MessageCore::AttachmentPart::Ptr part = job->attachmentPart();
    delete job;
    job = nullptr;

    QVERIFY(!part->data().isEmpty());
    QCOMPARE(part->mimeType(), QByteArray("text/x-vcard"));
    const QString newName = name + QLatin1String(".vcf");
    QCOMPARE(part->name(), newName);
    QVERIFY(part->description().isEmpty());
    QVERIFY(!part->isInline());
    QVERIFY(!part->fileName().isEmpty());
}

void AttachmentVcardFromAddressBookJobTest::testAttachmentVCardWithInvalidVCard()
{
    Akonadi::Item item(42);
    auto job = new MessageComposer::AttachmentVcardFromAddressBookJob(item);
    QVERIFY(!job->exec());
    delete job;
    job = nullptr;
}

void AttachmentVcardFromAddressBookJobTest::testAttachmentVCardWithEmptyVCard()
{
    Akonadi::Item item(42);
    item.setMimeType(KContacts::Addressee::mimeType());
    KContacts::Addressee address;
    item.setPayload<KContacts::Addressee>(address);
    auto job = new MessageComposer::AttachmentVcardFromAddressBookJob(item);
    QVERIFY(!job->exec());
    delete job;
    job = nullptr;
}

QTEST_MAIN(AttachmentVcardFromAddressBookJobTest)
