/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QWidget>
namespace WebEngineViewer
{
class MailWebEngineView;
}

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = nullptr);
    ~TestWidget();
};

class TestWebEngineAccesskey : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineAccesskey(QWidget *parent = nullptr);
    ~TestWebEngineAccesskey();

private Q_SLOTS:
    void slotShowAccessKey();

private:
    WebEngineViewer::MailWebEngineView *mTestWebEngine = nullptr;
};

