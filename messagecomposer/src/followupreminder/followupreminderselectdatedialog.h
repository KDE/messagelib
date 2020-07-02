/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_FOLLOWUPREMINDERSELECTDATEDIALOG_H
#define MESSAGECOMPOSER_FOLLOWUPREMINDERSELECTDATEDIALOG_H

#include "messagecomposer_export.h"

#include <AkonadiCore/Collection>

#include <QDialog>
#include <QPushButton>

#include <memory>

class QAbstractItemModel;

namespace MessageComposer {
class FollowUpReminderSelectDateDialogPrivate;
/**
 * @brief The FollowUpReminderSelectDateDialog class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT FollowUpReminderSelectDateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FollowUpReminderSelectDateDialog(QWidget *parent = nullptr, QAbstractItemModel *model = nullptr);
    ~FollowUpReminderSelectDateDialog() override;

    Q_REQUIRED_RESULT QDate selectedDate() const;

    void accept() override;

    Akonadi::Collection collection() const;

private Q_SLOTS:
    void slotDateChanged();
    void updateOkButton();

private:
    std::unique_ptr<FollowUpReminderSelectDateDialogPrivate> const d;
};
}

#endif
