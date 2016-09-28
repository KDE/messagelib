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

#include "webengineview.h"
#include "webenginemanagescript.h"
#include "webengineviewer_debug.h"
#include "../config-webengineviewer.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

using namespace WebEngineViewer;

class WebEngineViewer::WebEngineViewPrivate
{
public:
    WebEngineViewPrivate()
        : mSavedRelativePosition(-1),
          mCurrentWidget(Q_NULLPTR)
    {

    }
    qreal mSavedRelativePosition;
    QWidget *mCurrentWidget;
    WebEngineManageScript *mManagerScript;
};

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent),
      d(new WebEngineViewer::WebEngineViewPrivate)
{
    installEventFilter(this);
    d->mManagerScript = new WebEngineManageScript(this);
}

WebEngineView::~WebEngineView()
{
    delete d;
}

void WebEngineView::initializeJQueryScript()
{
    QFile file;
    file.setFileName(QStringLiteral(":/data/jquery.min.js"));
    file.open(QIODevice::ReadOnly);
    QString jquery = QString::fromUtf8(file.readAll());
    jquery.append(QStringLiteral("\nvar qt = { 'jQuery': jQuery.noConflict(true) };"));
    file.close();
    d->mManagerScript->addScript(page()->profile(), jquery, QStringLiteral("jquery"), QWebEngineScript::DocumentCreation);

}

void WebEngineView::addScript(const QString &source, const QString &scriptName, QWebEngineScript::InjectionPoint injectionPoint)
{
    d->mManagerScript->addScript(page()->profile(), source, scriptName, injectionPoint);
}

void WebEngineView::forwardWheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}

void WebEngineView::forwardKeyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void WebEngineView::forwardKeyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void WebEngineView::forwardMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void WebEngineView::forwardMouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug()<<" void WebEngineView::forwardMouseMoveEvent(QKeyEvent *event)";
}

void WebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void WebEngineView::dragMoveEvent(QDragMoveEvent *e)
{
    qDebug() << " void WebEngineView::dragMoveEvent(QDragMoveEvent *e)";
    QWebEngineView::dragMoveEvent(e);
}

void WebEngineView::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << " void WebEngineView::dragEnterEvent(QDragEnterEvent *event)";
    QWebEngineView::dragEnterEvent(e);
}

void WebEngineView::dropEvent(QDropEvent *e)
{
    qDebug() << " WebEngineView::dropEvent ";
    QWebEngineView::dropEvent(e);
}

void WebEngineView::dragLeaveEvent(QDragLeaveEvent *e)
{
    qDebug() << " WebEngineView::dragLeaveEvent ";
    QWebEngineView::dragLeaveEvent(e);
}

bool WebEngineView::eventFilter(QObject *obj, QEvent *event)
{
    // Hack to find widget that receives input events
    if (obj == this && event->type() == QEvent::ChildAdded) {
        QWidget *child = qobject_cast<QWidget *>(static_cast<QChildEvent *>(event)->child());
        if (child && child->inherits("QtWebEngineCore::RenderWidgetHostViewQtDelegateWidget")) {
            d->mCurrentWidget = child;
            d->mCurrentWidget->installEventFilter(this);
        }
    }

    // Forward events to WebEngineView
    if (obj == d->mCurrentWidget) {
#define HANDLE_EVENT(f, t) \
    { \
        bool wasAccepted = event->isAccepted(); \
        event->setAccepted(false); \
        f(static_cast<t*>(event)); \
        bool ret = event->isAccepted(); \
        event->setAccepted(wasAccepted); \
        return ret; \
    }

        switch (event->type()) {
        case QEvent::KeyPress:
            HANDLE_EVENT(forwardKeyPressEvent, QKeyEvent);
        case QEvent::KeyRelease:
            HANDLE_EVENT(forwardKeyReleaseEvent, QKeyEvent);
        case QEvent::MouseButtonPress:
            HANDLE_EVENT(forwardMousePressEvent, QMouseEvent);
        case QEvent::MouseButtonRelease:
            HANDLE_EVENT(forwardMouseReleaseEvent, QMouseEvent);
        case QEvent::MouseMove:
            HANDLE_EVENT(forwardMouseMoveEvent, QMouseEvent);
        case QEvent::Wheel:
            HANDLE_EVENT(forwardWheelEvent, QWheelEvent);
        default:
            break;
        }

#undef HANDLE_EVENT
    }
    // Block already handled events
    if (obj == this) {
        switch (event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Wheel:
            return true;

        default:
            break;
        }
    }
    return QWebEngineView::eventFilter(obj, event);
}

QWebEngineView *WebEngineView::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type);
    return Q_NULLPTR;
}

void WebEngineView::clearRelativePosition()
{
    d->mSavedRelativePosition = -1;
}

void WebEngineView::saveRelativePosition()
{
#if QT_VERSION >= 0x050700
    d->mSavedRelativePosition = page()->scrollPosition().toPoint().y();
#endif
}

qreal WebEngineView::relativePosition() const
{
    qCDebug(WEBENGINEVIEWER_LOG) << "Relative Position" << d->mSavedRelativePosition;
    return d->mSavedRelativePosition;
}
