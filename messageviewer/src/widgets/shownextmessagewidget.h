/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QWidget>
class QPushButton;
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT ShowNextMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowNextMessageWidget(QWidget *parent = nullptr);
    ~ShowNextMessageWidget() override;

    void updateButton(bool hasPreviousMessage, bool hasNextMessage);
Q_SIGNALS:
    void showNextMessage();
    void showPreviousMessage();

private:
    QPushButton *const mNextMessage;
    QPushButton *const mPreviousMessage;
};
}
