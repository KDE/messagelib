/*
   Copyright (C) 2016-2018 Montel Laurent <montel@kde.org>

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
    QTest::newRow("fullselection") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nFoo faa") << 0 << 21;
    QTest::newRow("selection1") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nBoo bla bli.\nfoo faa") << 0 << 10;
    QTest::newRow("selection2") << QStringLiteral("\nboo bla bli.\nfoo faa") << QStringLiteral("\nboo bla bli.\nfoo faa") << 5 << 10;
    QTest::newRow("twouppercase") << QStringLiteral("\nBOo bla bli.\nfoo FAa") << QStringLiteral("\nBoo bla bli.\nFoo Faa") << 0 << 21;
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
Q_DECLARE_METATYPE(KIdentityManagement::Signature::Type)

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
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagement::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagement::Signature::AddedTextFlag>("signatureaddtext");

    //Add Separator AtEnd
    QTest::newRow("newlinebody") << QStringLiteral("Signature") << QStringLiteral("\n")
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("emptybody") << QStringLiteral("Signature") << QString()
                               << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("spacebody") << QStringLiteral("Signature") << QStringLiteral(" ")
                               << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("simple") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                            << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewline") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewlineatbegin") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                        << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewlineatbeginandend") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;

    //Add separator AtStart
    QTest::newRow("newlinebody-2") << QStringLiteral("Signature") << QStringLiteral("\n")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("emptybody-2") << QStringLiteral("Signature") << QString()
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("spacebody-2") << QStringLiteral("Signature") << QStringLiteral(" ")
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("simple-2") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewline-2") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewlineatbegin-2") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("withnewlineatbeginandend-2") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;

    //Add nothing End
    //FIXME
    //QTest::newRow("newlinebody-3") << QStringLiteral("Signature") << QStringLiteral("\n")
    //                             << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("emptybody-3") << QStringLiteral("Signature") << QString()
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("spacebody-3") << QStringLiteral("Signature") << QStringLiteral(" ")
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("simple-3") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewline-3") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewlineatbegin-3") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewlineatbeginandend-3") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNothing;

    //Add nothing Start
    QTest::newRow("newlinebody-4") << QStringLiteral("Signature") << QStringLiteral("\n")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("emptybody-4") << QStringLiteral("Signature") << QString()
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("spacebody-4") << QStringLiteral("Signature") << QStringLiteral(" ")
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("simple-4") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewline-4") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewlineatbegin-4") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;
    QTest::newRow("withnewlineatbeginandend-4") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNothing;

    //Add newline End
    QTest::newRow("emptybody-5") << QStringLiteral("Signature") << QString()
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("newlinebody-5") << QStringLiteral("Signature") << QStringLiteral("\n")
                                   << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("spacebody-5") << QStringLiteral("Signature") << QStringLiteral(" ")
                                 << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("simple-5") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewline-5") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewlineatbegin-5") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewlineatbeginandend-5") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddNewLines;
#if 0 //Need to fix it.
      //Add newline start
    QTest::newRow("emptybody-6") << QStringLiteral("Signature") << QString()
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("newlinebody-6") << QStringLiteral("Signature") << QStringLiteral("\n")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("spacebody-6") << QStringLiteral("Signature") << QStringLiteral(" ")
                                 << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("simple-6") << QStringLiteral("Signature") << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewline-6") << QStringLiteral("Signature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewlineatbegin-6") << QStringLiteral("\nSignature\nnew line") << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
    QTest::newRow("withnewlineatbeginandend6") << QStringLiteral("\nSignature\nnew line\n") << QStringLiteral("foo bla, bli\nbb")
                                               << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
#endif
}

void RichTextComposerNgTest::shouldReplaceSignature()
{
    QFETCH(QString, signatureText);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagement::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagement::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(signatureText);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagement::Signature::AddNothing:
        break;
    case KIdentityManagement::Signature::AddSeparator:
        addText = QStringLiteral("-- \n");
        break;
    case KIdentityManagement::Signature::AddNewLines:
        addText = QStringLiteral("\n");
        break;
    }

    QString expected;
    switch (signatureplacement) {
    case KIdentityManagement::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagement::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagement::Signature::AtCursor:
        break;
    }
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagement::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    //When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagement::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldLoadSignatureFromFile_data()
{
    QTest::addColumn<QString>("signatureFile");
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagement::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagement::Signature::AddedTextFlag>("signatureaddtext");

    QTest::newRow("signature1") << QStringLiteral("signature1.txt") << QStringLiteral("\n")
                                << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;

    QTest::newRow("signature2") << QStringLiteral("signature2.txt") << QStringLiteral("\n")
                                << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
}

void RichTextComposerNgTest::shouldLoadSignatureFromFile()
{
    QFETCH(QString, signatureFile);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagement::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagement::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(QLatin1String(SIGNATURE_DATA_DIR) + QLatin1Char('/') + signatureFile, false);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagement::Signature::AddNothing:
        break;
    case KIdentityManagement::Signature::AddSeparator:
        addText = QStringLiteral("-- \n");
        break;
    case KIdentityManagement::Signature::AddNewLines:
        addText = QStringLiteral("\n");
        break;
    }

    QString expected;
    QString signatureText = newSignature.toPlainText();
    QVERIFY(!signatureText.isEmpty());
    switch (signatureplacement) {
    case KIdentityManagement::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagement::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagement::Signature::AtCursor:
        break;
    }

    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagement::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    //When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagement::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldLoadSignatureFromCommand_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagement::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagement::Signature::AddedTextFlag>("signatureaddtext");

    QTest::newRow("command1") << QStringLiteral("echo \"foo\"") << QStringLiteral("\n")
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("command2") << QStringLiteral("echo \"foo\"") << QStringLiteral("foo ddd \n")
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("command3") << QStringLiteral("echo \"foo\"") << QString()
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("command4") << QStringLiteral("echo \"foo\nsss\"") << QString()
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;
    QTest::newRow("command5") << QStringLiteral("echo \"foo\nsss\n\"") << QStringLiteral("foo ddd \n")
                              << KIdentityManagement::Signature::End << KIdentityManagement::Signature::AddSeparator;

    //Start
    QTest::newRow("command6") << QStringLiteral("echo \"foo\nsss\n\"") << QStringLiteral("foo ddd \n")
                              << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddSeparator;

    //Failed
    //QTest::newRow("command7") << QStringLiteral("echo \"foo\nsss\n\"") << QStringLiteral("foo ddd \n")
    //                             << KIdentityManagement::Signature::Start << KIdentityManagement::Signature::AddNewLines;
}

void RichTextComposerNgTest::shouldLoadSignatureFromCommand()
{
    QFETCH(QString, command);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagement::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagement::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagement::Signature newSignature(command, true);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagement::Signature::AddNothing:
        break;
    case KIdentityManagement::Signature::AddSeparator:
        addText = QStringLiteral("-- \n");
        break;
    case KIdentityManagement::Signature::AddNewLines:
        addText = QStringLiteral("\n");
        break;
    }

    QString expected;
    QString signatureText = newSignature.toPlainText();
    QVERIFY(!signatureText.isEmpty());
    switch (signatureplacement) {
    case KIdentityManagement::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagement::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagement::Signature::AtCursor:
        break;
    }

    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagement::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    //When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagement::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

QTEST_MAIN(RichTextComposerNgTest)
