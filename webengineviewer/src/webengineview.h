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

#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#include <QWebEngineScript>
#include "webengineviewer_export.h"
namespace WebEngineViewer {
class WebEngineViewPrivate;
class WebEngineManageScript;
class LocalDataBaseManager;

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
    void initializeJQueryScript();

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
