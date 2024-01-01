/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class MailingListTest : public QObject
{
    Q_OBJECT
public:
    explicit MailingListTest(QObject *parent = nullptr);
    ~MailingListTest() override;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldRestoreFromSettings();
    void shouldCopyReminderInfo();
};
