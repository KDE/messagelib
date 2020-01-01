/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "richtextcomposersignaturestest.h"
#include "../richtextcomposerng.h"
#include "../richtextcomposersignatures.h"
#include <KActionCollection>
#include <QTest>

QTEST_MAIN(RichTextComposerSignaturesTest)

RichTextComposerSignaturesTest::RichTextComposerSignaturesTest(QObject *parent)
    : QObject(parent)
{
}

void RichTextComposerSignaturesTest::shouldCleanSignature()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    MessageComposer::RichTextComposerSignatures *composerSignature = richtextComposerNg.composerSignature();
    QVERIFY(composerSignature);

    KIdentityManagement::Signature signature1;
    signature1.setText(QStringLiteral("bla      Bla\t"));
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagement::Signature signature2(QStringLiteral("Signature"));
    signature2.setText(QStringLiteral("Foo      Bla\t"));
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagement::Signature::Start, KIdentityManagement::Signature::AddNewLines);
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

    KIdentityManagement::Signature signature1;
    signature1.setText(QStringLiteral("bla      Bla\t"));
    signature1.setEnabledSignature(true);
    signature1.setInlinedHtml(false);

    KIdentityManagement::Signature signature2(QStringLiteral("Signature"));
    signature2.setText(QStringLiteral("Foo      Bla\t"));
    signature2.setEnabledSignature(true);
    signature2.setInlinedHtml(false);

    richtextComposerNg.insertSignature(signature1, KIdentityManagement::Signature::End, KIdentityManagement::Signature::AddSeparator);
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
