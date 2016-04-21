/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TESTMAILWEBENGINESELECTION_H
#define TESTMAILWEBENGINESELECTION_H

#include <QWidget>

namespace MessageViewer
{
class MailWebEngineView;
}

class TestMailWebEngineSelection : public QWidget
{
    Q_OBJECT
public:
    explicit TestMailWebEngineSelection(QWidget *parent = Q_NULLPTR);
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
