/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SCAMDETECTIONWEBENGINETEST_H
#define SCAMDETECTIONWEBENGINETEST_H

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
    ~TestWebEngineScamDetection();

    void setHtml(const QString &html);
private Q_SLOTS:
    void loadFinished(bool b);
Q_SIGNALS:
    void resultScanDetection(bool result);

private:
    QWebEngineView *mEngineView = nullptr;
    MessageViewer::ScamDetectionWebEngine *mScamDetectionWebEngine = nullptr;
};

class ScamDetectionWebEngineTest : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWebEngineTest(QObject *parent = nullptr);
    ~ScamDetectionWebEngineTest();
private Q_SLOTS:
    void scamtest_data();
    void scamtest();
};

#endif // SCAMDETECTIONWEBENGINETEST_H
