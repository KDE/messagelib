/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "composerviewbasetest.h"
#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <MessageComposer/Composer>
#include <MessageComposer/ComposerViewBase>
#include <MessageComposer/MessageComposerSettings>
#include <MessageComposer/RecipientsEditor>
#include <MessageComposer/RichTextComposerNg>

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KIdentityManagementWidgets/IdentityCombo>

#include <MailTransport/TransportComboBox>

#include <QStandardPaths>
#include <QTest>

using namespace MessageComposer;

Q_DECLARE_METATYPE(Kleo::CryptoMessageFormat)

QTEST_MAIN(ComposerViewBaseTest)

ComposerViewBaseTest::ComposerViewBaseTest(QObject *parent)
    : QObject(parent)
{
    Test::setupEnv();
}

ComposerViewBaseTest::~ComposerViewBaseTest() = default;

void ComposerViewBaseTest::cleanupTestCase()
{
    delete mIdentMan;
    mIdentMan = nullptr;
    delete mIdentCombo;
    mIdentCombo = nullptr;
}

void ComposerViewBaseTest::initTestCase()
{
    mIdentMan = new KIdentityManagementCore::IdentityManager;

    KIdentityManagementCore::Identity &ident = mIdentMan->modifyIdentityForUoid(mIdentMan->defaultIdentity().uoid());
    ident.setAutocryptEnabled(true);
    ident.setPGPEncryptionKey("1BA323932B3FAA826132C79E8D9860C58F246DE6");
    ident.setPGPSigningKey("1BA323932B3FAA826132C79E8D9860C58F246DE6");
    mIdentMan->commit();
    mIdentCombo = new KIdentityManagementWidgets::IdentityCombo(mIdentMan);

    MessageComposerSettings::self()->setCryptoShowKeysForApproval(false);

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    autocryptDir = QDir(genericDataLocation.filePath(QStringLiteral("autocrypt")));
}

void ComposerViewBaseTest::init()
{
    autocryptDir.removeRecursively();
    autocryptDir.mkpath(QStringLiteral("."));
}

void ComposerViewBaseTest::cleanup()
{
    autocryptDir.removeRecursively();
}

void ComposerViewBaseTest::shouldHaveDefaultValue()
{
    ComposerViewBase composerViewBase;
    QVERIFY(!composerViewBase.attachmentModel());
    QVERIFY(!composerViewBase.attachmentController());
    QVERIFY(!composerViewBase.recipientsEditor());
    QVERIFY(!composerViewBase.signatureController());
    QVERIFY(!composerViewBase.identityCombo());
    QVERIFY(!composerViewBase.identityManager());
    QVERIFY(!composerViewBase.editor());
    QVERIFY(!composerViewBase.transportComboBox());
    QVERIFY(!composerViewBase.fccCombo());
    QVERIFY(!composerViewBase.dictionary());
    QVERIFY(composerViewBase.to().isEmpty());
    QVERIFY(composerViewBase.cc().isEmpty());
    QVERIFY(composerViewBase.bcc().isEmpty());
    QVERIFY(composerViewBase.from().isEmpty());
    QVERIFY(composerViewBase.replyTo().isEmpty());
    QVERIFY(composerViewBase.subject().isEmpty());
    QCOMPARE(composerViewBase.autoSaveInterval(), 60000);
}

void ComposerViewBaseTest::testAutoSaveMessage()
{
    QString fileName(QStringLiteral("testfile"));

    MessageComposer::RichTextComposerNg editor;
    MessageComposer::RecipientsEditor recipientsEditor;
    MailTransport::TransportComboBox transpCombo;
    ComposerViewBase composerViewBase;
    composerViewBase.setAutoSaveInterval(0);
    composerViewBase.setAutoSaveFileName(fileName);
    composerViewBase.setIdentityCombo(mIdentCombo);
    composerViewBase.setIdentityManager(mIdentMan);
    composerViewBase.setEditor(&editor);
    composerViewBase.setTransportCombo(&transpCombo);
    composerViewBase.setRecipientsEditor(&recipientsEditor);
    composerViewBase.setFrom(QStringLiteral("me@me.example"));
    KMime::Types::Mailbox mb;
    mb.from7BitString("to@to.example");
    recipientsEditor.setRecipientString({mb}, Recipient::To);
    editor.setPlainText(QStringLiteral("Hello,\n\nThis is a test message\n\nGreez"));

    composerViewBase.autoSaveMessage();

    // It may be possible that we need to wait till the autosave message is ready composed.
    if (composerViewBase.m_composers.size() > 0) {
        QEventLoop loop;
        bool notFinished = false;
        for (const auto composer : composerViewBase.m_composers) {
            if (!composer->finished()) {
                notFinished = true;
                connect(composer, &MessageComposer::Composer::result, this, [&composerViewBase, &loop]() {
                    if (composerViewBase.m_composers.size() < 1) {
                        loop.quit();
                    }
                });
            }
        }
        if (notFinished) {
            loop.exec();
        }
    }

    const QString autosavePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1StringView("/kmail2/autosave/");
    auto msg = Test::loadMessage(autosavePath + fileName);
    msg->messageID()->from7BitString("<test@autotest.example>");
    msg->date()->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    msg->assemble();
    Test::compareFile(msg.data(), QStringLiteral(MAIL_DATA_DIR "/autosave.mbox"));
}

void ComposerViewBaseTest::testGenerateCryptoMessagesAutocrypt_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("sign");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<Kleo::CryptoMessageFormat>("format");

    QString data = QStringLiteral("Hello,\n\nThis is a test message\n\nGreez");

    QTest::newRow("Plain") << data << false << false << Kleo::AutoFormat;
    QTest::newRow("Sign") << data << true << false << Kleo::AutoFormat;
    QTest::newRow("Encrypt") << data << false << true << Kleo::AutoFormat;
    QTest::newRow("SignEncrypt") << data << true << true << Kleo::AutoFormat;
}

void ComposerViewBaseTest::testGenerateCryptoMessagesAutocrypt()
{
    QFETCH(QString, data);
    QFETCH(bool, sign);
    QFETCH(bool, encrypt);
    QFETCH(Kleo::CryptoMessageFormat, format);

    MessageComposer::RichTextComposerNg editor;
    MessageComposer::RecipientsEditor recipientsEditor;
    MailTransport::TransportComboBox transpCombo;
    ComposerViewBase composerViewBase;
    composerViewBase.setIdentityCombo(mIdentCombo);
    composerViewBase.setIdentityManager(mIdentMan);
    composerViewBase.setEditor(&editor);
    composerViewBase.setTransportCombo(&transpCombo);
    composerViewBase.setRecipientsEditor(&recipientsEditor);
    composerViewBase.setFrom(QStringLiteral("me@me.example"));
    composerViewBase.mExpandedTo << QStringLiteral("you@you.com");
    composerViewBase.setAkonadiLookupEnabled(false);
    KMime::Types::Mailbox mb;
    mb.from7BitString("you@you.com");
    recipientsEditor.setRecipientString({mb}, Recipient::To);
    editor.setPlainText(data);

    bool wasCanceled = false;
    composerViewBase.setCryptoOptions(sign, encrypt, format, false);
    auto composers = composerViewBase.generateCryptoMessages(wasCanceled);
    QCOMPARE(wasCanceled, false);

    QCOMPARE(composers.size(), 1); // No additional composers are created

    auto composer = composers.first();
    composerViewBase.fillComposer(composer);

    VERIFYEXEC(composer);

    QCOMPARE(composer->resultMessages().size(), 1);

    auto msg = composer->resultMessages().first();
    msg->assemble();

    // Every message should have an Autocrypt in the outer message structure
    // as it should always been possible to start an encrypted reply
    QCOMPARE(msg->hasHeader("Autocrypt"), true);

    MimeTreeParser::SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    otp.parseObjectTree(msg.data());

    KMime::Content *content = msg.data();

    if (encrypt) {
        QCOMPARE(nh->encryptionState(msg.data()), MimeTreeParser::KMMsgFullyEncrypted);
        const auto extra = nh->extraContents(msg.data());
        QCOMPARE(extra.size(), 1);
        content = extra.first();
    } else {
        QCOMPARE(nh->encryptionState(msg.data()), MimeTreeParser::KMMsgNotEncrypted);
    }

    if (sign) {
        QVERIFY(!content->contents().empty());
        content = content->contents().at(0);
        QCOMPARE(nh->signatureState(content), MimeTreeParser::KMMsgFullySigned);
    } else {
        QCOMPARE(nh->signatureState(msg.data()), MimeTreeParser::KMMsgNotSigned);
    }

    QCOMPARE(QString::fromUtf8(content->decodedContent()), data);
}

void ComposerViewBaseTest::testGenerateCryptoMessagesAutocryptSMime()
{
    QString data = QStringLiteral("Hello,\n\nThis is a test message\n\nGreez");

    MessageComposer::RichTextComposerNg editor;
    MessageComposer::RecipientsEditor recipientsEditor;
    MailTransport::TransportComboBox transpCombo;
    ComposerViewBase composerViewBase;
    composerViewBase.setIdentityCombo(mIdentCombo);
    composerViewBase.setIdentityManager(mIdentMan);
    composerViewBase.setEditor(&editor);
    composerViewBase.setTransportCombo(&transpCombo);
    composerViewBase.setRecipientsEditor(&recipientsEditor);
    composerViewBase.setFrom(QStringLiteral("me@me.example"));
    composerViewBase.mExpandedTo << QStringLiteral("you@you.com");
    composerViewBase.setAkonadiLookupEnabled(false);
    KMime::Types::Mailbox mb;
    mb.from7BitString("you@you.com");
    recipientsEditor.setRecipientString({mb}, Recipient::To);
    editor.setPlainText(data);

    bool wasCanceled = false;
    composerViewBase.setCryptoOptions(false, false, Kleo::AnySMIME, false);
    auto composers = composerViewBase.generateCryptoMessages(wasCanceled);
    QCOMPARE(wasCanceled, false);

    QCOMPARE(composers.size(), 1); // No additional composers are created

    auto composer = composers.first();
    composerViewBase.fillComposer(composer);

    VERIFYEXEC(composer);

    QCOMPARE(composer->resultMessages().size(), 1);

    auto msg = composer->resultMessages().first();
    msg->assemble();

    QCOMPARE(QString::fromUtf8(msg->decodedContent()), data);
    QCOMPARE(msg->hasHeader("Autocrypt"), false);
}

void ComposerViewBaseTest::testAutocryptKey()
{
    QFile file(QLatin1StringView(MESSAGECORE_DATA_DIR) + QStringLiteral("/autocrypt/recipient.json"));
    QVERIFY(file.copy(autocryptDir.path() + QStringLiteral("/recipient%40autocrypt.example.json")));

    qDebug() << autocryptDir.path();

    QString data = QStringLiteral("Hello,\n\nThis is a test message\n\nGreez");

    MessageComposer::RichTextComposerNg editor;
    MessageComposer::RecipientsEditor recipientsEditor;
    MailTransport::TransportComboBox transpCombo;
    ComposerViewBase composerViewBase;
    composerViewBase.setIdentityCombo(mIdentCombo);
    composerViewBase.setIdentityManager(mIdentMan);
    composerViewBase.setEditor(&editor);
    composerViewBase.setTransportCombo(&transpCombo);
    composerViewBase.setRecipientsEditor(&recipientsEditor);
    composerViewBase.setFrom(QStringLiteral("me@me.example"));
    composerViewBase.mExpandedTo << QStringLiteral("recipient@autocrypt.example");
    composerViewBase.setAkonadiLookupEnabled(false);
    KMime::Types::Mailbox mb;
    mb.from7BitString("recipient@autocrypt.example");
    recipientsEditor.setRecipientString({mb}, Recipient::To);
    editor.setPlainText(data);

    bool wasCanceled = false;
    composerViewBase.setCryptoOptions(false, true, Kleo::AutoFormat, false);
    auto composers = composerViewBase.generateCryptoMessages(wasCanceled);
    QCOMPARE(wasCanceled, false);

    QCOMPARE(composers.size(), 1); // No additional composers are created

    auto composer = composers.first();
    composerViewBase.fillComposer(composer);

    VERIFYEXEC(composer);

    QCOMPARE(composer->resultMessages().size(), 1);

    // The key for recipient@autocrypt.example is only available in Autocrypt
    // test is we created a special GNUPGHOME to contain all needed keys.
    QVERIFY(!composer->gnupgHome().isEmpty());

    auto msg = composer->resultMessages().first();
    msg->assemble();

    // Every message should have an Autocrypt in the outer message structure
    // as it should always been possible to start an encrypted reply
    QCOMPARE(msg->hasHeader("Autocrypt"), true);

    MimeTreeParser::SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    auto nh = new MimeTreeParser::NodeHelper;
    MimeTreeParser::ObjectTreeParser otp(&testSource, nh);

    otp.parseObjectTree(msg.data());

    KMime::Content *content = msg.data();

    QCOMPARE(nh->encryptionState(msg.data()), MimeTreeParser::KMMsgFullyEncrypted);
    const auto extra = nh->extraContents(msg.data());
    QCOMPARE(extra.size(), 1);
    content = extra.first();

    QCOMPARE(QString::fromUtf8(content->decodedContent()), data);
}

#include "moc_composerviewbasetest.cpp"
