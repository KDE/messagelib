/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeydialogtest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimmanagerkeydialog.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>

QTEST_MAIN(DKIMManagerKeyDialogTest)

DKIMManagerKeyDialogTest::DKIMManagerKeyDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

DKIMManagerKeyDialogTest::~DKIMManagerKeyDialogTest() = default;

void DKIMManagerKeyDialogTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMManagerKeyDialog dlg;

    auto mainLayout = dlg.findChild<QVBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);

    auto w = dlg.findChild<MessageViewer::DKIMManagerKeyWidget *>(u"managerWidget"_s);
    QVERIFY(w);

    auto buttonBox = dlg.findChild<QDialogButtonBox *>(u"buttonbox"_s);
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Close});
}

#include "moc_dkimmanagerkeydialogtest.cpp"
