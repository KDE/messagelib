/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWebEngineView>
#include <QWidget>
namespace WebEngineViewer
{
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
    ~TestWebEngine() override;

private:
    WebEngineViewer::WebEnginePage *mEnginePage;
};
