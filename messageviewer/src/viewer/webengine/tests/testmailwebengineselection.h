/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTMAILWEBENGINESELECTION_H
#define TESTMAILWEBENGINESELECTION_H

#include <QWidget>

namespace MessageViewer {
class MailWebEngineView;
}

class TestMailWebEngineSelection : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailWebEngineSelection(QWidget *parent = nullptr);
    ~TestMailWebEngineSelection();
private Q_SLOTS:
    void slotOpenUrl(const QUrl &url);
    void slotSwitchHtml();
    void slotShowSelection();
private:
    MessageViewer::MailWebEngineView *mTestWebEngine;
    int mNumber;
};

#endif // TESTMAILWEBENGINE_H
