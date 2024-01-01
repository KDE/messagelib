/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "followupreminderselectdatedialogtest.h"
#include <Akonadi/CollectionComboBox>
#include <KDateComboBox>
#include <MessageComposer/FollowUpReminderSelectDateDialog>
#include <QLineEdit>
#include <QTest>

#include <Akonadi/EntityTreeModel>
#include <KCalendarCore/Todo>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>

FollowupReminderSelectDateDialogTest::FollowupReminderSelectDateDialogTest(QObject *parent)
    : QObject(parent)
{
}

FollowupReminderSelectDateDialogTest::~FollowupReminderSelectDateDialogTest() = default;

QStandardItemModel *FollowupReminderSelectDateDialogTest::defaultItemModel()
{
    auto model = new QStandardItemModel;
    for (int id = 42; id < 51; ++id) {
        Akonadi::Collection collection(id);
        collection.setRights(Akonadi::Collection::AllRights);
        collection.setName(QString::number(id));
        collection.setContentMimeTypes(QStringList() << KCalendarCore::Todo::todoMimeType());

        auto item = new QStandardItem(collection.name());
        item->setData(QVariant::fromValue(collection), Akonadi::EntityTreeModel::CollectionRole);
        item->setData(QVariant::fromValue(collection.id()), Akonadi::EntityTreeModel::CollectionIdRole);

        model->appendRow(item);
    }
    return model;
}

void FollowupReminderSelectDateDialogTest::shouldHaveDefaultValue()
{
    auto model = defaultItemModel();
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, model);
    auto datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);

    auto combobox = dlg.findChild<Akonadi::CollectionComboBox *>(QStringLiteral("collectioncombobox"));
    QVERIFY(combobox);
    QDate currentDate = QDate::currentDate();
    QCOMPARE(datecombobox->date(), currentDate.addDays(1));

    auto okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(okButton);
    QVERIFY(okButton->isEnabled());
    delete model;
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfDateIsEmpty()
{
    auto model = defaultItemModel();
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, model);
    auto datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    auto okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(okButton->isEnabled());
    datecombobox->lineEdit()->clear();
    QVERIFY(!okButton->isEnabled());
    delete model;
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfDateIsNotValid()
{
    auto model = defaultItemModel();
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, model);
    auto datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    datecombobox->lineEdit()->setText(QStringLiteral(" "));
    auto okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(!okButton->isEnabled());
    const QDate date = QDate::currentDate();
    datecombobox->setDate(date);
    QVERIFY(okButton->isEnabled());
    delete model;
}

void FollowupReminderSelectDateDialogTest::shouldDisableOkButtonIfModelIsEmpty()
{
    std::unique_ptr<QStandardItemModel> model(new QStandardItemModel(nullptr));
    MessageComposer::FollowUpReminderSelectDateDialog dlg(nullptr, model.get());
    auto datecombobox = dlg.findChild<KDateComboBox *>(QStringLiteral("datecombobox"));
    QVERIFY(datecombobox);
    auto okButton = dlg.findChild<QPushButton *>(QStringLiteral("ok_button"));
    QVERIFY(!okButton->isEnabled());

    datecombobox->lineEdit()->setText(QStringLiteral(" "));
    QVERIFY(!okButton->isEnabled());
    const QDate date = QDate::currentDate();
    datecombobox->setDate(date);
    QVERIFY(!okButton->isEnabled());
}

QTEST_MAIN(FollowupReminderSelectDateDialogTest)

#include "moc_followupreminderselectdatedialogtest.cpp"
