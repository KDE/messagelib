/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#ifndef SCAMDETECTIONWEBENGINETEST_H
#define SCAMDETECTIONWEBENGINETEST_H

#include <QObject>
#include <QWidget>

namespace MessageViewer {
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
