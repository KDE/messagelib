/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_SENDLATERDIALOG_H
#define MESSAGECOMPOSER_SENDLATERDIALOG_H

#include <QDialog>
#include <QDateTime>

#include "messagecomposer_export.h"

class QCheckBox;
class QPushButton;
namespace Ui {
class SendLaterWidget;
}

namespace MessageComposer {
class SendLaterInfo;

/** Send later dialog. */
class MESSAGECOMPOSER_EXPORT SendLaterDialog : public QDialog
{
    Q_OBJECT
public:
    enum SendLaterAction {
        Unknown = 0,
        SendDeliveryAtTime = 1,
        Canceled = 2,
        PutInOutbox = 3
    };

    explicit SendLaterDialog(SendLaterInfo *info, QWidget *parent = nullptr);
    ~SendLaterDialog() override;

    SendLaterInfo *info();

    Q_REQUIRED_RESULT SendLaterAction action() const;

private:
    void slotRecurrenceClicked(bool);
    void slotOkClicked();
    void slotDelay(bool delayEnabled);

    void slotDateChanged(const QString &date);
    void load(SendLaterInfo *info);

private:
    QDateTime mSendDateTime;
    SendLaterAction mAction = Unknown;
    QCheckBox *mDelay = nullptr;
    Ui::SendLaterWidget *mSendLaterWidget = nullptr;
    SendLaterInfo *mInfo = nullptr;
    QPushButton *mOkButton = nullptr;
};
}
#endif // SENDLATERDIALOG_H
