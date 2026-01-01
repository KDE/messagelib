/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "richtextcomposerngtest.h"
#include "../richtextcomposerng.h"
#include "../richtextcomposersignatures.h"

#include <KActionCollection>
#include <KPIMTextEdit/RichTextComposerControler>

#include <QStandardPaths>
#include <QTest>
#include <TextAutoCorrectionCore/AutoCorrection>

using namespace Qt::Literals::StringLiterals;

RichTextComposerNgTest::RichTextComposerNgTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

RichTextComposerNgTest::~RichTextComposerNgTest() = default;

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
    // FIXME first char !
    QTest::newRow("test1") << u"boo bla bli. foo faa"_s << QStringLiteral("boo bla bli. Foo faa");
    QTest::newRow("test2") << u"boo bla bli.\nfoo faa"_s << QStringLiteral("boo bla bli.\nFoo faa");
    QTest::newRow("test3") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nBoo bla bli.\nFoo faa");
}

void RichTextComposerNgTest::shouldForceAutoCorrection()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);

    MessageComposer::RichTextComposerNg richtextComposerNg;

    richtextComposerNg.setPlainText(original);
    TextAutoCorrectionCore::AutoCorrection autocorrection;
    auto settings = new TextAutoCorrectionCore::AutoCorrectionSettings;
    settings->setEnabledAutoCorrection(true);
    settings->setUppercaseFirstCharOfSentence(true);
    autocorrection.setAutoCorrectionSettings(settings);
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
    QTest::newRow("noselection") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nBoo bla bli.\nFoo faa") << 0 << 0;
    QTest::newRow("noselection2") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nBoo bla bli.\nFoo faa") << 1 << 1;
    QTest::newRow("fullselection") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nBoo bla bli.\nFoo faa") << 0 << 21;
    QTest::newRow("selection1") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nBoo bla bli.\nfoo faa") << 0 << 10;
    QTest::newRow("selection2") << u"\nboo bla bli.\nfoo faa"_s << QStringLiteral("\nboo bla bli.\nfoo faa") << 5 << 10;
    QTest::newRow("twouppercase") << u"\nBOo bla bli.\nfoo FAa"_s << QStringLiteral("\nBoo bla bli.\nFoo Faa") << 0 << 21;
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
    TextAutoCorrectionCore::AutoCorrection autocorrection;
    auto settings = new TextAutoCorrectionCore::AutoCorrectionSettings;
    settings->setEnabledAutoCorrection(true);
    settings->setUppercaseFirstCharOfSentence(true);
    settings->setFixTwoUppercaseChars(true);
    autocorrection.setAutoCorrectionSettings(settings);
    richtextComposerNg.setAutocorrection(&autocorrection);
    richtextComposerNg.forceAutoCorrection(true);

    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldNotChangeSignatureWhenOriginalAndNewSignatureAreSame()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    KIdentityManagementCore::Signature oldSignature;

    const bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(oldSignature, oldSignature);
    QVERIFY(!replaceSignature);
}

Q_DECLARE_METATYPE(KIdentityManagementCore::Signature::Placement)
Q_DECLARE_METATYPE(KIdentityManagementCore::Signature::AddedTextFlag)
Q_DECLARE_METATYPE(KIdentityManagementCore::Signature::Type)

void RichTextComposerNgTest::shouldAddSignature_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<KIdentityManagementCore::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagementCore::Signature::AddedTextFlag>("signatureaddtext");
    QTest::newRow("startandaddseparator") << u"foo bla, bli\nbb"_s << QStringLiteral("-- \nSignaturefoo bla, bli\nbb")
                                          << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddSeparator;

    QTest::newRow("startandnewline") << u"foo bla, bli\nbb"_s << QStringLiteral("\n\nSignature\nfoo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                     << KIdentityManagementCore::Signature::AddNewLines;

    QTest::newRow("startandnothing") << u"foo bla, bli\nbb"_s << QStringLiteral("Signaturefoo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                     << KIdentityManagementCore::Signature::AddNothing;

    QTest::newRow("endandaddseparator") << u"foo bla, bli\nbb"_s << QStringLiteral("foo bla, bli\nbb-- \nSignature") << KIdentityManagementCore::Signature::End
                                        << KIdentityManagementCore::Signature::AddSeparator;

    QTest::newRow("endandnewline") << u"foo bla, bli\nbb"_s << QStringLiteral("foo bla, bli\nbb\nSignature") << KIdentityManagementCore::Signature::End
                                   << KIdentityManagementCore::Signature::AddNewLines;

    QTest::newRow("endandnothing") << u"foo bla, bli\nbb"_s << QStringLiteral("foo bla, bli\nbbSignature") << KIdentityManagementCore::Signature::End
                                   << KIdentityManagementCore::Signature::AddNothing;

    // TODO test "Add Cursor"
}

void RichTextComposerNgTest::shouldAddSignature()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    QFETCH(KIdentityManagementCore::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagementCore::Signature::AddedTextFlag, signatureaddtext);
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.setPlainText(original);

    KIdentityManagementCore::Signature newSignature(u"Signature"_s);
    newSignature.setEnabledSignature(true);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldAddSpecificSignature_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::addColumn<KIdentityManagementCore::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagementCore::Signature::AddedTextFlag>("signatureaddtext");
    QTest::addColumn<bool>("enablesignature");
    QTest::addColumn<bool>("signaturehtml");
    QTest::newRow("startandaddseparatordisablenonhtml")
        << u"foo bla, bli\nbb"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
        << KIdentityManagementCore::Signature::AddSeparator << false << false;

    QTest::newRow("startandaddseparatordisablehtml") << u"foo bla, bli\nbb"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                                     << KIdentityManagementCore::Signature::AddSeparator << false << true;

    QTest::newRow("startandaddseparatorenablehtml") << u"foo bla, bli\nbb"_s << QStringLiteral("-- \nSignaturefoo bla, bli\nbb")
                                                    << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddSeparator << true
                                                    << true;
}

void RichTextComposerNgTest::shouldAddSpecificSignature()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    QFETCH(KIdentityManagementCore::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagementCore::Signature::AddedTextFlag, signatureaddtext);
    QFETCH(bool, enablesignature);
    QFETCH(bool, signaturehtml);
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    richtextComposerNg.setPlainText(original);

    KIdentityManagementCore::Signature newSignature(u"Signature"_s);
    newSignature.setEnabledSignature(enablesignature);
    newSignature.setInlinedHtml(signaturehtml);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    // qDebug() << " clean html "<<richtextComposerNg.toCleanHtml();
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldReplaceSignature_data()
{
    QTest::addColumn<QString>("signatureText");
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagementCore::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagementCore::Signature::AddedTextFlag>("signatureaddtext");

    // Add Separator AtEnd
    QTest::newRow("newlinebody") << u"Signature"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::End
                                 << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("emptybody") << u"Signature"_s << QString() << KIdentityManagementCore::Signature::End << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("spacebody") << u"Signature"_s << QStringLiteral(" ") << KIdentityManagementCore::Signature::End
                               << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("simple") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                            << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewline") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                 << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewlineatbegin") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                        << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewlineatbeginandend") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                              << KIdentityManagementCore::Signature::AddSeparator;

    // Add separator AtStart
    QTest::newRow("newlinebody-2") << u"Signature"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::Start
                                   << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("emptybody-2") << u"Signature"_s << QString() << KIdentityManagementCore::Signature::Start
                                 << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("spacebody-2") << u"Signature"_s << QStringLiteral(" ") << KIdentityManagementCore::Signature::Start
                                 << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("simple-2") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                              << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewline-2") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                   << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewlineatbegin-2") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                          << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("withnewlineatbeginandend-2") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddSeparator;

    // Add nothing End
    // FIXME
    // QTest::newRow("newlinebody-3") << u"Signature"_s << QStringLiteral("\n")
    //                             << KIdentityManagementCore::Signature::End << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("emptybody-3") << u"Signature"_s << QString() << KIdentityManagementCore::Signature::End << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("spacebody-3") << u"Signature"_s << QStringLiteral(" ") << KIdentityManagementCore::Signature::End
                                 << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("simple-3") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewline-3") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                   << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewlineatbegin-3") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                          << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewlineatbeginandend-3") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                                << KIdentityManagementCore::Signature::AddNothing;

    // Add nothing Start
    QTest::newRow("newlinebody-4") << u"Signature"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::Start
                                   << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("emptybody-4") << u"Signature"_s << QString() << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("spacebody-4") << u"Signature"_s << QStringLiteral(" ") << KIdentityManagementCore::Signature::Start
                                 << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("simple-4") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                              << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewline-4") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                   << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewlineatbegin-4") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::Start
                                          << KIdentityManagementCore::Signature::AddNothing;
    QTest::newRow("withnewlineatbeginandend-4") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb")
                                                << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNothing;

    // Add newline End
    QTest::newRow("emptybody-5") << u"Signature"_s << QString() << KIdentityManagementCore::Signature::End << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("newlinebody-5") << u"Signature"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::End
                                   << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("spacebody-5") << u"Signature"_s << QStringLiteral(" ") << KIdentityManagementCore::Signature::End
                                 << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("simple-5") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewline-5") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                   << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewlineatbegin-5") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                          << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewlineatbeginandend-5") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb") << KIdentityManagementCore::Signature::End
                                                << KIdentityManagementCore::Signature::AddNewLines;
#if 0 // Need to fix it.
      // Add newline start
    QTest::newRow("emptybody-6") << u"Signature"_s << QString()
                                 << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("newlinebody-6") << u"Signature"_s << QStringLiteral("\n")
                                   << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("spacebody-6") << u"Signature"_s << QStringLiteral(" ")
                                 << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("simple-6") << u"Signature"_s << QStringLiteral("foo bla, bli\nbb")
                              << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewline-6") << u"Signature\nnew line"_s << QStringLiteral("foo bla, bli\nbb")
                                   << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewlineatbegin-6") << u"\nSignature\nnew line"_s << QStringLiteral("foo bla, bli\nbb")
                                          << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
    QTest::newRow("withnewlineatbeginandend6") << u"\nSignature\nnew line\n"_s << QStringLiteral("foo bla, bli\nbb")
                                               << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
#endif
}

void RichTextComposerNgTest::shouldReplaceSignature()
{
    QFETCH(QString, signatureText);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagementCore::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagementCore::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagementCore::Signature newSignature(signatureText);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagementCore::Signature::AddNothing:
        break;
    case KIdentityManagementCore::Signature::AddSeparator:
        addText = u"-- \n"_s;
        break;
    case KIdentityManagementCore::Signature::AddNewLines:
        addText = u"\n"_s;
        break;
    }

    QString expected;
    switch (signatureplacement) {
    case KIdentityManagementCore::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagementCore::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagementCore::Signature::AtCursor:
        break;
    }
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagementCore::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    // When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagementCore::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldLoadSignatureFromFile_data()
{
    QTest::addColumn<QString>("signatureFile");
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagementCore::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagementCore::Signature::AddedTextFlag>("signatureaddtext");

    QTest::newRow("signature1") << u"signature1.txt"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::End
                                << KIdentityManagementCore::Signature::AddSeparator;

    QTest::newRow("signature2") << u"signature2.txt"_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::End
                                << KIdentityManagementCore::Signature::AddSeparator;
}

void RichTextComposerNgTest::shouldLoadSignatureFromFile()
{
    QFETCH(QString, signatureFile);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagementCore::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagementCore::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagementCore::Signature newSignature(QLatin1StringView(SIGNATURE_DATA_DIR) + u'/' + signatureFile, false);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagementCore::Signature::AddNothing:
        break;
    case KIdentityManagementCore::Signature::AddSeparator:
        addText = u"-- \n"_s;
        break;
    case KIdentityManagementCore::Signature::AddNewLines:
        addText = u"\n"_s;
        break;
    }

    QString expected;
    QString signatureText = newSignature.toPlainText();
    QVERIFY(!signatureText.isEmpty());
    switch (signatureplacement) {
    case KIdentityManagementCore::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagementCore::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagementCore::Signature::AtCursor:
        break;
    }

#ifdef Q_OS_WIN
    expected.replace(u'\n', u"\r\n"_s);
#endif

    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagementCore::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    // When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagementCore::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::shouldLoadSignatureFromCommand_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("bodytext");
    QTest::addColumn<KIdentityManagementCore::Signature::Placement>("signatureplacement");
    QTest::addColumn<KIdentityManagementCore::Signature::AddedTextFlag>("signatureaddtext");

    QTest::newRow("command1") << u"echo \"foo\""_s << QStringLiteral("\n") << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("command2") << u"echo \"foo\""_s << QStringLiteral("foo ddd \n") << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("command3") << u"echo \"foo\""_s << QString() << KIdentityManagementCore::Signature::End << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("command4") << u"echo \"foo\nsss\""_s << QString() << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddSeparator;
    QTest::newRow("command5") << u"echo \"foo\nsss\n\""_s << QStringLiteral("foo ddd \n") << KIdentityManagementCore::Signature::End
                              << KIdentityManagementCore::Signature::AddSeparator;

    // Start
    QTest::newRow("command6") << u"echo \"foo\nsss\n\""_s << QStringLiteral("foo ddd \n") << KIdentityManagementCore::Signature::Start
                              << KIdentityManagementCore::Signature::AddSeparator;

    // Failed
    // QTest::newRow("command7") << u"echo \"foo\nsss\n\""_s << QStringLiteral("foo ddd \n")
    //                             << KIdentityManagementCore::Signature::Start << KIdentityManagementCore::Signature::AddNewLines;
}

void RichTextComposerNgTest::shouldLoadSignatureFromCommand()
{
    QFETCH(QString, command);
    QFETCH(QString, bodytext);
    QFETCH(KIdentityManagementCore::Signature::Placement, signatureplacement);
    QFETCH(KIdentityManagementCore::Signature::AddedTextFlag, signatureaddtext);

    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.createActions(new KActionCollection(this));
    const QString original(bodytext);
    richtextComposerNg.setPlainText(original);

    KIdentityManagementCore::Signature newSignature(command, true);
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    QString addText;
    switch (signatureaddtext) {
    case KIdentityManagementCore::Signature::AddNothing:
        break;
    case KIdentityManagementCore::Signature::AddSeparator:
        addText = u"-- \n"_s;
        break;
    case KIdentityManagementCore::Signature::AddNewLines:
        addText = u"\n"_s;
        break;
    }

    QString expected;
    QString signatureText = newSignature.toPlainText();
    QVERIFY(!signatureText.isEmpty());
    switch (signatureplacement) {
    case KIdentityManagementCore::Signature::Start:
        expected = addText + signatureText + bodytext;
        break;
    case KIdentityManagementCore::Signature::End:
        expected = bodytext + addText + signatureText;
        break;
    case KIdentityManagementCore::Signature::AtCursor:
        break;
    }

#ifdef Q_OS_WIN
    expected.replace(u'\n', u"\r\n"_s);
#endif

    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);

    KIdentityManagementCore::Signature emptySignature;

    bool replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(newSignature, emptySignature);
    QVERIFY(replaceSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), original);

    replaceSignature = richtextComposerNg.composerSignature()->replaceSignature(emptySignature, newSignature);
    QVERIFY(!replaceSignature);
    // When signature is empty we can't replace it.=> we need to insertSignature

    //=> insertSignature(signature, KIdentityManagementCore::Signature::End, addedText);
    richtextComposerNg.insertSignature(newSignature, signatureplacement, signatureaddtext);
    QCOMPARE(richtextComposerNg.toPlainText(), expected);
}

void RichTextComposerNgTest::fixHtmlFontSizeTest()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    QString str = QStringLiteral(
        "<span style=\"color: green; font-size: 10pt;\">int font</span> size\n"
        "<span style=\"font-size: 10.8pt; font-family: foo;\">double font</span> size\n"
        "<span style=\"font-size: 15,2pt;\">invalid font</span> size, left as is\n");

    richtextComposerNg.fixHtmlFontSize(str);
    const QString expected(
        QStringLiteral("<span style=\"color: green; font-size:0.83em;\">int font</span> size\n"
                       "<span style=\"font-size:0.9em; font-family: foo;\">double font</span> size\n"
                       "<span style=\"font-size: 15,2pt;\">invalid font</span> size, left as is\n"));

    QCOMPARE(str, expected);
}

void RichTextComposerNgTest::toCleanHtmlRegexTest()
{
    MessageComposer::RichTextComposerNg richtextComposerNg;
    richtextComposerNg.setHtml(
        QStringLiteral("<p style=\"-qt-paragraph-type:empty\"></p>\n"
                       "<p>some text</p>\n"
                       "<p style=\"-qt-paragraph-type:empty\"></p>\n"));

    const QString result = richtextComposerNg.toCleanHtml();
    const int idx = result.indexOf(u"<p style=\"-qt-paragraph-type:empty"_s);
    // Remove the XHTML DOCTYPE and <style> parts as they differ
    // based on the fonts on the system
    const QString resultStripped = result.mid(idx);

    const QString expected = QStringLiteral(
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; "
        "\">&nbsp;</p>\n"
        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">some text</p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; "
        "\">&nbsp;</p></body></html>");

    QCOMPARE(resultStripped, expected);
}

QTEST_MAIN(RichTextComposerNgTest)

#include "moc_richtextcomposerngtest.cpp"
