/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtextcomposersignaturestest.h"
#include "../richtextcomposerng.h"
#include "../richtextcomposersignatures.h"
#include <KActionCollection>
#include <QStandardPaths>
#include <QTest>

QTEST_MAIN(RichTextComposerSignaturesTest)

RichTextComposerSignaturesTest::RichTextComposerSignaturesTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void RichTextComposerSignaturesTest::shouldCleanSignature()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    MessageComposer::RichTextComposerSignatures *composerSignature = richtextComposerNg.composerSignature();
    QVERIFY(composerSignature);

    KIdentityManagementCore::Signature signature1;
    signature1.setText(QStringLiteral("bla      Bla\t"));
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagementCore::Signature signature2(QStringLiteral("Signature"));
    signature2.setText(QStringLiteral("Foo      Bla\t"));
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagementCore::Signature::Start, KIdentityManagementCore::Signature::AddNewLines);
    composerSignature->cleanWhitespace(signature2);

    QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    QCOMPARE(richtextComposerNg.toPlainText(), QStringLiteral("\n\nbla Bla "));

    for (int i = 0; i < 10; i++) {
        QVERIFY(!composerSignature->replaceSignature(signature2, signature1));
        QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    }
    QCOMPARE(richtextComposerNg.toPlainText(), QStringLiteral("\n\nbla Bla "));
}

void RichTextComposerSignaturesTest::shouldReplaceSignatureWhenText()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.setText(QStringLiteral("foo\nbla  \nfoo"));
    richtextComposerNg.createActions(new KActionCollection(this));
    MessageComposer::RichTextComposerSignatures *composerSignature = richtextComposerNg.composerSignature();
    QVERIFY(composerSignature);

    KIdentityManagementCore::Signature signature1;
    signature1.setText(QStringLiteral("bla      Bla\t"));
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagementCore::Signature signature2(QStringLiteral("Signature"));
    signature2.setText(QStringLiteral("Foo      Bla\t"));
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagementCore::Signature::End, KIdentityManagementCore::Signature::AddSeparator);
    composerSignature->cleanWhitespace(signature2);

    QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    const QString result = QStringLiteral("foo\nbla\nfoo--\nbla Bla ");
    QCOMPARE(richtextComposerNg.toPlainText(), result);

    for (int i = 0; i < 10; i++) {
        QVERIFY(!composerSignature->replaceSignature(signature2, signature1));
        QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    }
    QCOMPARE(richtextComposerNg.toPlainText(), result);
}

#include "moc_richtextcomposersignaturestest.cpp"
