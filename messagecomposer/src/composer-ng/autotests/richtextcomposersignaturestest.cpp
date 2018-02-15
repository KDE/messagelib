/*
   Copyright (C) 2018 Montel Laurent <montel@kde.org>

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

    KIdentityManagement::Signature emptySignature;
    emptySignature.setText(QStringLiteral("bla      Bla\t"));
    emptySignature.setEnabledSignature(true);
    emptySignature.setInlinedHtml(false);

    KIdentityManagement::Signature newSignature(QStringLiteral("Signature"));
    newSignature.setText(QStringLiteral("Foo      Bla\t"));
    newSignature.setEnabledSignature(true);
    newSignature.setInlinedHtml(false);

    richtextComposerNg.insertSignature(emptySignature, KIdentityManagement::Signature::Start, KIdentityManagement::Signature::AddNewLines);
    composerSignature->cleanWhitespace(newSignature);


    composerSignature->replaceSignature(emptySignature, newSignature);
    QCOMPARE(richtextComposerNg.toPlainText(), QStringLiteral("\n\nbla Bla "));

}
