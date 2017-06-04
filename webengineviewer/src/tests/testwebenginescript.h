/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#ifndef TESTWEBENGINESCRIPT_H
#define TESTWEBENGINESCRIPT_H

#include <QWebEngineView>
#include <QWidget>
class QTextEdit;
class QComboBox;
class TestScriptWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestScriptWidget(QWidget *parent = nullptr);
    void setResult(const QString &res);
    QString script() const;
Q_SIGNALS:
    void executeScript();
private Q_SLOTS:
    void slotCurrentIndexChanged(int);
private:
    void fillScriptCombo(QComboBox *scriptCombo);
    QTextEdit *mScriptEdit;
    QTextEdit *mResultEdit;
    QComboBox *mScriptCombo;
};

class TestWebEngineScriptPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit TestWebEngineScriptPage(QObject *parent = nullptr);

protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;

Q_SIGNALS:
    void showConsoleMessage(const QString &msg);
};

class TestWebEngineScriptView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit TestWebEngineScriptView(QWidget *parent = nullptr);
};

class TestWebEngineScript : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineScript(QWidget *parent = nullptr);
    ~TestWebEngineScript();

private Q_SLOTS:
    void slotExecuteScript();
    void handleScript(const QVariant &res);
    void slotShowConsoleMessage(const QString &msg);
private:
    TestScriptWidget *mTestScriptWidget;
    TestWebEngineScriptView *mTestWebEngine;
};

#endif // TESTWEBENGINESCRIPT_H
