/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    explicit WebEngineView(QWidget *parent = Q_NULLPTR);
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
private Q_SLOTS:
    void slotLoadFinished();
private:
    WebEngineViewPrivate *const d;
};
}
#endif // WEBENGINEVIEW_H
