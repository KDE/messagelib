/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEngineScript>
#include <QWebEngineView>
#include <WebEngineViewer/BlockTrackingUrlInterceptor>
namespace WebEngineViewer
{
class WebEngineViewPrivate;
class WebEngineManageScript;
class LocalDataBaseManager;

/**
 * @brief The WebEngineView class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit WebEngineView(QWidget *parent = nullptr);
    ~WebEngineView() override;

    void clearRelativePosition();
    void saveRelativePosition();
    Q_REQUIRED_RESULT qreal relativePosition() const;

    void addScript(const QString &source, const QString &scriptName, QWebEngineScript::InjectionPoint injectionPoint);

    Q_REQUIRED_RESULT WebEngineManageScript *webEngineManagerScript() const;

    Q_REQUIRED_RESULT WebEngineViewer::LocalDataBaseManager *phishingDatabase() const;

Q_SIGNALS:
    void mailTrackingFound(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

    virtual void forwardWheelEvent(QWheelEvent *event);
    virtual void forwardKeyPressEvent(QKeyEvent *event);
    virtual void forwardKeyReleaseEvent(QKeyEvent *event);
    virtual void forwardMousePressEvent(QMouseEvent *event);
    virtual void forwardMouseMoveEvent(QMouseEvent *event);
    virtual void forwardMouseReleaseEvent(QMouseEvent *event);

private:
    WebEngineViewPrivate *const d;
};
}
