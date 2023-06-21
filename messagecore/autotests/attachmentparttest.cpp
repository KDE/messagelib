/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentparttest.h"
#include "qtest_messagecore.h"

#include <QHash>

#include "messagecore_debug.h"
#include <QTest>

#include <MessageCore/AttachmentPart>
using namespace MessageCore;

QTEST_MAIN(AttachmentPartTest)

void AttachmentPartTest::testApi()
{
    const QString str = QStringLiteral("test");
    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);

    // Test that an AttachmentPart::Ptr can be put in a QHash.
    QHash<AttachmentPart::Ptr, QString> hash;
    hash[part] = str;
    QVERIFY(hash.contains(part));

    // Test that an AttachmentPart::Ptr can be put in a QVariant.
    QVariant variant = QVariant::fromValue(part);
    QVERIFY(variant.isValid());
    QVERIFY(variant.canConvert<AttachmentPart::Ptr>());
    QVERIFY(variant.value<AttachmentPart::Ptr>() == part);
}

void AttachmentPartTest::shouldHaveDefaultValue()
{
    AttachmentPart part;
    QCOMPARE(part.size(), (qint64)-1);
    QVERIFY(!part.isInline());
    QVERIFY(part.isAutoEncoding());
    QVERIFY(!part.isCompressed());
    QVERIFY(!part.isEncrypted());
    QVERIFY(!part.isSigned());
    QCOMPARE(part.encoding(), KMime::Headers::CE7Bit);
    QVERIFY(!part.url().isValid());
    QVERIFY(part.name().isEmpty());
    QVERIFY(part.fileName().isEmpty());
    QVERIFY(part.description().isEmpty());
    QVERIFY(part.charset().isEmpty());
    QVERIFY(part.mimeType().isEmpty());
    QVERIFY(part.data().isEmpty());
}

#include "moc_attachmentparttest.cpp"
