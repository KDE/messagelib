/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptheadersjobtest.h"

#include "qtest_messagecomposer.h"
#include "cryptofunctions.h"
#include "setupenv.h"

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/AutocryptHeadersJob>
#include <MessageComposer/Util>

#include <QGpgME/Protocol>
#include <QGpgME/KeyListJob>

#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>

#include <stdlib.h>
#include <KCharsets>

#include <QDebug>
#include <QTest>
#include <keylistresult.h>
#include <decryptionresult.h>

QTEST_MAIN(AutocryptHeadersJobTest)

using namespace MessageComposer;

void AutocryptHeadersJobTest::initTestCase()
{
    Test::setupEnv();
}

void AutocryptHeadersJobTest::testAutocryptHeader()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");
    skeletonMessage.to(true)->from7BitString("Bob <bob@autocrypt.example>");
    skeletonMessage.subject(true)->from7BitString("an Autocrypt header example using Ed25519+Cv25519 key");
    skeletonMessage.date(true)->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    skeletonMessage.messageID(true)->from7BitString("<abe640bb-018d-4f9d-b4d8-1636d6164e22@autocrypt.example>");

    KMime::Content content;
    content.contentType(true)->from7BitString("text/plain");
    content.setBody("This is an example e-mail with Autocrypt header and Ed25519+Cv25519 key (key\nfingerprint: ) as defined in Level 1 revision 1.1.\n");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector< GpgME::Key > ownKeys;
    auto res = job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);

    auto aJob = new AutocryptHeadersJob(&composer);

    QVERIFY(aJob);

    aJob->setContent(&content);
    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setRecipientKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    VERIFYEXEC(aJob);

    skeletonMessage.assemble();
    content.assemble();

    auto referenceFile = QStringLiteral("autcryptheader.mbox");
    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(skeletonMessage.head());
    f.write(content.encodedContent());
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/")+referenceFile);
}
