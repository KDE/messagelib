/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONVERSATIONVIEWWIDGET_H
#define CONVERSATIONVIEWWIDGET_H

#include <QWidget>
namespace MessageViewer {
class ConversationViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConversationViewWidget(QWidget *parent = nullptr);
    ~ConversationViewWidget();
};
}

#endif // CONVERSATIONVIEWWIDGET_H
