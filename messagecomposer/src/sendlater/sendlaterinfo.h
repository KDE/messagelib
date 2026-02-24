/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Item>
#include <QDateTime>

#include "messagecomposer_export.h"

namespace MessageComposer
{
/*!
 * \class MessageComposer::SendLaterInfo
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/SendLaterInfo
 * \brief Send later information.
 */
class MESSAGECOMPOSER_EXPORT SendLaterInfo
{
public:
    explicit SendLaterInfo() = default;

    enum RecurrenceUnit : uint8_t {
        Days = 0,
        Weeks,
        Months,
        Years,
    };

    /*! \brief Returns whether the send later info contains valid data. */
    [[nodiscard]] bool isValid() const;

    /*! \brief Sets the Akonadi item ID for the message.
        \param id The Akonadi item ID.
    */
    void setItemId(Akonadi::Item::Id id);
    /*! \brief Returns the Akonadi item ID for the message. */
    [[nodiscard]] Akonadi::Item::Id itemId() const;

    /*! \brief Sets the recurrence unit (days, weeks, months, or years).
        \param unit The recurrence unit.
    */
    void setRecurrenceUnit(RecurrenceUnit unit);
    /*! \brief Returns the recurrence unit. */
    [[nodiscard]] RecurrenceUnit recurrenceUnit() const;

    /*! \brief Sets how many recurrence units between each send.
        \param value The number of units between sends.
    */
    void setRecurrenceEachValue(int value);
    /*! \brief Returns how many recurrence units between each send. */
    [[nodiscard]] int recurrenceEachValue() const;

    /*! \brief Returns whether the message should recur. */
    [[nodiscard]] bool isRecurrence() const;
    /*! \brief Sets whether the message should recur.
        \param b True to enable recurrence, false otherwise.
    */
    void setRecurrence(bool b);

    /*! \brief Sets the date and time to send the message.
        \param time The scheduled send time.
    */
    void setDateTime(const QDateTime &time);
    /*! \brief Returns the scheduled send date and time. */
    [[nodiscard]] QDateTime dateTime() const;

    /*! \brief Sets the date and time when the message was last sent (for recurrence).
        \param date The last send date and time.
    */
    void setLastDateTimeSend(const QDateTime &date);
    /*! \brief Returns the date and time when the message was last sent. */
    [[nodiscard]] QDateTime lastDateTimeSend() const;

    /*! \brief Sets the subject of the scheduled message.
        \param subject The message subject.
    */
    void setSubject(const QString &subject);
    /*! \brief Returns the subject of the scheduled message. */
    [[nodiscard]] QString subject() const;

    void setTo(const QString &to);
    [[nodiscard]] QString to() const;

    bool operator==(const SendLaterInfo &other) const;
    inline bool operator!=(const SendLaterInfo &other) const
    {
        return !(*this == other);
    }

private:
    QString mTo;
    QString mSubject;
    QDateTime mDateTime;
    QDateTime mLastDateTimeSend;
    Akonadi::Item::Id mId = -1;
    int mRecurrenceEachValue = 1;
    RecurrenceUnit mRecurrenceUnit = Days;
    bool mRecurrence = false;
};
}

MESSAGECOMPOSER_EXPORT QDebug operator<<(QDebug debug, const MessageComposer::SendLaterInfo &info);
