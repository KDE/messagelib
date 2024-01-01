/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class QStandardItemModel;
class FollowupReminderSelectDateDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit FollowupReminderSelectDateDialogTest(QObject *parent = nullptr);
    ~FollowupReminderSelectDateDialogTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldDisableOkButtonIfDateIsEmpty();
    void shouldDisableOkButtonIfDateIsNotValid();
    void shouldDisableOkButtonIfModelIsEmpty();

private:
    QStandardItemModel *defaultItemModel();
};
