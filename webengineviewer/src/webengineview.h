/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#include <QWebEngineScript>
#include "webengineviewer_export.h"
namespace WebEngineViewer {
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
    void setLinkHovered(const QUrl &url);

    Q_REQUIRED_RESULT WebEngineViewer::LocalDataBaseManager *phishingDatabase() const;

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
#endif // WEBENGINEVIEW_H
