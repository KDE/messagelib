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

#ifndef MESSAGECOMPOSER_SENDLATERINFO_H
#define MESSAGECOMPOSER_SENDLATERINFO_H

#include <AkonadiCore/Item>
#include <QDateTime>

#include "messagecomposer_export.h"

namespace MessageComposer {
/** Send later information. */
class MESSAGECOMPOSER_EXPORT SendLaterInfo
{
public:
    explicit SendLaterInfo() = default;

    enum RecurrenceUnit {
        Days = 0,
        Weeks,
        Months,
        Years
    };

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

    bool operator ==(const SendLaterInfo &other) const;
    inline bool operator !=(const SendLaterInfo &other) const
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

#endif // SENDLATERINFO_H
