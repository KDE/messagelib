/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertooldialogtest.h"
using namespace Qt::Literals::StringLiterals;

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
    auto mainLayout = d.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);

    auto mDeveloperToolWidget = d.findChild<WebEngineViewer::DeveloperToolWidget *>(u"mDeveloperToolWidget"_s);
    QVERIFY(mDeveloperToolWidget);

    auto buttonBox = d.findChild<QDialogButtonBox *>(u"buttonBox"_s);
    QVERIFY(buttonBox);
}

#include "moc_developertooldialogtest.cpp"
