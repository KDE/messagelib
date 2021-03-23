/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class ComposerViewBaseTest : public QObject
{
    Q_OBJECT
public:
    explicit ComposerViewBaseTest(QObject *parent = nullptr);
    ~ComposerViewBaseTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

