/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QDateTime>
#include <QDialog>

#include "messagecomposer_export.h"

class QCheckBox;
class QPushButton;
namespace Ui
{
class SendLaterWidget;
}

namespace MessageComposer
{
class SendLaterInfo;

/*! \class SendLaterDialog
    \inheaderfile sendlaterdialog.h
    \brief Dialog for configuring delayed message sending.
    \inmodule MessageComposer

    This dialog allows users to configure when and how a message should be sent later,
    including options for recurrence and delay settings. It manages the SendLaterInfo
    that controls the delayed sending behavior.
*/
class MESSAGECOMPOSER_EXPORT SendLaterDialog : public QDialog
{
    Q_OBJECT
public:
    /*! \enum SendLaterAction
        Actions that can be performed in the send-later dialog.
    */
    enum SendLaterAction : uint8_t {
        Unknown = 0, /*!< Unknown or no action taken. */
        SendDeliveryAtTime = 1, /*!< Send the message at a specified time. */
        Canceled = 2, /*!< The dialog was canceled. */
        PutInOutbox = 3, /*!< Put the message in the outbox without delay. */
    };

    /*! \brief Constructs a SendLaterDialog for configuring delayed sending.
        \param info Pointer to the SendLaterInfo object containing delay settings.
        \param parent The parent widget.
    */
    explicit SendLaterDialog(SendLaterInfo *info, QWidget *parent = nullptr);

    /*! \brief Destroys the SendLaterDialog. */
    ~SendLaterDialog() override;

    /*! \brief Returns a pointer to the SendLaterInfo object configured by this dialog. */
    [[nodiscard]] SendLaterInfo *info();

    /*! \brief Returns the action selected by the user.
        \return The SendLaterAction corresponding to the user's choice.
    */
    [[nodiscard]] SendLaterAction action() const;

private:
    MESSAGECOMPOSER_NO_EXPORT void slotRecurrenceClicked(bool);
    MESSAGECOMPOSER_NO_EXPORT void slotOkClicked();
    MESSAGECOMPOSER_NO_EXPORT void slotDelay(bool delayEnabled);

    MESSAGECOMPOSER_NO_EXPORT void slotDateChanged(const QString &date);
    MESSAGECOMPOSER_NO_EXPORT void load(SendLaterInfo *info);

    QDateTime mSendDateTime;
    SendLaterAction mAction = Unknown;
    QCheckBox *mDelay = nullptr;
    Ui::SendLaterWidget *const mSendLaterWidget;
    SendLaterInfo *mInfo = nullptr;
    QPushButton *mOkButton = nullptr;
};
}
