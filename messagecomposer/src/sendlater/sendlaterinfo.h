/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <AkonadiCore/Item>
#include <QDateTime>

#include "messagecomposer_export.h"

namespace MessageComposer
{
/** Send later information. */
class MESSAGECOMPOSER_EXPORT SendLaterInfo
{
public:
    explicit SendLaterInfo() = default;

    enum RecurrenceUnit { Days = 0, Weeks, Months, Years };

    Q_REQUIRED_RESULT bool isValid() const;

    void setItemId(Akonadi::Item::Id id);
    Q_REQUIRED_RESULT Akonadi::Item::Id itemId() const;

    void setRecurrenceUnit(RecurrenceUnit unit);
    Q_REQUIRED_RESULT RecurrenceUnit recurrenceUnit() const;

    void setRecurrenceEachValue(int value);
    Q_REQUIRED_RESULT int recurrenceEachValue() const;

    Q_REQUIRED_RESULT bool isRecurrence() const;
    void setRecurrence(bool b);

    void setDateTime(const QDateTime &time);
    Q_REQUIRED_RESULT QDateTime dateTime() const;

    void setLastDateTimeSend(const QDateTime &date);
    Q_REQUIRED_RESULT QDateTime lastDateTimeSend() const;

    void setSubject(const QString &subject);
    Q_REQUIRED_RESULT QString subject() const;

    void setTo(const QString &to);
    Q_REQUIRED_RESULT QString to() const;

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

