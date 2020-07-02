/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTSELECTEDTEXTWEBENGINE_H
#define TESTSELECTEDTEXTWEBENGINE_H

#include <QWidget>
namespace WebEngineViewer {
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

#endif
