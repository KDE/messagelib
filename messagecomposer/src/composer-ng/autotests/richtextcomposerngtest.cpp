/*
   Copyright (C) 2016 Montel Laurent <montel@kde.org>

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


#include "richtextcomposerngtest.h"
#include "../richtextcomposerng.h"
#include "../richtextcomposersignatures.h"

#include <KPIMTextEdit/RichTextComposerControler>
#include <KActionCollection>

#include <PimCommon/AutoCorrection>
#include <QTest>

RichTextComposerNgTest::RichTextComposerNgTest(QObject *parent)
    : QObject(parent)
{

}

RichTextComposerNgTest::~RichTextComposerNgTest()
{

}

void RichTextComposerNgTest::shouldHaveDefaultValue()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    QVERIFY(richtextComposerNg.composerSignature());
    QVERIFY(!richtextComposerNg.autocorrection());
    QVERIFY(richtextComposerNg.composerControler());
    QVERIFY(richtextComposerNg.composerControler()->composerImages());
}


void RichTextComposerNgTest::shouldForceAutoCorrection_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    //FIXME first char !
    QTest::newRow("test1") << QStringLiteral("boo bla bli. foo faa") << QStringLiteral("boo bla bli. Foo faa");
    QTest::newRow("test2") << QStringLiteral("boo bla bli.\nfoo faa") << QStringLiteral("boo bla bli.\nFoo faa");
    QTest::newRow("test3") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nFoo faa");

}

void RichTextComposerNgTest::shouldForceAutoCorrection()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);

    MessageComposer::RichTextComposerNg richtextComposerNg;

    richtextComposerNg.setPlainText(original);
    PimCommon::AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setUppercaseFirstCharOfSentence(true);
    richtextComposerNg.setAutocorrection(&autocorrection);
    richtextComposerNg.forceAutoCorrection(false);

    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldForceAutoCorrectionWithSelection_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<int>("selectionStart");
    QTest::addColumn<int>("selectionEnd");
    QTest::newRow("noselection") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nFoo faa") << 0 << 0;
    QTest::newRow("noselection2") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nFoo faa") << 1 << 1;
    QTest::newRow("fullselection") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nFoo faa") << 0 << 22;
    QTest::newRow("selection1") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nfoo faa") << 0 << 10;
    QTest::newRow("selection2") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nboo bla bli.\nfoo faa") << 5 << 10;
    QTest::newRow("twouppercase") << QStringLiteral("\nBOo bla bli.\nfoo FAa") << QStringLiteral("\nBoo bla bli.\nFoo Faa") << 0 << 22;
}

void RichTextComposerNgTest::shouldForceAutoCorrectionWithSelection()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    QFETCH(int, selectionStart);
    QFETCH(int, selectionEnd);

    MessageComposer::RichTextComposerNg richtextComposerNg;

    richtextComposerNg.setPlainText(original);
    QTextCursor cur = richtextComposerNg.textCursor();
    cur.setPosition(selectionStart);
    cur.setPosition(selectionEnd, QTextCursor::KeepAnchor);
    richtextComposerNg.setTextCursor(cur);
    PimCommon::AutoCorrection autocorrection;
    autocorrection.setEnabledAutoCorrection(true);
    autocorrection.setUppercaseFirstCharOfSentence(true);
    autocorrection.setFixTwoUppercaseChars(true);
    richtextComposerNg.setAutocorrection(&autocorrection);
    richtextComposerNg.forceAutoCorrection(true);

    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldNotChangeSignatureWhenOriginalAndNewSignatureAreSame()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    KIdentityManagement::Signature oldSignature;

    const bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(oldSignature, oldSignature);
    QVERIFY(!replaceSignature);
}
Q_DECLARE_METATYPE(KIdentityManagement::Signature::Placement)
Q_DECLARE_METATYPE(KIdentityManagement::Signature::AddedTextFlag)

void RichTextComposerNgTest::shouldAddSignature_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<KIdentityManagement::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagement::Signature::AddedTextFlag>("signatureaddtext");
    QTest::newRow("startandaddseparator") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("-- \nSignaturefoo bla, bli\nbb")
                                          << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;

    QTest::newRow("startandnewline") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("\n\nSignature\nfoo bla, bli\nbb")
                                     << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;

    QTest::newRow("startandnothing") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("Signaturefoo bla, bli\nbb")
                                     << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;

    QTest::newRow("endandaddseparator") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("foo bla, bli\nbb-- \nSignature")
                                        << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;

    QTest::newRow("endandnewline") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("foo bla, bli\nbb\nSignature")
                                   << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;

    QTest::newRow("endandnothing") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("foo bla, bli\nbbSignature")
                                   << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;

    //TODO test "Add Cursor"
}

void RichTextComposerNgTest::shouldAddSignature()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    QFETCH(KIdentityManagement::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagement::Signature::AddedTextFlag, signatureaddtext);
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(QStringLiteral("Signature"));
    newSignature.setEnabledSignature(true);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldAddSpecificSignature_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<KIdentityManagement::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagement::Signature::AddedTextFlag>("signatureaddtext");
    QTest::addColumn<bool>("enablesignature");
    QTest::addColumn<bool>("signaturehtml");
    QTest::newRow("startandaddseparatordisablenonhtml") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("foo bla, bli\nbb")
                                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator
                                                 << false << false;

    QTest::newRow("startandaddseparatordisablehtml") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("foo bla, bli\nbb")
                                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator
                                                 << false << true;

    QTest::newRow("startandaddseparatorenablehtml") << QStringLiteral("foo bla, bli\nbb") << QStringLiteral("-- \nSignaturefoo bla, bli\nbb")
                                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator
                                                 << true << true;
}

void RichTextComposerNgTest::shouldAddSpecificSignature()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    QFETCH(KIdentityManagement::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagement::Signature::AddedTextFlag, signatureaddtext);
    QFETCH(bool, enablesignature);
    QFETCH(bool, signaturehtml);
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(QStringLiteral("Signature"));
    newSignature.setEnabledSignature(enablesignature);
    newSignature.setInlinedHtml(signaturehtml);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    //qDebug() << " clean html "<<richtextComposerNg.toCleanHtml();
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldReplaceSignature_data()
{
    QTest::addColumn<QString>("signatureText");
    QTest::newRow("simple") << QStringLiteral("Signature");
    QTest::newRow("withnewline") << QStringLiteral("Signature\nnew line");
    QTest::newRow("withnewlineatbegin") << QStringLiteral("\nSignature\nnew line");
}

void RichTextComposerNgTest::shouldReplaceSignature()
{
    QFETCH(QString, signatureText);
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(QStringLiteral("foo bla, bli\nbb"));
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(signatureText);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString expected = QStringLiteral("foo bla, bli\nbb-- \n") + signatureText;
    richtextComposerNg.insertSignature(newSignature, KIdentityManagement::Signature::End, KIdentityManagement::Signature::AddSeparator);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagement::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    //When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagement::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, KIdentityManagement::Signature::End, KIdentityManagement::Signature::AddSeparator);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

QTEST_MAIN(RichTextComposerNgTest)
