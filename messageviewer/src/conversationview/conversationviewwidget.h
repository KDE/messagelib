/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
namespace MessageViewer
{
class ConversationViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConversationViewWidget(QWidget *parent = nullptr);
    ~ConversationViewWidget() override;
};
}

