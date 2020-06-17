/*
   Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

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
    ~SendLaterDialog();

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
