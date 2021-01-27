/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidgettest.h"
#include "remote-content/remotecontentwidget.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTest>
QTEST_MAIN(RemoteContentWidgetTest)
RemoteContentWidgetTest::RemoteContentWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void RemoteContentWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentWidget w;
    auto mainLayout = w.findChild<QFormLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), {});

    auto mLineEdit = w.findChild<QLineEdit *>(QStringLiteral("mLineEdit"));
    QVERIFY(mLineEdit);
    QVERIFY(mLineEdit->text().isEmpty());

    auto mStatusCheckBox = w.findChild<QCheckBox *>(QStringLiteral("mStatusCheckBox"));
    QVERIFY(mStatusCheckBox);
}
