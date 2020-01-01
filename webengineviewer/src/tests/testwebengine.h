/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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

#ifndef TESTWEBENGINE_H
#define TESTWEBENGINE_H

#include <QWidget>
#include <QWebEngineView>
namespace WebEngineViewer {
class WebEnginePage;
class WebHitTestResult;
}

class TestWebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit TestWebEngineView(QWidget *parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent *e) override;
private Q_SLOTS:
    void slotHitTestFinished(const WebEngineViewer::WebHitTestResult &result);
};

class TestWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngine(QWidget *parent = nullptr);
    ~TestWebEngine();
private:
    WebEngineViewer::WebEnginePage *mEnginePage;
};

#endif // TESTWEBENGINE_H
