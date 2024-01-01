/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
namespace MessageViewer
{
class MailWebEngineView;
}
class TestWebEngineScrollAddAttachment : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineScrollAddAttachment(QWidget *parent = nullptr);

private Q_SLOTS:
    void slotScrollToAttachment();

private:
    MessageViewer::MailWebEngineView *mTestWebEngine;
};
