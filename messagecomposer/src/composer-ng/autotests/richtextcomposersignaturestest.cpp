/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtextcomposersignaturestest.h"
using namespace Qt::Literals::StringLiterals;

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
    signature1.setText(u"bla      Bla\t"_s);
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagementCore::Signature signature2(u"Signature"_s);
    signature2.setText(u"Foo      Bla\t"_s);
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagementCore::Signature::Start, KIdentityManagementCore::Signature::AddNewLines);
    composerSignature->cleanWhitespace(signature2);

    QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    QCOMPARE(richtextComposerNg.toPlainText(), u"\n\nbla Bla "_s);

    for (int i = 0; i < 10; i++) {
        QVERIFY(!composerSignature->replaceSignature(signature2, signature1));
        QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    }
    QCOMPARE(richtextComposerNg.toPlainText(), u"\n\nbla Bla "_s);
}

void RichTextComposerSignaturesTest::shouldReplaceSignatureWhenText()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.setText(u"foo\nbla  \nfoo"_s);
    richtextComposerNg.createActions(new KActionCollection(this));
    MessageComposer::RichTextComposerSignatures *composerSignature = richtextComposerNg.composerSignature();
    QVERIFY(composerSignature);

    KIdentityManagementCore::Signature signature1;
    signature1.setText(u"bla      Bla\t"_s);
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagementCore::Signature signature2(u"Signature"_s);
    signature2.setText(u"Foo      Bla\t"_s);
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagementCore::Signature::End, KIdentityManagementCore::Signature::AddSeparator);
    composerSignature->cleanWhitespace(signature2);

    QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    const QString result = u"foo\nbla\nfoo--\nbla Bla "_s;
    QCOMPARE(richtextComposerNg.toPlainText(), result);

    for (int i = 0; i < 10; i++) {
        QVERIFY(!composerSignature->replaceSignature(signature2, signature1));
        QVERIFY(!composerSignature->replaceSignature(signature1, signature2));
    }
    QCOMPARE(richtextComposerNg.toPlainText(), result);
}

#include "moc_richtextcomposersignaturestest.cpp"
