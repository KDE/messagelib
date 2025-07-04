/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterdialogtest.h"
using namespace Qt::Literals::StringLiterals;

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
    auto timeCombo = dlg.findChild<KTimeComboBox *>(u"time_sendlater"_s);
    QVERIFY(timeCombo);
    auto dateCombo = dlg.findChild<KDateComboBox *>(u"date_sendlater"_s);
    QVERIFY(dateCombo);
    QVERIFY(dateCombo->date().isValid());
    auto okButton = dlg.findChild<QPushButton *>(u"okbutton"_s);
    QVERIFY(okButton);
    QVERIFY(okButton->isEnabled());
    dateCombo->lineEdit()->clear();
    QVERIFY(!dateCombo->date().isValid());
    QVERIFY(!okButton->isEnabled());
}

QTEST_MAIN(SendLaterDialogTest)

#include "moc_sendlaterdialogtest.cpp"
