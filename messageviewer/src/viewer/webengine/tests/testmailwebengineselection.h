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

class TestMailWebEngineSelection : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailWebEngineSelection(QWidget *parent = nullptr);
    ~TestMailWebEngineSelection() override;
private Q_SLOTS:
    void slotOpenUrl(const QUrl &url);
    void slotSwitchHtml();
    void slotShowSelection();

private:
    MessageViewer::MailWebEngineView *mTestWebEngine;
    int mNumber;
};
