/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cryptocomposertest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"

#include "setupenv.h"

#include <Libkleo/Enum>

#include <KMime/Headers>

#include <MessageComposer/AttachmentControllerBase>
#include <MessageComposer/AttachmentModel>
#include <MessageComposer/ComposerJob>
#include <MessageComposer/ComposerViewBase>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/RichTextComposerNg>
#include <MessageComposer/TextPart>
#include <MessageComposer/Util>

#include <MessageCore/AttachmentPart>

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>
#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>

#include <gpgme.h>

#include <QDebug>
#include <QTest>

using namespace KMime;
using namespace MessageComposer;
using namespace MessageCore;

Q_DECLARE_METATYPE(MessageCore::AttachmentPart)

QTEST_MAIN(CryptoComposerTest)

void CryptoComposerTest::initMain()
{
    gpgme_check_version(nullptr);
}

void CryptoComposerTest::initTestCase()
{
    MessageComposer::Test::setupEnv();
}

Q_DECLARE_METATYPE(Headers::contentEncoding)

// openpgp
void CryptoComposerTest::testOpenPGPMime_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    QTest::newRow("SignOpenPGPMime") << data << true << false << Headers::CE7Bit;
    QTest::newRow("EncryptOpenPGPMime") << data << false << true << Headers::CE7Bit;
    QTest::newRow("SignEncryptOpenPGPMime") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testOpenPGPMime()
{
    QFETCH(QString, data);
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);
    QFETCH(Headers::contentEncoding, cte);

    auto composerJob = new ComposerJob;

    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    composerJob->setSignAndEncrypt(sign, encrypt);
    composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().first();
    delete composerJob;
    composerJob = nullptr;

    // qDebug()<< "message:" << message.data()->encodedContent();
    ComposerTestUtil::verify(sign, encrypt, message.data(), data.toUtf8(), Kleo::OpenPGPMIMEFormat, cte);

    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
}

// the following will do for s-mime as well, as the same sign/enc jobs are used
void CryptoComposerTest::testEncryptSameAttachments_data()
{
    QTest::addColumn<int>("format");

    QTest::newRow("OpenPGPMime") << (int)Kleo::OpenPGPMIMEFormat;
    // TODO: fix Inline PGP with encrypted attachments
    // QTest::newRow( "InlineOpenPGP" ) << (int) Kleo::InlineOpenPGPFormat;
}

void CryptoComposerTest::testEncryptSameAttachments()
{
    QFETCH(int, format);
    auto composerJob = new ComposerJob;
    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    attachment->setFileName(QString::fromLocal8Bit("anattachment.txt"));
    attachment->setEncrypted(true);
    attachment->setSigned(false);
    composerJob->addAttachmentPart(attachment);

    composerJob->setSignAndEncrypt(false, true);
    composerJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)format);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().constFirst();
    delete composerJob;
    composerJob = nullptr;

    // qDebug()<< "message:" << message.data()->encodedContent();
    ComposerTestUtil::verifyEncryption(message.data(), data.toUtf8(), (Kleo::CryptoMessageFormat)format, true);

    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));

    MimeTreeParser::SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    otp.parseObjectTree(message.data());
    KMime::Message::Ptr unencrypted = nh->unencryptedMessage(message);

    KMime::Content *testAttachment = Util::findTypeInMessage(unencrypted.data(), "x-some", "x-type");

    QCOMPARE(testAttachment->body(), QString::fromLatin1("abc").toUtf8());
    QCOMPARE(testAttachment->contentDisposition()->filename(), QString::fromLatin1("anattachment.txt"));
}

void CryptoComposerTest::testEditEncryptAttachments_data()
{
    QTest::addColumn<int>("format");
    QTest::newRow("OpenPGPMime") << (int)Kleo::OpenPGPMIMEFormat;
    // TODO: SMIME should also be tested
}

void CryptoComposerTest::testEditEncryptAttachments()
{
    QFETCH(int, format);
    auto composerJob = new ComposerJob;
    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    const QString fileName = u"anattachment.txt"_s;
    const QByteArray fileData = "abc";
    attachment->setData(fileData);
    attachment->setMimeType("x-some/x-type");
    attachment->setFileName(fileName);
    attachment->setEncrypted(true);
    attachment->setSigned(false);
    composerJob->addAttachmentPart(attachment);

    composerJob->setSignAndEncrypt(false, true);
    composerJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)format);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().first();
    delete composerJob;
    composerJob = nullptr;

    // setup a viewer
    ComposerViewBase view(this, nullptr);
    AttachmentModel model(this);
    AttachmentControllerBase controller(&model, nullptr, nullptr);
    MessageComposer::RichTextComposerNg editor;
    view.setAttachmentModel(&model);
    view.setAttachmentController(&controller);
    view.setEditor(&editor);

    // Let's load the email to the viewer
    view.setMessage(message, true);

    QModelIndex index = model.index(0, 0);
    QCOMPARE(editor.toPlainText(), data);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(index, AttachmentModel::NameRole).toString(), fileName);
    AttachmentPart::Ptr part = model.attachments()[0];
    QCOMPARE(part->data(), fileData);
    QCOMPARE(part->fileName(), fileName);
}

void CryptoComposerTest::testEditEncryptAndLateAttachments_data()
{
    QTest::addColumn<int>("format");
    QTest::newRow("OpenPGPMime") << (int)Kleo::OpenPGPMIMEFormat;
    // TODO: SMIME should also be tested
}

void CryptoComposerTest::testEditEncryptAndLateAttachments()
{
    QFETCH(int, format);
    auto composerJob = new ComposerJob;
    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    const QString fileName = u"anattachment.txt"_s;
    const QByteArray fileData = "abc";
    const QString fileName2 = u"nonencrypt.txt"_s;
    const QByteArray fileData2 = "readable";
    attachment->setData(fileData);
    attachment->setMimeType("x-some/x-type");
    attachment->setFileName(fileName);
    attachment->setEncrypted(true);
    attachment->setSigned(false);
    composerJob->addAttachmentPart(attachment);

    attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData(fileData2);
    attachment->setMimeType("x-some/x-type2");
    attachment->setFileName(fileName2);
    attachment->setEncrypted(false);
    attachment->setSigned(false);
    composerJob->addAttachmentPart(attachment);

    composerJob->setSignAndEncrypt(false, true);
    composerJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)format);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().constFirst();
    delete composerJob;
    composerJob = nullptr;

    // setup a viewer
    ComposerViewBase view(this, nullptr);
    AttachmentModel model(this);
    AttachmentControllerBase controller(&model, nullptr, nullptr);
    MessageComposer::RichTextComposerNg editor;
    view.setAttachmentModel(&model);
    view.setAttachmentController(&controller);
    view.setEditor(&editor);

    // Let's load the email to the viewer
    view.setMessage(message, true);

    // QModelIndex index = model.index(0, 0);
    QCOMPARE(editor.toPlainText(), data);
    QCOMPARE(model.rowCount(), 2);
    AttachmentPart::Ptr part = model.attachments()[0];
    QCOMPARE(part->fileName(), fileName);
    QCOMPARE(part->data(), fileData);
    part = model.attachments()[1];
    QCOMPARE(part->fileName(), fileName2);
    QCOMPARE(part->data(), fileData2);
}

void CryptoComposerTest::testSignEncryptLateAttachments_data()
{
    QTest::addColumn<int>("format");

    QTest::newRow("OpenPGPMime") << (int)Kleo::OpenPGPMIMEFormat;
    QTest::newRow("InlineOpenPGP") << (int)Kleo::InlineOpenPGPFormat;
}

void CryptoComposerTest::testSignEncryptLateAttachments()
{
    QFETCH(int, format);
    auto composerJob = new ComposerJob;
    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    attachment->setFileName(QString::fromLocal8Bit("anattachment.txt"));
    attachment->setEncrypted(false);
    attachment->setSigned(false);
    composerJob->addAttachmentPart(attachment);

    composerJob->setSignAndEncrypt(true, true);
    composerJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)format);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().constFirst();
    delete composerJob;
    composerJob = nullptr;

    // as we have an additional attachment, just ignore it when checking for sign/encrypt
    QVERIFY(message->contents().size() >= 2);
    KMime::Content *b = message->contents().at(0);
    ComposerTestUtil::verifySignatureAndEncryption(b, data.toUtf8(), (Kleo::CryptoMessageFormat)format, true);

    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));

    // now check the attachment separately
    QCOMPARE(QString::fromLatin1(message->contents().at(1)->body()), QString::fromLatin1("abc"));
}

// protected headers
void CryptoComposerTest::testProtectedHeaders_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    QTest::newRow("SignOpenPGPMime") << data << true << false << Headers::CE7Bit;
    QTest::newRow("EncryptOpenPGPMime") << data << false << true << Headers::CE7Bit;
    QTest::newRow("SignEncryptOpenPGPMime") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testProtectedHeaders()
{
    QFETCH(QString, data);
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);
    QFETCH(Headers::contentEncoding, cte);

    auto composerJob = new ComposerJob;

    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    composerJob->setSignAndEncrypt(sign, encrypt);
    composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().first();

    // parse the result and make sure it is valid in various ways
    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::NodeHelper nh;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nh);

    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.data());

    if (encrypt) {
        QCOMPARE(nh.encryptionState(message.data()), MimeTreeParser::KMMsgFullyEncrypted);
    }

    if (sign && !encrypt) {
        QCOMPARE(nh.signatureState(message.data()), MimeTreeParser::KMMsgFullySigned);
    }

    KMime::Content *part = nullptr;

    if (sign && !encrypt) {
        part = Util::findTypeInMessage(message.data(), "text", "plain");
    } else if (encrypt) {
        auto extraContents = nh.extraContents(message.data());
        QCOMPARE(extraContents.size(), 1);
        part = extraContents.first();
        if (sign) {
            QVERIFY(!part->contentType(KMime::CreatePolicy::DontCreate)->hasParameter("protected-headers"));
            QVERIFY(!part->headerByType("to"));
            QVERIFY(!part->headerByType("from:"));
            part = Util::findTypeInMessage(part, "text", "plain");
        }
    }
    QVERIFY(part);
    QCOMPARE(part->contentType(KMime::CreatePolicy::DontCreate)->parameter("protected-headers"), u"v1"_s);
    QCOMPARE(part->headerByType("to")->asUnicodeString(), u"you@you.you"_s);
    QCOMPARE(part->headerByType("from")->asUnicodeString(), u"me@me.me"_s);
}

void CryptoComposerTest::testBCCEncrypt_data()
{
    QTest::addColumn<int>("format");

    QTest::newRow("OpenPGPMime") << (int)Kleo::OpenPGPMIMEFormat;
    QTest::newRow("InlineOpenPGP") << (int)Kleo::InlineOpenPGPFormat;
}

// secondary recipients

void CryptoComposerTest::testBCCEncrypt()
{
    QFETCH(int, format);
    auto composerJob = new ComposerJob;
    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    fillComposerData(composerJob, data);
    composerJob->infoPart()->setBcc(QStringList(u"bcc@bcc.org"_s));

    std::vector<GpgME::Key> keys = MessageComposer::Test::getKeys();

    QStringList primRecipients;
    primRecipients << QString::fromLocal8Bit("you@you.you");
    std::vector<GpgME::Key> pkeys;
    pkeys.push_back(keys[1]);

    QStringList secondRecipients;
    secondRecipients << QString::fromLocal8Bit("bcc@bcc.org");
    std::vector<GpgME::Key> skeys;
    skeys.push_back(keys[2]);

    QList<QPair<QStringList, std::vector<GpgME::Key>>> encKeys;
    encKeys.append(QPair<QStringList, std::vector<GpgME::Key>>(primRecipients, pkeys));
    encKeys.append(QPair<QStringList, std::vector<GpgME::Key>>(secondRecipients, skeys));

    composerJob->setSignAndEncrypt(true, true);
    composerJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)format);

    composerJob->setEncryptionKeys(encKeys);
    composerJob->setSigningKeys(keys);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 2);

    KMime::Message::Ptr primMessage = composerJob->resultMessages().constFirst();
    KMime::Message::Ptr secMessage = composerJob->resultMessages()[1];
    delete composerJob;
    composerJob = nullptr;

    ComposerTestUtil::verifySignatureAndEncryption(primMessage.data(), data.toUtf8(), (Kleo::CryptoMessageFormat)format);

    QCOMPARE(primMessage->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(primMessage->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));

    ComposerTestUtil::verifySignatureAndEncryption(secMessage.data(), data.toUtf8(), (Kleo::CryptoMessageFormat)format);

    QCOMPARE(secMessage->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(secMessage->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
}

// inline pgp
void CryptoComposerTest::testOpenPGPInline_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    QTest::newRow("SignOpenPGPInline") << data << true << false << Headers::CE7Bit;
    QTest::newRow("EncryptOpenPGPInline") << data << false << true << Headers::CE7Bit;
    QTest::newRow("SignEncryptOpenPGPInline") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testOpenPGPInline()
{
    QFETCH(QString, data);
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);
    QFETCH(Headers::contentEncoding, cte);

    auto composerJob = new ComposerJob;

    fillComposerData(composerJob, data);
    fillComposerCryptoData(composerJob);

    composerJob->setSignAndEncrypt(sign, encrypt);
    composerJob->setCryptoMessageFormat(Kleo::InlineOpenPGPFormat);

    VERIFYEXEC(composerJob);
    QCOMPARE(composerJob->resultMessages().size(), 1);

    KMime::Message::Ptr message = composerJob->resultMessages().constFirst();
    delete composerJob;
    composerJob = nullptr;

    if (sign && !encrypt) {
        data += u'\n';
    }
    // qDebug() << "message:" << message->encodedContent();
    ComposerTestUtil::verify(sign, encrypt, message.data(), data.toUtf8(), Kleo::InlineOpenPGPFormat, cte);

    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
}

// s-mime

void CryptoComposerTest::testSMIME_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    QTest::newRow("SignSMIME") << data << true << false << Headers::CE7Bit;
    QTest::newRow("EncryptSMIME") << data << false << true << Headers::CE7Bit;
    QTest::newRow("SignEncryptSMIME") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testSMIME()
{
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);

    runSMIMETest(sign, encrypt, false);
}

void CryptoComposerTest::testSMIMEOpaque_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
    QTest::newRow("SignSMIMEOpaque") << data << true << false << Headers::CE7Bit;
    QTest::newRow("EncryptSMIMEOpaque") << data << false << true << Headers::CE7Bit;
    QTest::newRow("SignEncryptSMIMEOpaque") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testSMIMEOpaque()
{
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);

    runSMIMETest(sign, encrypt, true);
}

// contentTransferEncoding

void CryptoComposerTest::testCTEquPr_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(QString::fromLatin1("All happy families are alike; each unhappy family is unhappy in its own way. [ä]"));
    QTest::newRow("CTEquPr:Sign") << data << true << false << Headers::CEquPr;
    QTest::newRow("CTEquPr:Encrypt") << data << false << true << Headers::CE7Bit;
    QTest::newRow("CTEquPr:SignEncrypt") << data << true << true << Headers::CE7Bit;

    data = QStringLiteral(
        "All happy families are alike;\n\n\n\neach unhappy family is unhappy in its own way.\n--\n hallloasdfasdfsadfsdf asdf sadfasdf sdf sdf sdf sadfasdf "
        "sdaf daf sdf asdf sadf asdf asdf [ä]");
    QTest::newRow("CTEquPr:Sign:Newline") << data << true << false << Headers::CEquPr;
    QTest::newRow("CTEquPr:Encrypt:Newline") << data << false << true << Headers::CE7Bit;
    QTest::newRow("CTEquPr:SignEncrypt:Newline") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testCTEquPr()
{
    testSMIME();
    testSMIMEOpaque();
    testOpenPGPMime();
    testOpenPGPInline();
}

void CryptoComposerTest::testCTEbase64_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Headers::contentEncoding>("cte");

    QString data(
        QStringLiteral("[ääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääää"
                       "äääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääääää"
                       "äääääääääääääääääääääääääääääääääääääääääääääää]"));
    QTest::newRow("CTEbase64:Sign") << data << true << false << Headers::CEbase64;
    QTest::newRow("CTEbase64:Encrypt") << data << false << true << Headers::CE7Bit;
    QTest::newRow("CTEbase64:SignEncrypt") << data << true << true << Headers::CE7Bit;
}

void CryptoComposerTest::testCTEbase64()
{
    testSMIME();
    testSMIMEOpaque();
    testOpenPGPMime();
    testOpenPGPInline();
}

void CryptoComposerTest::testSetGnupgHome_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);

    QTest::newRow("sign") << data << true;
    QTest::newRow("notsign") << data << false;
}

void CryptoComposerTest::testSetGnupgHome()
{
    QFETCH(QString, data);
    QFETCH(bool, sign);

    bool encrypt(true);

    KMime::Message::Ptr message;
    QTemporaryDir dir;
    {
        auto composerJob = new ComposerJob;

        fillComposerData(composerJob, data);
        fillComposerCryptoData(composerJob);

        composerJob->setGnupgHome(dir.path());
        composerJob->setSignAndEncrypt(sign, encrypt);
        composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

        QCOMPARE(composerJob->exec(), false);
    }

    const std::vector<GpgME::Key> &keys = Test::getKeys();
    for (const auto &key : keys) {
        Test::populateKeyring(dir.path(), key);
    }

    {
        auto composerJob = new ComposerJob;

        fillComposerData(composerJob, data);
        fillComposerCryptoData(composerJob);

        composerJob->setGnupgHome(dir.path());
        composerJob->setSignAndEncrypt(sign, encrypt);
        composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

        VERIFYEXEC((composerJob));
        QCOMPARE(composerJob->resultMessages().size(), 1);

        message = composerJob->resultMessages().first();
    }

    ComposerTestUtil::verify(sign, encrypt, message.data(), data.toUtf8(), Kleo::OpenPGPMIMEFormat, Headers::CE7Bit);

    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
}

void CryptoComposerTest::testAutocryptHeaders_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<bool>("sign");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);

    QTest::newRow("encrypt+sign") << data << true << true;
    QTest::newRow("encrypt") << data << true << false;
    QTest::newRow("sign") << data << false << true;
    QTest::newRow("noencrypt+nosign") << data << false << false;
}

void CryptoComposerTest::testAutocryptHeaders()
{
    QFETCH(QString, data);
    QFETCH(bool, encrypt);
    QFETCH(bool, sign);

    std::vector<GpgME::Key> keys = Test::getKeys();

    KMime::Message::Ptr message;
    QTemporaryDir dir;
    {
        auto composerJob = new ComposerJob;

        fillComposerData(composerJob, data);
        fillComposerCryptoData(composerJob);

        composerJob->setAutocryptEnabled(true);
        composerJob->setSenderEncryptionKey(keys[0]);
        composerJob->setGnupgHome(dir.path());
        composerJob->setSignAndEncrypt(sign, encrypt);
        composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

        QCOMPARE(composerJob->exec(), false);
    }

    for (const auto &key : keys) {
        Test::populateKeyring(dir.path(), key);
    }

    {
        auto composerJob = new ComposerJob;

        fillComposerData(composerJob, data);
        fillComposerCryptoData(composerJob);

        composerJob->setAutocryptEnabled(true);
        composerJob->setSenderEncryptionKey(keys[0]);
        composerJob->setGnupgHome(dir.path());
        composerJob->setSignAndEncrypt(sign, encrypt);
        composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

        VERIFYEXEC((composerJob));
        QCOMPARE(composerJob->resultMessages().size(), 1);

        message = composerJob->resultMessages().first();
    }

    if (sign || encrypt) {
        ComposerTestUtil::verify(sign, encrypt, message.data(), data.toUtf8(), Kleo::OpenPGPMIMEFormat, Headers::CE7Bit);
    }

    QVERIFY(message->headerByType("autocrypt"));
    QVERIFY(message->headerByType("Autocrypt")
                ->asUnicodeString()
                .startsWith(u"addr=me@me.me; keydata= mQENBEr9ij4BCADaFvyhzV7IrCAr/sCvfoPerAd4dYIGTeCeBmInu3p4oEG0rXTB2zL2t9zd7jV"_s));
    QVERIFY(!message->headerByType("autocrypt-gossip"));
    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
}

void CryptoComposerTest::testAutocryptGossip_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<QStringList>("recipients");

    QString data(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);

    QStringList recipients({u"you@you.you"_s});

    QTest::newRow("encrypt") << data << true << false << recipients;
    QTest::newRow("encrypt+sign") << data << true << true << recipients;
    QTest::newRow("sign") << data << false << true << recipients;

    recipients.append(u"leo@kdab.com"_s);
    QTest::newRow("encrypt-multiple") << data << true << false << recipients;
    QTest::newRow("encrypt+sign-multiple") << data << true << true << recipients;
    QTest::newRow("sign-multiple") << data << false << true << recipients;
}

void CryptoComposerTest::testAutocryptGossip()
{
    QFETCH(QString, data);
    QFETCH(bool, encrypt);
    QFETCH(bool, sign);
    QFETCH(QStringList, recipients);

    std::vector<GpgME::Key> keys = Test::getKeys();

    KMime::Message::Ptr message;

    {
        auto composerJob = new ComposerJob;

        fillComposerData(composerJob, data);
        composerJob->infoPart()->setTo(recipients);
        fillComposerCryptoData(composerJob);

        if (recipients.size() > 1) {
            auto job = QGpgME::openpgp()->keyListJob(false);
            std::vector<GpgME::Key> eKeys;
            GpgME::KeyListResult res = job->exec(recipients, false, eKeys);

            QVERIFY(!res.error());
            QCOMPARE(eKeys.size(), 1);

            eKeys.push_back(keys[0]);
            eKeys.push_back(keys[1]);

            QList<QPair<QStringList, std::vector<GpgME::Key>>> encKeys;
            encKeys.append({recipients, eKeys});

            composerJob->setEncryptionKeys(encKeys);
        }
        composerJob->setAutocryptEnabled(true);
        composerJob->setSenderEncryptionKey(keys[0]);
        composerJob->setSignAndEncrypt(sign, encrypt);
        composerJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

        VERIFYEXEC((composerJob));
        QCOMPARE(composerJob->resultMessages().size(), 1);

        message = composerJob->resultMessages().first();
    }

    if (sign || encrypt) {
        ComposerTestUtil::verify(sign, encrypt, message.data(), data.toUtf8(), Kleo::OpenPGPMIMEFormat, Headers::CE7Bit);
    }

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::NodeHelper nh;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nh);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.data());

    QVERIFY(!message->headerByType("autocrypt-gossip"));
    if (encrypt && recipients.size() > 1) {
        QCOMPARE(nh.headers("autocrypt-gossip", message.data()).size(), 2);

        auto headers = QStringList();
        for (const auto header : nh.headers("autocrypt-gossip", message.data())) {
            headers.append(header->asUnicodeString());
        }

        headers.sort();

        QVERIFY(headers[0].startsWith(u"addr=leo@kdab.com; keydata= mQINBEr4pSwBEADG/B1VaoxT7mnQfwekkY+f8wkqFVLvTwN0W59Ze/pxmuRf/iS0tZjsEiPK0za"_s));
        QVERIFY(headers[1].startsWith(u"addr=you@you.com; keydata= mI0ESw2YuAEEALakcld4goNkwIL5gMETM3R+zI+AoJcuQWUpvS7AqwyR9/UAkVd3D+r32CgWhFi"_s));
    } else {
        QCOMPARE(nh.headers("autocrypt-gossip", message.data()).size(), 0);
    }
    QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("me@me.me"));
    QCOMPARE(message->to()->asUnicodeString(), recipients.join(u", "_s));
}

// Helper methods
void CryptoComposerTest::fillComposerData(ComposerJob *composerJob, const QString &data)
{
    composerJob->infoPart()->setFrom(u"me@me.me"_s);
    composerJob->infoPart()->setTo(QStringList(u"you@you.you"_s));
    composerJob->textPart()->setWrappedPlainText(data);
}

void CryptoComposerTest::fillComposerCryptoData(ComposerJob *composerJob)
{
    std::vector<GpgME::Key> keys = MessageComposer::Test::getKeys();

    // qDebug() << "got num of keys:" << keys.size();

    QStringList recipients;
    recipients << QString::fromLocal8Bit("you@you.you");

    QList<QPair<QStringList, std::vector<GpgME::Key>>> encKeys;
    encKeys.append(QPair<QStringList, std::vector<GpgME::Key>>(recipients, keys));

    composerJob->setEncryptionKeys(encKeys);
    composerJob->setSigningKeys(keys);
}

void CryptoComposerTest::runSMIMETest(bool sign, bool enc, bool opaque)
{
    QFETCH(QString, data);
    QFETCH(Headers::contentEncoding, cte);

    auto composerJob = new ComposerJob;

    fillComposerData(composerJob, data);
    composerJob->infoPart()->setFrom(u"test@example.com"_s);

    std::vector<GpgME::Key> keys = MessageComposer::Test::getKeys(true);
    QStringList recipients;
    recipients << QString::fromLocal8Bit("you@you.you");
    QList<QPair<QStringList, std::vector<GpgME::Key>>> encKeys;
    encKeys.append(QPair<QStringList, std::vector<GpgME::Key>>(recipients, keys));
    composerJob->setEncryptionKeys(encKeys);
    composerJob->setSigningKeys(keys);
    composerJob->setSignAndEncrypt(sign, enc);
    Kleo::CryptoMessageFormat f;
    if (opaque) {
        f = Kleo::SMIMEOpaqueFormat;
    } else {
        f = Kleo::SMIMEFormat;
    }
    composerJob->setCryptoMessageFormat(f);

    const bool result = composerJob->exec();
    // QEXPECT_FAIL("", "GPG setup problems", Continue);
    QVERIFY(result);
    if (result) {
        QCOMPARE(composerJob->resultMessages().size(), 1);
        KMime::Message::Ptr message = composerJob->resultMessages().constFirst();
        delete composerJob;
        composerJob = nullptr;

        // qDebug() << "message:" << message->encodedContent();

        ComposerTestUtil::verify(sign, enc, message.data(), data.toUtf8(), f, cte);

        QCOMPARE(message->from()->asUnicodeString(), QString::fromLocal8Bit("test@example.com"));
        QCOMPARE(message->to()->asUnicodeString(), QString::fromLocal8Bit("you@you.you"));
    }
}

#include "moc_cryptocomposertest.cpp"
