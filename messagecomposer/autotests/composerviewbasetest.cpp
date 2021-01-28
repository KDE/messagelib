/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "composerviewbasetest.h"
#include "../src/composer/composerviewbase.h"
#include <QTest>

ComposerViewBaseTest::ComposerViewBaseTest(QObject *parent)
    : QObject(parent)
{
}

ComposerViewBaseTest::~ComposerViewBaseTest()
{
}

void ComposerViewBaseTest::shouldHaveDefaultValue()
{
    MessageComposer::ComposerViewBase composerViewBase;
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
}

QTEST_MAIN(ComposerViewBaseTest)
