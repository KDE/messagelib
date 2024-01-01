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

class TestMailWebEngine : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailWebEngine(QWidget *parent = nullptr);
    ~TestMailWebEngine() override;
private Q_SLOTS:
    void slotScrollUp();

    void slotScrollDown();
    void slotZoomUp();
    void slotZoomDown();
    void slotOpenUrl(const QUrl &url);
    void slotPrintPreview();

private:
    MessageViewer::MailWebEngineView *mTestWebEngine;
    qreal mZoom;
};
