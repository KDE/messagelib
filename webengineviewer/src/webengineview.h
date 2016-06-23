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

#ifndef WEBENGINEVIEW_H
#define WEBENGINEVIEW_H

#include <QWebEngineView>
#include "webengineviewer_export.h"
class QWebEngineDownloadItem;
namespace WebEngineViewer
{
class WebEngineViewPrivate;
class WEBENGINEVIEWER_EXPORT WebEngineView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit WebEngineView(bool useJQuery = false, QWidget *parent = Q_NULLPTR);
    ~WebEngineView();

    void clearRelativePosition();
    void saveRelativePosition();
    qreal relativePosition() const;

    bool hasPrintPreviewSupport() const;
    void saveHtml(QWebEngineDownloadItem *download);

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) Q_DECL_OVERRIDE;

    virtual void forwardWheelEvent(QWheelEvent *event);
    virtual void forwardKeyPressEvent(QKeyEvent *event);
    virtual void forwardKeyReleaseEvent(QKeyEvent *event);
    virtual void forwardMousePressEvent(QMouseEvent *event);
    virtual void forwardMouseMoveEvent(QMouseEvent *event);
    virtual void forwardMouseReleaseEvent(QMouseEvent *event);

    void dragMoveEvent(QDragMoveEvent *e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotLoadFinished();
private:
    WebEngineViewPrivate *const d;
};
}
#endif // WEBENGINEVIEW_H
