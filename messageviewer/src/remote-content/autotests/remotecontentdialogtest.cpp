/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentdialogtest.h"
#include "remote-content/remotecontentdialog.h"
#include "remote-content/remotecontentwidget.h"
#include <QDialogButtonBox>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(RemoteContentDialogTest)

RemoteContentDialogTest::RemoteContentDialogTest(QObject *parent)
    : QObject(parent)
{
}

void RemoteContentDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentDialog w;

    QVERIFY(w.windowTitle().isEmpty());

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);

    auto mRemoveContentWidget = w.findChild<MessageViewer::RemoteContentWidget *>(QStringLiteral("mRemoveContentWidget"));
    QVERIFY(mRemoveContentWidget);

    auto buttonBox = w.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Ok | QDialogButtonBox::Cancel});
}

#include "moc_remotecontentdialogtest.cpp"
