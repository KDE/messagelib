/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertooldialogtest.h"
#include "widgets/developertooldialog.h"
#include "widgets/developertoolwidget.h"
#include <QTest>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QDialogButtonBox>
QTEST_MAIN(DeveloperToolDialogTest)
DeveloperToolDialogTest::DeveloperToolDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DeveloperToolDialogTest::shouldHaveDefaultValues()
{
   MessageViewer::DeveloperToolDialog d;
   auto *mainLayout = d.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
   QVERIFY(mainLayout);

   auto *mDeveloperToolWidget = d.findChild<MessageViewer::DeveloperToolWidget *>(QStringLiteral("mDeveloperToolWidget"));
   QVERIFY(mDeveloperToolWidget);

   auto *buttonBox = d.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
   QVERIFY(buttonBox);
}
