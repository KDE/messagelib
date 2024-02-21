/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <Akonadi/Collection>

#include <QDialog>
#include <QPushButton>

#include <memory>

class QAbstractItemModel;

namespace MessageComposer
{
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

    [[nodiscard]] QDate selectedDate() const;

    void accept() override;

    [[nodiscard]] Akonadi::Collection collection() const;

private:
    MESSAGECOMPOSER_NO_EXPORT void slotDateChanged();
    MESSAGECOMPOSER_NO_EXPORT void updateOkButton();

private:
    std::unique_ptr<FollowUpReminderSelectDateDialogPrivate> const d;
};
}
