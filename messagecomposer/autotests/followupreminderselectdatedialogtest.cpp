/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "followupreminderselectdatedialogtest.h"
#include "../src/followupreminder/followupreminderselectdatedialog.h"
#include <QTest>
#include <QLineEdit>
#include <KDateComboBox>
#include <AkonadiWidgets/CollectionComboBox>

#include <QLineEdit>
#include <QPushButton>
#include <AkonadiCore/EntityTreeModel>
#include <QStandardItemModel>
#include <KCalendarCore/Todo>

FollowupReminderSelectDateDialogTest::FollowupReminderSelectDateDialogTest(QObject *parent)
    : QObject(parent)
{
}

FollowupReminderSelectDateDialogTest::~FollowupReminderSelectDateDialogTest()
{
}

QStandardItemModel *FollowupReminderSelectDateDialogTest::defaultItemModel()
{
    QStandardItemModel *model = new QStandardItemModel;
    for (int id = 42; id < 51; ++id) {
        Akonadi::Collection collection(id);
        collection.setRights(Akonadi::Collection::AllRights);
        collection.setName(QString::number(id));
        collection.setContentMimeTypes(QStringList() << KCalendarCore::Todo::todoMimeType());

        QStandardItem *item = new QStandardItem(collection.name());
        item->setData(QVariant::fromValue(collection),
                      Akonadi::EntityTreeModel::CollectionRole);
        item->setData(QVariant::fromValue(collection.id()),
                      Akonadi::EntityTreeModel::CollectionIdRole);

        model->appendRow(item);
    }
    return model;
}

void FollowupReminderSelectDateDialogTest::shouldHaveDefaultValue()
{
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, defaultItemModel());
    KDateComboBox *datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);

    Akonadi::CollectionComboBox *combobox = dlg.findChild<Akonadi::CollectionComboBox *>(QStringLiteral("collectioncombobox"));
    QVERIFY(combobox);
    QDate currentDate = QDate::currentDate();
    QCOMPARE(datecombobox->date(), currentDate.addDays(1));

    QPushButton *okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(okButton);
    QVERIFY(okButton->isEnabled());
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfDateIsEmpty()
{
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, defaultItemModel());
    KDateComboBox *datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    QPushButton *okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(okButton->isEnabled());
    datecombobox->lineEdit()->clear();
    QVERIFY(!okButton->isEnabled());
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfDateIsNotValid()
{
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, defaultItemModel());
    KDateComboBox *datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    datecombobox->lineEdit()->setText(QStringLiteral(" "));
    QPushButton *okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(!okButton->isEnabled());
    const QDate date = QDate::currentDate();
    datecombobox->setDate(date);
    QVERIFY(okButton->isEnabled());
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfModelIsEmpty()
{
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, new QStandardItemModel(nullptr));
    KDateComboBox *datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    QPushButton *okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(!okButton->isEnabled());

    datecombobox->lineEdit()->setText(QStringLiteral(" "));
    QVERIFY(!okButton->isEnabled());
    const QDate date = QDate::currentDate();
    datecombobox->setDate(date);
    QVERIFY(!okButton->isEnabled());
}

QTEST_MAIN(FollowupReminderSelectDateDialogTest)
