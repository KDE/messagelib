/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterinfotest.h"
#include "sendlater/sendlaterinfo.h"

#include <QDateTime>
#include <QStandardPaths>
#include <QTest>

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
    info.setDateTime(QDateTime(date.startOfDay()));
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
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
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
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
    info.setDateTime(QDateTime(date.startOfDay()));
    info.setLastDateTimeSend(QDateTime(date.startOfDay()));
    MessageComposer::SendLaterInfo copyInfo(info);
    QCOMPARE(info, copyInfo);
}

QTEST_MAIN(SendLaterInfoTest)

#include "moc_sendlaterinfotest.cpp"
