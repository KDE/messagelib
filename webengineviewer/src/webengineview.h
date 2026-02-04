/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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

/*!
 * \class WebEngineViewer::WebEngineView
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/WebEngineView
 *
 * \brief The WebEngineView class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT WebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    /*! Constructs a WebEngineView object with the given parent. */
    explicit WebEngineView(QWidget *parent = nullptr);
    /*! Destroys the WebEngineView object. */
    ~WebEngineView() override;

    /*! Clears the relative position. */
    void clearRelativePosition();
    /*! Saves the current relative position. */
    void saveRelativePosition();
    /*! Returns the relative position. */
    [[nodiscard]] qreal relativePosition() const;

    /*! Returns the phishing database manager. */
    [[nodiscard]] WebEngineViewer::LocalDataBaseManager *phishingDatabase() const;

Q_SIGNALS:
    /*! Emitted when mail tracking is found. */
    void mailTrackingFound(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);

protected:
    /*! Filters events for the object. */
    [[nodiscard]] bool eventFilter(QObject *obj, QEvent *event) override;
    /*! Creates a new window of the specified type. */
    [[nodiscard]] QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

    /*! Forwards the wheel event. */
    virtual void forwardWheelEvent(QWheelEvent *event);
    /*! Forwards the key press event. */
    virtual void forwardKeyPressEvent(QKeyEvent *event);
    /*! Forwards the key release event. */
    virtual void forwardKeyReleaseEvent(QKeyEvent *event);
    /*! Forwards the mouse press event. */
    virtual void forwardMousePressEvent(QMouseEvent *event);
    /*! Forwards the mouse move event. */
    virtual void forwardMouseMoveEvent(QMouseEvent *event);
    /*! Forwards the mouse release event. */
    virtual void forwardMouseReleaseEvent(QMouseEvent *event);

private:
    std::unique_ptr<WebEngineViewPrivate> const d;
};
}
