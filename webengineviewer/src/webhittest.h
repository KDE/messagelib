/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <QPoint>
class QWebEnginePage;
namespace WebEngineViewer
{
class WebHitTestResult;
class WebHitTestPrivate;
/**
 * @brief The WebHitTest class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebHitTest : public QObject
{
    Q_OBJECT
public:
    explicit WebHitTest(QWebEnginePage *page, const QPoint &zoomedPos, const QPoint &pos, QObject *parent = nullptr);
    ~WebHitTest() override;

Q_SIGNALS:
    void finished(const WebEngineViewer::WebHitTestResult &result);

private Q_SLOTS:
    void handleHitTest(const QVariant &result);

private:
    WebHitTestPrivate *const d;
};
}

