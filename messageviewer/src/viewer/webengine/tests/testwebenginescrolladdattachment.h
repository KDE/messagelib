/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTWEBENGINESCROLLADDATTACHMENT_H
#define TESTWEBENGINESCROLLADDATTACHMENT_H

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

#endif // TESTWEBENGINESCROLLADDATTACHMENT_H
