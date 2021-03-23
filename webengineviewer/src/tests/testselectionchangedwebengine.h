/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

namespace WebEngineViewer
{
class WebEngineView;
}

class TestSelectionChangedEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestSelectionChangedEngine(QWidget *parent = nullptr);
    ~TestSelectionChangedEngine();
private Q_SLOTS:
    void slotSelectionChanged();

private:
    WebEngineViewer::WebEngineView *pageView;
};

