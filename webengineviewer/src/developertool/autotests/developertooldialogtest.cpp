/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertooldialogtest.h"
#include "developertool/developertooldialog.h"
#include "developertool/developertoolwidget.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(DeveloperToolDialogTest)
DeveloperToolDialogTest::DeveloperToolDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DeveloperToolDialogTest::shouldHaveDefaultValues()
{
    WebEngineViewer::DeveloperToolDialog d;
    auto mainLayout = d.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);

    auto mDeveloperToolWidget = d.findChild<WebEngineViewer::DeveloperToolWidget *>(QStringLiteral("mDeveloperToolWidget"));
    QVERIFY(mDeveloperToolWidget);

    auto buttonBox = d.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
    QVERIFY(buttonBox);
}

#include "moc_developertooldialogtest.cpp"
