/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWebEngineView>
#include <QWidget>

namespace WebEngineViewer
{
class WebEngineView;
}

class WebEngineViewBase : public QWebEngineView
{
    Q_OBJECT
public:
    explicit WebEngineViewBase(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};

class TestDndWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestDndWebEngine(QWidget *parent = nullptr);
    ~TestDndWebEngine() override;

private:
    WebEngineViewer::WebEngineView *pageView = nullptr;
    WebEngineViewBase *webEngineView = nullptr;
};
