/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "adblockaddsubscriptiondialogtest.h"
#include "../adblockaddsubscriptiondialog.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTest>

AdBlockAddSubscriptionDialogTest::AdBlockAddSubscriptionDialogTest(QObject *parent)
    : QObject(parent)
{

}

AdBlockAddSubscriptionDialogTest::~AdBlockAddSubscriptionDialogTest()
{

}

void AdBlockAddSubscriptionDialogTest::shouldHaveDefaultValue()
{
    QStringList lst;
    MessageViewer::AdBlockAddSubscriptionDialog w(lst);


    QLabel *lab = w.findChild<QLabel *>(QStringLiteral("listsubscriptionlabel"));
    QVERIFY(lab);

    QComboBox *listSubscription = w.findChild<QComboBox *>(QStringLiteral("listsubscriptioncombobox"));
    QVERIFY(listSubscription);
    QVERIFY(listSubscription->count()>0);

    QDialogButtonBox *buttonBox = w.findChild<QDialogButtonBox *>(QStringLiteral("listsubscriptionbuttonBox"));
    QVERIFY(buttonBox);
}

QTEST_MAIN(AdBlockAddSubscriptionDialogTest)
