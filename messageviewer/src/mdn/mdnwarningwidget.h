/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include <KMessageWidget>

class MDNWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit MDNWarningWidget(QWidget *parent = nullptr);
    ~MDNWarningWidget() override;

private:
    void slotSend();
    void slotIgnore();
    QAction *const mIgnoreAction;
    QAction *const mSendAction;
};
