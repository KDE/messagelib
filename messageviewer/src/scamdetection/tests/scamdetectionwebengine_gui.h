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

#ifndef TEST_SCAMDETECTIONWEBENGINE_GUI_H
#define TEST_SCAMDETECTIONWEBENGINE_GUI_H

#include <QWidget>

namespace MessageViewer
{
class ScamDetectionWarningWidget;
class ScamDetectionWebEngine;
}
class QWebEngineView;
class ScamDetectionWebEngineTestWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScamDetectionWebEngineTestWidget(const QString &filename, QWidget *parent = Q_NULLPTR);
    ~ScamDetectionWebEngineTestWidget();

private Q_SLOTS:
    void slotLoadFinished();
    void slotOpenHtml();

private:
    MessageViewer::ScamDetectionWarningWidget *mScamWarningWidget;
    MessageViewer::ScamDetectionWebEngine *mScamDetection;
    QWebEngineView *mWebEngineView;
};

#endif
