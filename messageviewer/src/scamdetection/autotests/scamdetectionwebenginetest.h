/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QWidget>

namespace MessageViewer
{
class ScamDetectionWebEngine;
}
class QWebEngineView;
class TestWebEngineScamDetection : public QWidget
{
    Q_OBJECT
public:
    explicit TestWebEngineScamDetection(QWidget *parent = nullptr);
    ~TestWebEngineScamDetection() override;

    void setHtml(const QString &html);
private Q_SLOTS:
    void loadFinished(bool b);
Q_SIGNALS:
    void resultScanDetection(bool result);

private:
    QWebEngineView *const mEngineView;
    MessageViewer::ScamDetectionWebEngine *const mScamDetectionWebEngine;
};

class ScamDetectionWebEngineTest : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWebEngineTest(QObject *parent = nullptr);
    ~ScamDetectionWebEngineTest() override;
private Q_SLOTS:
    void scamtest_data();
    void scamtest();
};
