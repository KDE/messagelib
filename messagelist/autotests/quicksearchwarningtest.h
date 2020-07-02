/*
  SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef QUICKSEARCHWARNINGTEST_H
#define QUICKSEARCHWARNINGTEST_H

#include <QObject>

class QuickSearchWarningTest : public QObject
{
    Q_OBJECT
public:
    explicit QuickSearchWarningTest(QObject *parent = nullptr);
    ~QuickSearchWarningTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldSetVisible();
    void shouldSetSearchText();
    void shouldSetSearchText_data();
};

#endif // QUICKSEARCHWARNINGTEST_H
