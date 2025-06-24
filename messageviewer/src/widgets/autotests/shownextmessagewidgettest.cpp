/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "shownextmessagewidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "widgets/shownextmessagewidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QTest>
QTEST_MAIN(ShowNextMessageWidgetTest)
ShowNextMessageWidgetTest::ShowNextMessageWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void ShowNextMessageWidgetTest::shouldHaveDefaultValue()
{
    MessageViewer::ShowNextMessageWidget w;

    auto mainLayout = w.findChild<QHBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);
    // mainLayout->setContentsMargins({});

    auto mPreviousMessage = w.findChild<QPushButton *>(u"previous_message"_s);
    QVERIFY(mPreviousMessage);
    QVERIFY(!mPreviousMessage->text().isEmpty());
    QVERIFY(!mPreviousMessage->isEnabled());

    auto mNextMessage = w.findChild<QPushButton *>(u"next_message"_s);
    QVERIFY(mNextMessage);
    QVERIFY(!mNextMessage->text().isEmpty());
    QVERIFY(!mNextMessage->isEnabled());
}

#include "moc_shownextmessagewidgettest.cpp"
