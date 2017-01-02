/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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
#ifndef MAILWEBENGINEACCESSKEYANCHORFROMHTMLTEST_H
#define MAILWEBENGINEACCESSKEYANCHORFROMHTMLTEST_H

#include <QObject>
#include <QWidget>

#include "../webengineaccesskeyanchor.h"
class QWebEngineView;
class TestWebEngineAccessKey : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineAccessKey(QWidget *parent = nullptr);
    ~TestWebEngineAccessKey();

    void setHtml(const QString &html);
private Q_SLOTS:
    void loadFinished(bool b);

    void handleSearchAccessKey(const QVariant &var);
Q_SIGNALS:
    void accessKeySearchFinished(const QVector<WebEngineViewer::WebEngineAccessKeyAnchor> &var);

private:
    QWebEngineView *mEngineView;
};

class WebEngineAccessKeyAnchorFromHtmlTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineAccessKeyAnchorFromHtmlTest(QObject *parent = nullptr);
private Q_SLOTS:
    void shouldNotShowAccessKeyWhenHtmlAsNotAnchor();
    void shouldReturnOneAnchor();
    void shouldReturnTwoAnchor();
};

#endif // MAILWEBENGINEACCESSKEYANCHORFROMHTMLTEST_H
