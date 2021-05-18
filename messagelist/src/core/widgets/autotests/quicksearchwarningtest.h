/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class QuickSearchWarningTest : public QObject
{
    Q_OBJECT
public:
    explicit QuickSearchWarningTest(QObject *parent = nullptr);
    ~QuickSearchWarningTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldSetVisible();
    void shouldSetSearchText();
    void shouldSetSearchText_data();
};
