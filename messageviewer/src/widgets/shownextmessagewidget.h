/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHOWNEXTMESSAGEWIDGET_H
#define SHOWNEXTMESSAGEWIDGET_H

#include <QWidget>
#include "messageviewer_private_export.h"
class QPushButton;
namespace MessageViewer {
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
    QPushButton *mNextMessage = nullptr;
    QPushButton *mPreviousMessage = nullptr;
};
}

#endif // SHOWNEXTMESSAGEWIDGET_H
