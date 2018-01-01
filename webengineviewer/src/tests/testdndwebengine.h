/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TestDndWebEngine_H
#define TestDndWebEngine_H

#include <QWebEngineView>
#include <QWidget>

namespace WebEngineViewer {
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
    ~TestDndWebEngine();
private:
    WebEngineViewer::WebEngineView *pageView = nullptr;
    WebEngineViewBase *webEngineView = nullptr;
};

#endif // TestDndWebEngine_H
