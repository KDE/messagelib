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

#ifndef TESTWEBENGINESCRIPT_H
#define TESTWEBENGINESCRIPT_H

#include <QWebEngineView>
#include <QWidget>
class QTextEdit;
class TestScriptWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TestScriptWidget(QWidget *parent = Q_NULLPTR);
    void setResult(const QString &res);
    QString script() const;
Q_SIGNALS:
    void executeScript();
private:
    QTextEdit *mScriptEdit;
    QTextEdit *mResultEdit;
};

class TestWebEngineScriptView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit TestWebEngineScriptView(QWidget *parent = Q_NULLPTR);
};

class TestWebEngineScript : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineScript(QWidget *parent = Q_NULLPTR);
    ~TestWebEngineScript();

private Q_SLOTS:
    void slotExecuteScript();
    void handleScript(const QVariant &res);

private:
    TestScriptWidget *mTestScriptWidget;
    TestWebEngineScriptView *mTestWebEngine;
};

#endif // TESTWEBENGINESCRIPT_H
