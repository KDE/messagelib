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

#include "webengineview.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

using namespace MessageViewer;

class MessageViewer::WebEngineViewPrivate
{
public:
    WebEngineViewPrivate()
        : mCurrentWidget(Q_NULLPTR)
    {

    }
    QWidget *mCurrentWidget;
};

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent),
      d(new MessageViewer::WebEngineViewPrivate)
{
    installEventFilter(this);
}

WebEngineView::~WebEngineView()
{
    delete d;
}

void WebEngineView::forwardWheelEvent(QWheelEvent *event)
{
    qDebug()<<" void WebEngineView::forwardWheelEvent(QWheelEvent *event)";
}

void WebEngineView::forwardKeyPressEvent(QKeyEvent *event)
{
    qDebug()<<" void WebEngineView::forwardKeyPressEvent(QKeyEvent *event)";
}

void WebEngineView::forwardKeyReleaseEvent(QKeyEvent *event)
{
    qDebug()<<" void WebEngineView::forwardKeyReleaseEvent(QKeyEvent *event)";
}

void WebEngineView::forwardMousePressEvent(QMouseEvent *event)
{
    qDebug()<<" void WebEngineView::forwardMousePressEvent(QKeyEvent *event)";
}

void WebEngineView::forwardMouseMoveEvent(QMouseEvent *event)
{
    qDebug()<<" void WebEngineView::forwardMouseMoveEvent(QKeyEvent *event)";
}

void WebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<" void WebEngineView::forwardMouseReleaseEvent(QKeyEvent *event)";
}

bool WebEngineView::eventFilter(QObject *obj, QEvent *event)
{
    // Hack to find widget that receives input events
    if (obj == this && event->type() == QEvent::ChildAdded) {
        QWidget *child = qobject_cast<QWidget*>(static_cast<QChildEvent*>(event)->child());
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
