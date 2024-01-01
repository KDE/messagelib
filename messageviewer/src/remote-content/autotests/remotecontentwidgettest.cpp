/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidgettest.h"
#include "remote-content/remotecontentstatustypecombobox.h"
#include "remote-content/remotecontentwidget.h"
#include <QHBoxLayout>
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
    auto mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mLineEdit = w.findChild<QLineEdit *>(QStringLiteral("mLineEdit"));
    QVERIFY(mLineEdit);
    QVERIFY(mLineEdit->text().isEmpty());

    auto mStatusComboBox = w.findChild<MessageViewer::RemoteContentStatusTypeComboBox *>(QStringLiteral("mStatusComboBox"));
    QVERIFY(mStatusComboBox);
}

#include "moc_remotecontentwidgettest.cpp"
