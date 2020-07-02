/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef MAILINGLISTTEST_H
#define MAILINGLISTTEST_H

#include <QObject>

class MailingListTest : public QObject
{
    Q_OBJECT
public:
    explicit MailingListTest(QObject *parent = nullptr);
    ~MailingListTest();

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldRestoreFromSettings();
    void shouldCopyReminderInfo();
};

#endif // MAILINGLISTTEST_H
