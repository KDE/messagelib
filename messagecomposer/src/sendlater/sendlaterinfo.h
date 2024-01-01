/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Item>
#include <QDateTime>

#include "messagecomposer_export.h"

namespace MessageComposer
{
/** Send later information. */
class MESSAGECOMPOSER_EXPORT SendLaterInfo
{
public:
    explicit SendLaterInfo() = default;

    enum RecurrenceUnit {
        Days = 0,
        Weeks,
        Months,
        Years,
    };

    [[nodiscard]] bool isValid() const;

    void setItemId(Akonadi::Item::Id id);
    [[nodiscard]] Akonadi::Item::Id itemId() const;

    void setRecurrenceUnit(RecurrenceUnit unit);
    [[nodiscard]] RecurrenceUnit recurrenceUnit() const;

    void setRecurrenceEachValue(int value);
    [[nodiscard]] int recurrenceEachValue() const;

    [[nodiscard]] bool isRecurrence() const;
    void setRecurrence(bool b);

    void setDateTime(const QDateTime &time);
    [[nodiscard]] QDateTime dateTime() const;

    void setLastDateTimeSend(const QDateTime &date);
    [[nodiscard]] QDateTime lastDateTimeSend() const;

    void setSubject(const QString &subject);
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
