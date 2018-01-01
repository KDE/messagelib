/*
   Copyright (C) 2014-2018 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef FOLLOWUPREMINDERSELECTDATEDIALOG_H
#define FOLLOWUPREMINDERSELECTDATEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <AkonadiCore/Collection>
#include "messagecomposer_export.h"
class QAbstractItemModel;

namespace MessageComposer {
class FollowUpReminderSelectDateDialogPrivate;
class MESSAGECOMPOSER_EXPORT FollowUpReminderSelectDateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FollowUpReminderSelectDateDialog(QWidget *parent = nullptr, QAbstractItemModel *model = nullptr);
    ~FollowUpReminderSelectDateDialog();

    QDate selectedDate() const;

    void accept() override;

    Akonadi::Collection collection() const;
private Q_SLOTS:
    void slotDateChanged();
    void updateOkButton();
private:
    FollowUpReminderSelectDateDialogPrivate *const d;
};
}

#endif // FOLLOWUPREMINDERSELECTDATEDIALOG_H
