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

#ifndef TESTWEBENGINEACCESSKEY_H
#define TESTWEBENGINEACCESSKEY_H

#include <QWebEngineView>
#include <QWidget>
namespace MessageViewer
{
class MailWebEngineView;
class MailWebView;
}

class TestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestWidget(QWidget *parent = Q_NULLPTR);
    ~TestWidget();
};

class TestWebKitAccesskey : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebKitAccesskey(QWidget *parent = Q_NULLPTR);
    ~TestWebKitAccesskey();

private Q_SLOTS:
    void slotShowAccessKey();

private:
    MessageViewer::MailWebView *mTestWebEngine;
};

class TestWebEngineAccesskey : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineAccesskey(QWidget *parent = Q_NULLPTR);
    ~TestWebEngineAccesskey();

private Q_SLOTS:
    void slotShowAccessKey();

private:
    MessageViewer::MailWebEngineView *mTestWebEngine;
};

#endif // TESTWEBENGINEACCESSKEY_H
