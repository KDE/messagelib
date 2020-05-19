/*
   Copyright (C) 2014-2020 Laurent Montel <montel@kde.org>

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

#include "sendlaterinfotest.h"
#include "sendlater/sendlaterinfo.h"

#include <QTest>
#include <QDateTime>
#include <QStandardPaths>

SendLaterInfoTest::SendLaterInfoTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void SendLaterInfoTest::shouldHaveDefaultValue()
{
    MessageComposer::SendLaterInfo info;
    QCOMPARE(info.itemId(), Akonadi::Item::Id(-1));
    QCOMPARE(info.isRecurrence(), false);
    QVERIFY(!info.dateTime().isValid());
    QVERIFY(!info.lastDateTimeSend().isValid());
    QCOMPARE(info.to(), QString());
    QCOMPARE(info.subject(), QString());
    QVERIFY(!info.isValid());
    QCOMPARE(info.recurrenceUnit(), MessageComposer::SendLaterInfo::Days);
    QCOMPARE(info.recurrenceEachValue(), 1);
}

void SendLaterInfoTest::shouldNotValidIfIdIsNotValid()
{
    MessageComposer::SendLaterInfo info;
    const QString to = QStringLiteral("kde.org");
    info.setTo(to);
    info.setSubject(QStringLiteral("Subject"));
    info.setRecurrence(true);
    info.setRecurrenceEachValue(5);
    info.setRecurrenceUnit(MessageComposer::SendLaterInfo::Years);
    const QDate date(2014, 1, 1);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    info.setDateTime(QDateTime(date));
    info.setLastDateTimeSend(QDateTime(date));
#else
    info.setDateTime(QDateTime(date.startOfDay()));
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
#endif
    QVERIFY(!info.isValid());
}

void SendLaterInfoTest::shouldNotValidIfDateIsNotValid()
{
    MessageComposer::SendLaterInfo info;
    const QString to = QStringLiteral("kde.org");
    info.setTo(to);
    info.setItemId(Akonadi::Item::Id(42));
    info.setSubject(QStringLiteral("Subject"));
    info.setRecurrence(true);
    info.setRecurrenceEachValue(5);
    info.setRecurrenceUnit(MessageComposer::SendLaterInfo::Years);
    const QDate date(2014, 1, 1);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    info.setLastDateTimeSend(QDateTime(date));
#else
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
#endif
    QVERIFY(!info.isValid());
}

void SendLaterInfoTest::shouldCopyInfo()
{
    MessageComposer::SendLaterInfo info;
    const QString to = QStringLiteral("kde.org");
    info.setTo(to);
    info.setItemId(Akonadi::Item::Id(42));
    info.setSubject(QStringLiteral("Subject"));
    info.setRecurrence(true);
    info.setRecurrenceEachValue(5);
    info.setRecurrenceUnit(MessageComposer::SendLaterInfo::Years);
    const QDate date(2014, 1, 1);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    info.setDateTime(QDateTime(date));
    info.setLastDateTimeSend(QDateTime(date));
#else
    info.setDateTime(QDateTime(date.startOfDay()));
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
#endif
    MessageComposer::SendLaterInfo copyInfo(info);
    QCOMPARE(info, copyInfo);
}

QTEST_MAIN(SendLaterInfoTest)
