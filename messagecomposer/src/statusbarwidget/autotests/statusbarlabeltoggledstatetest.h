/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class StatusBarLabelToggledStateTest : public QObject
{
    Q_OBJECT
public:
    explicit StatusBarLabelToggledStateTest(QObject *parent = nullptr);
    ~StatusBarLabelToggledStateTest() override;

private Q_SLOTS:
    void shouldHasDefaultValue();
    void shouldChangeState();
    void shouldEmitSignalWhenChangeState();
    void shouldNotEmitSignalWhenWeDontChangeState();
    void shouldEmitSignalWhenClickOnLabel();
    void shouldChangeTestWhenStateChanged();
};
