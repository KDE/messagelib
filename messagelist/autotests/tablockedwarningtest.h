/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#ifndef TABLOCKEDWARNINGTEST_H
#define TABLOCKEDWARNINGTEST_H

#include <QObject>

class TabLockedWarningTest : public QObject
{
    Q_OBJECT
public:
    explicit TabLockedWarningTest(QObject *parent = nullptr);
    ~TabLockedWarningTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // TABLOCKEDWARNINGTEST_H
