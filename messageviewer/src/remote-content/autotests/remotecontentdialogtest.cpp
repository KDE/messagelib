/*
   SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentdialogtest.h"
using namespace Qt::Literals::StringLiterals;

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

    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);

    auto mRemoveContentWidget = w.findChild<MessageViewer::RemoteContentWidget *>(u"mRemoveContentWidget"_s);
    QVERIFY(mRemoveContentWidget);

    auto buttonBox = w.findChild<QDialogButtonBox *>(u"buttonBox"_s);
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Ok | QDialogButtonBox::Cancel});
}

#include "moc_remotecontentdialogtest.cpp"
