/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterdialogtest.h"
#include "sendlater/sendlaterdialog.h"

#include <KDateComboBox>
#include <KTimeComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardPaths>
#include <QTest>

SendLaterDialogTest::SendLaterDialogTest(QObject *parent)
    : QObject(parent)
{
}

SendLaterDialogTest::~SendLaterDialogTest() = default;

void SendLaterDialogTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void SendLaterDialogTest::shouldHaveDefaultValue()
{
    MessageComposer::SendLaterDialog dlg(nullptr);
    auto timeCombo = dlg.findChild<KTimeComboBox *>(QStringLiteral("time_sendlater"));
    QVERIFY(timeCombo);
    auto dateCombo = dlg.findChild<KDateComboBox *>(QStringLiteral("date_sendlater"));
    QVERIFY(dateCombo);
    QVERIFY(dateCombo->date().isValid());
    auto okButton = dlg.findChild<QPushButton *>(QStringLiteral("okbutton"));
    QVERIFY(okButton);
    QVERIFY(okButton->isEnabled());
    dateCombo->lineEdit()->clear();
    QVERIFY(!dateCombo->date().isValid());
    QVERIFY(!okButton->isEnabled());
}

QTEST_MAIN(SendLaterDialogTest)

#include "moc_sendlaterdialogtest.cpp"
