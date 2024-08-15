/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEngineView>
#include <WebEngineViewer/BlockTrackingUrlInterceptor>
namespace WebEngineViewer
{
class WebEngineViewPrivate;
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
    [[nodiscard]] qreal relativePosition() const;

    [[nodiscard]] WebEngineViewer::LocalDataBaseManager *phishingDatabase() const;

Q_SIGNALS:
    void mailTrackingFound(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);

protected:
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    [[nodiscard]] QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

    virtual void forwardWheelEvent(QWheelEvent *event);
    virtual void forwardKeyPressEvent(QKeyEvent *event);
    virtual void forwardKeyReleaseEvent(QKeyEvent *event);
    virtual void forwardMousePressEvent(QMouseEvent *event);
    virtual void forwardMouseMoveEvent(QMouseEvent *event);
    virtual void forwardMouseReleaseEvent(QMouseEvent *event);

private:
    std::unique_ptr<WebEngineViewPrivate> const d;
};
}
