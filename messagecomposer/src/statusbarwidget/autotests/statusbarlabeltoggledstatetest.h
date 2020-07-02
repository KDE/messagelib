/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#ifndef StatusBarLabelToggledStateTEST_H
#define StatusBarLabelToggledStateTEST_H

#include <QObject>

class StatusBarLabelToggledStateTest : public QObject
{
    Q_OBJECT
public:
    explicit StatusBarLabelToggledStateTest(QObject *parent = nullptr);
    ~StatusBarLabelToggledStateTest();

private Q_SLOTS:
    void shouldHasDefaultValue();
    void shouldChangeState();
    void shouldEmitSignalWhenChangeState();
    void shouldNotEmitSignalWhenWeDontChangeState();
    void shouldEmitSignalWhenClickOnLabel();
    void shouldChangeTestWhenStateChanged();
};

#endif // StatusBarLabelToggledStateTEST_H
