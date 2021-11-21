/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "composerviewbasetest.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <MessageComposer/Composer>
#include <MessageComposer/ComposerViewBase>
#include <MessageComposer/RichTextComposerNg>
#include <MessageComposer/RecipientsEditor>

#include <KIdentityManagement/KIdentityManagement/Identity>
#include <KIdentityManagement/KIdentityManagement/IdentityCombo>
#include <KIdentityManagement/KIdentityManagement/IdentityManager>

#include <MailTransport/TransportComboBox>

#include <QStandardPaths>
#include <QTest>

using namespace MessageComposer;

ComposerViewBaseTest::ComposerViewBaseTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

ComposerViewBaseTest::~ComposerViewBaseTest()
{
}

void ComposerViewBaseTest::cleanupTestCase()
{
    delete mIdentMan;
    mIdentMan = nullptr;
    delete mIdentCombo;
    mIdentCombo = nullptr;
}

void ComposerViewBaseTest::initTestCase()
{
    mIdentMan = new KIdentityManagement::IdentityManager;

    KIdentityManagement::Identity &ident = mIdentMan->modifyIdentityForUoid(mIdentMan->defaultIdentity().uoid());
    ident.setAutocryptEnabled(true);
    mIdentMan->commit();
    mIdentCombo = new KIdentityManagement::IdentityCombo(mIdentMan);
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
                connect(composer, &MessageComposer::Composer::result, [&composerViewBase, &loop]() {
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

    const QString autosavePath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/kmail2/autosave/");
    auto msg = Test::loadMessage(autosavePath + fileName);
    msg->messageID()->from7BitString("<test@autotest.example>");
    msg->date()->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    msg->assemble();
    Test::compareFile(msg.data(), QStringLiteral(MAIL_DATA_DIR "/autosave.mbox"));
}

void ComposerViewBaseTest::testGenerateCryptoMessages()
{
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
    KMime::Types::Mailbox mb;
    mb.from7BitString("to@to.example");
    recipientsEditor.setRecipientString({mb}, Recipient::To);
    editor.setPlainText(QStringLiteral("Hello,\n\nThis is a test message\n\nGreez"));

    bool wasCanceled = false;
    auto composers = composerViewBase.generateCryptoMessages(wasCanceled);
    QCOMPARE(wasCanceled, false);

    QCOMPARE(composers.size(), 1); // No additional composers are created

    auto composer = composers.first();
    composerViewBase.fillComposer(composer);

    VERIFYEXEC(composer);

    QCOMPARE(composer->resultMessages().size(), 1);

    auto msg = composer->resultMessages().first();
    msg->messageID()->from7BitString("<test@autotest.example>");
    msg->date()->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    msg->assemble();
    Test::compareFile(msg.data(), QStringLiteral(MAIL_DATA_DIR "/autosave.mbox"));
}

QTEST_MAIN(ComposerViewBaseTest)
