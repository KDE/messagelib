/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>
namespace WebEngineViewer
{
class WebEnginePage;
class WebEngineView;
}

class TestSelectedTextWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestSelectedTextWebEngine(QWidget *parent = nullptr);
    ~TestSelectedTextWebEngine();

private Q_SLOTS:
    void slotSlowSelectedText();

private:
    WebEngineViewer::WebEnginePage *mEnginePage;
    WebEngineViewer::WebEngineView *pageView;
};

