/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeydialogtest.h"
#include "dkim-verify/dkimmanagerkeydialog.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTest>
#include <QStandardPaths>

QTEST_MAIN(DKIMManagerKeyDialogTest)

DKIMManagerKeyDialogTest::DKIMManagerKeyDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

DKIMManagerKeyDialogTest::~DKIMManagerKeyDialogTest()
{
}

void DKIMManagerKeyDialogTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMManagerKeyDialog dlg;

    QVBoxLayout *mainLayout = dlg.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);

    MessageViewer::DKIMManagerKeyWidget *w = dlg.findChild<MessageViewer::DKIMManagerKeyWidget *>(QStringLiteral("managerWidget"));
    QVERIFY(w);

    QDialogButtonBox *buttonBox = dlg.findChild<QDialogButtonBox *>(QStringLiteral("buttonbox"));
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), {QDialogButtonBox::Close});
}
