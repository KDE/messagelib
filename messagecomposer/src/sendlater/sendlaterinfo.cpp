/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterinfo.h"

using namespace MessageComposer;

bool SendLaterInfo::isValid() const
{
    return (mId != -1) && mDateTime.isValid();
}

bool SendLaterInfo::isRecurrence() const
{
    return mRecurrence;
}

void SendLaterInfo::setRecurrence(bool b)
{
    mRecurrence = b;
}

void SendLaterInfo::setRecurrenceUnit(SendLaterInfo::RecurrenceUnit unit)
{
    mRecurrenceUnit = unit;
}

SendLaterInfo::RecurrenceUnit SendLaterInfo::recurrenceUnit() const
{
    return mRecurrenceUnit;
}

void SendLaterInfo::setRecurrenceEachValue(int value)
{
    mRecurrenceEachValue = value;
}

int SendLaterInfo::recurrenceEachValue() const
{
    return mRecurrenceEachValue;
}

void SendLaterInfo::setItemId(Akonadi::Item::Id id)
{
    mId = id;
}

Akonadi::Item::Id SendLaterInfo::itemId() const
{
    return mId;
}

void SendLaterInfo::setDateTime(const QDateTime &time)
{
    mDateTime = time;
}

QDateTime SendLaterInfo::dateTime() const
{
    return mDateTime;
}

void SendLaterInfo::setLastDateTimeSend(const QDateTime &dateTime)
{
    mLastDateTimeSend = dateTime;
}

QDateTime SendLaterInfo::lastDateTimeSend() const
{
    return mLastDateTimeSend;
}

void SendLaterInfo::setSubject(const QString &subject)
{
    mSubject = subject;
}

QString SendLaterInfo::subject() const
{
    return mSubject;
}

void SendLaterInfo::setTo(const QString &to)
{
    mTo = to;
}

QString SendLaterInfo::to() const
{
    return mTo;
}

bool SendLaterInfo::operator==(const SendLaterInfo &other) const
{
    return (itemId() == other.itemId()) && (recurrenceUnit() == other.recurrenceUnit()) && (recurrenceEachValue() == other.recurrenceEachValue())
        && (isRecurrence() == other.isRecurrence()) && (dateTime() == other.dateTime()) && (lastDateTimeSend() == other.lastDateTimeSend())
        && (subject() == other.subject()) && (to() == other.to());
}

QDebug operator<<(QDebug d, const SendLaterInfo &info)
{
    d << "mTo: " << info.to();
    d << "mSubject: " << info.subject();
    d << "mDateTime: " << info.dateTime().toString();
    d << "mLastDateTimeSend: " << info.lastDateTimeSend().toString();
    d << "mId: " << info.itemId();
    d << "mRecurrenceEachValue: " << info.recurrenceEachValue();
    d << "mRecurrenceUnit: " << info.recurrenceUnit();
    d << "mRecurrence: " << info.isRecurrence();
    return d;
}
