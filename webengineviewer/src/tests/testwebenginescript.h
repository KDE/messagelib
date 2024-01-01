/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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
    ~TestWebEngineScript() override;

private Q_SLOTS:
    void slotExecuteScript();
    void handleScript(const QVariant &res);
    void slotShowConsoleMessage(const QString &msg);

private:
    TestScriptWidget *mTestScriptWidget;
    TestWebEngineScriptView *mTestWebEngine;
};
