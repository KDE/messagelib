/*
   SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentwidgettest.h"
using namespace Qt::Literals::StringLiterals;

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
    auto mainLayout = w.findChild<QHBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mLineEdit = w.findChild<QLineEdit *>(u"mLineEdit"_s);
    QVERIFY(mLineEdit);
    QVERIFY(mLineEdit->text().isEmpty());

    auto mStatusComboBox = w.findChild<MessageViewer::RemoteContentStatusTypeComboBox *>(u"mStatusComboBox"_s);
    QVERIFY(mStatusComboBox);
}

#include "moc_remotecontentwidgettest.cpp"
