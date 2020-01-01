/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "shownextmessagewidgettest.h"
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

    QHBoxLayout *mainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    //mainLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *mPreviousMessage = w.findChild<QPushButton *>(QStringLiteral("previous_message"));
    QVERIFY(mPreviousMessage);
    QVERIFY(!mPreviousMessage->text().isEmpty());
    QVERIFY(!mPreviousMessage->isEnabled());

    QPushButton *mNextMessage = w.findChild<QPushButton *>(QStringLiteral("next_message"));
    QVERIFY(mNextMessage);
    QVERIFY(!mNextMessage->text().isEmpty());
    QVERIFY(!mNextMessage->isEnabled());
}
