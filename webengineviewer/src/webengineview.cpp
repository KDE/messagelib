/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webengineview.h"
#include "checkphishingurl/localdatabasemanager.h"
#include "webenginemanagescript.h"
#include "webenginenavigationrequestinterceptor.h"
#include "webengineviewer_debug.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QWheelEvent>

using namespace WebEngineViewer;

class WebEngineViewer::WebEngineViewPrivate
{
public:
    explicit WebEngineViewPrivate(WebEngineView *q)
        : q(q)
    {
    }

    ~WebEngineViewPrivate()
    {
        delete mWebEngineNavigatorInterceptor;
        mWebEngineNavigatorInterceptor = nullptr;
        delete mWebEngineNavigatorInterceptorView;
        mWebEngineNavigatorInterceptorView = nullptr;
    }

    void renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus status)
    {
        switch (status) {
        case QWebEnginePage::NormalTerminationStatus:
            return;

        case QWebEnginePage::AbnormalTerminationStatus:
            qCInfo(WEBENGINEVIEWER_LOG) << "WebEngine render process terminated abnormally";
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            qCInfo(WEBENGINEVIEWER_LOG) << "WebEngine render process crashed";
            break;
        case QWebEnginePage::KilledTerminationStatus:
            qCInfo(WEBENGINEVIEWER_LOG) << "WebEngine render process killed";
            break;
        }

        // don't get stuck in a loop if the renderer keeps crashing. Five restarts
        // is an arbitrary constant.
        if (++mCrashCount < 6) {
            QTimer::singleShot(0, q, &QWebEngineView::reload);
        } else {
            // TODO: try to show a sadface page
        }
    }

    qreal mSavedRelativePosition = -1;
    QWidget *mCurrentWidget = nullptr;
    WebEngineNavigationRequestInterceptor *mWebEngineNavigatorInterceptor = nullptr;
    WebEngineView *mWebEngineNavigatorInterceptorView = nullptr;
    LocalDataBaseManager *mPhishingDatabase = nullptr;
    int mCrashCount = 0;

private:
    WebEngineView *const q;
};

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent)
    , d(new WebEngineViewer::WebEngineViewPrivate(this))
{
    installEventFilter(this);

    connect(this, &QWebEngineView::renderProcessTerminated, this, [this](QWebEnginePage::RenderProcessTerminationStatus status) {
        d->renderProcessTerminated(status);
    });
    connect(this, &QWebEngineView::loadFinished, this, [this]() {
        // Reset the crash counter if we manage to actually load a page.
        // This does not perfectly correspond to "we managed to render
        // a page", but it's the best we have
        d->mCrashCount = 0;
    });
}

WebEngineView::~WebEngineView() = default;

void WebEngineView::forwardWheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event)
}

void WebEngineView::forwardKeyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
}

void WebEngineView::forwardKeyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
}

void WebEngineView::forwardMousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void WebEngineView::forwardMouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void WebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

bool WebEngineView::eventFilter(QObject *obj, QEvent *event)
{
    // Keyboard events are sent to parent widget
    if (obj == this && event->type() == QEvent::ParentChange && parentWidget()) {
        parentWidget()->installEventFilter(this);
    }

    // Hack to find widget that receives input events
    if (obj == this && event->type() == QEvent::ChildAdded) {
        QTimer::singleShot(0, this, [this]() {
            if (focusProxy() && d->mCurrentWidget != focusProxy()) {
                d->mCurrentWidget = focusProxy();
                d->mCurrentWidget->installEventFilter(this);
            }
        });
    }

    // Forward events to WebEngineView
    if (obj == d->mCurrentWidget) {
#define HANDLE_EVENT(f, t)                                                                                                                                     \
    {                                                                                                                                                          \
        bool wasAccepted = event->isAccepted();                                                                                                                \
        event->setAccepted(false);                                                                                                                             \
        f(static_cast<t *>(event));                                                                                                                            \
        bool ret = event->isAccepted();                                                                                                                        \
        event->setAccepted(wasAccepted);                                                                                                                       \
        return ret;                                                                                                                                            \
    }

        switch (event->type()) {
        case QEvent::KeyPress:
            HANDLE_EVENT(forwardKeyPressEvent, QKeyEvent)
        case QEvent::KeyRelease:
            HANDLE_EVENT(forwardKeyReleaseEvent, QKeyEvent)
        case QEvent::MouseButtonPress:
            HANDLE_EVENT(forwardMousePressEvent, QMouseEvent)
        case QEvent::MouseButtonRelease:
            HANDLE_EVENT(forwardMouseReleaseEvent, QMouseEvent)
        case QEvent::MouseMove:
            HANDLE_EVENT(forwardMouseMoveEvent, QMouseEvent)
        case QEvent::Wheel:
            HANDLE_EVENT(forwardWheelEvent, QWheelEvent)
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
    Q_UNUSED(type)
    delete d->mWebEngineNavigatorInterceptor;
    delete d->mWebEngineNavigatorInterceptorView;
    d->mWebEngineNavigatorInterceptorView = new WebEngineView();

    d->mWebEngineNavigatorInterceptor = new WebEngineNavigationRequestInterceptor(this->page());
    d->mWebEngineNavigatorInterceptorView->setPage(d->mWebEngineNavigatorInterceptor);
    return d->mWebEngineNavigatorInterceptorView;
}

void WebEngineView::clearRelativePosition()
{
    d->mSavedRelativePosition = -1;
}

void WebEngineView::saveRelativePosition()
{
    if (d->mSavedRelativePosition != -1) {
        d->mSavedRelativePosition = page()->scrollPosition().toPoint().y();
    }
}

qreal WebEngineView::relativePosition() const
{
    qCDebug(WEBENGINEVIEWER_LOG) << "Relative Position" << d->mSavedRelativePosition;
    return d->mSavedRelativePosition;
}

LocalDataBaseManager *WebEngineView::phishingDatabase() const
{
    if (!d->mPhishingDatabase) {
        d->mPhishingDatabase = new LocalDataBaseManager(const_cast<WebEngineView *>(this));
        d->mPhishingDatabase->initialize();
    }
    return d->mPhishingDatabase;
}

#include "moc_webengineview.cpp"
