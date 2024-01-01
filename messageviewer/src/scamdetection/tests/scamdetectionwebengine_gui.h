/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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
    explicit ScamDetectionWebEngineTestWidget(const QString &filename, QWidget *parent = nullptr);
    ~ScamDetectionWebEngineTestWidget() override;

private Q_SLOTS:
    void slotLoadFinished();
    void slotOpenHtml();

private:
    MessageViewer::ScamDetectionWarningWidget *mScamWarningWidget;
    MessageViewer::ScamDetectionWebEngine *mScamDetection;
    QWebEngineView *mWebEngineView;
};
