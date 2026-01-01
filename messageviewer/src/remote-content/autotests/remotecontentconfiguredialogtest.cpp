/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfiguredialogtest.h"
using namespace Qt::Literals::StringLiterals;

#include "remote-content/remotecontentconfiguredialog.h"
#include "remote-content/remotecontentconfigurewidget.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(RemoteContentConfigureDialogTest)
RemoteContentConfigureDialogTest::RemoteContentConfigureDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void RemoteContentConfigureDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentConfigureDialog w;

    QVERIFY(!w.windowTitle().isEmpty());

    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);

    auto mRemoteContentConfigureWidget = w.findChild<MessageViewer::RemoteContentConfigureWidget *>(u"mRemoteContentConfigureWidget"_s);
    QVERIFY(mRemoteContentConfigureWidget);

    auto buttonBox = w.findChild<QDialogButtonBox *>(u"buttonBox"_s);
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Close});
}

#include "moc_remotecontentconfiguredialogtest.cpp"
