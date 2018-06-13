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

#include "webengineview.h"
#include "webenginenavigationrequestinterceptor.h"
#include "webenginemanagescript.h"
#include "webengineviewer_debug.h"
#include "checkphishingurl/localdatabasemanager.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

using namespace WebEngineViewer;

class WebEngineViewer::WebEngineViewPrivate
{
public:
    WebEngineViewPrivate(WebEngineView *q)
        : mSavedRelativePosition(-1)
        , mCurrentWidget(nullptr)
        , mWebEngineNavigatorInterceptor(nullptr)
        , mWebEngineNavigatorInterceptorView(nullptr)
        , mPhishingDatabase(nullptr)
        , mCrashCount(0)
        , q(q)
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

    qreal mSavedRelativePosition;
    QWidget *mCurrentWidget = nullptr;
    WebEngineManageScript *mManagerScript = nullptr;
    WebEngineNavigationRequestInterceptor *mWebEngineNavigatorInterceptor = nullptr;
    WebEngineView *mWebEngineNavigatorInterceptorView = nullptr;
    LocalDataBaseManager *mPhishingDatabase = nullptr;
    int mCrashCount;

private:
    WebEngineView *const q;
};

WebEngineView::WebEngineView(QWidget *parent)
    : QWebEngineView(parent)
    , d(new WebEngineViewer::WebEngineViewPrivate(this))
{
    installEventFilter(this);
    d->mManagerScript = new WebEngineManageScript(this);

    connect(this, &QWebEngineView::renderProcessTerminated,
            this, [this](QWebEnginePage::RenderProcessTerminationStatus status) {
        d->renderProcessTerminated(status);
    });
    connect(this, &QWebEngineView::loadFinished,
            this, [this]() {
        // Reset the crash counter if we manage to actually load a page.
        // This does not perfectly correspond to "we managed to render
        // a page", but it's the best we have
        d->mCrashCount = 0;
    });
}

WebEngineView::~WebEngineView()
{
    delete d;
}

WebEngineManageScript *WebEngineView::webEngineManagerScript() const
{
    return d->mManagerScript;
}

void WebEngineView::initializeJQueryScript()
{
    QFile file(QStringLiteral(":/data/jquery.min.js"));
    file.open(QIODevice::ReadOnly);
    QString jquery = QString::fromUtf8(file.readAll());
    jquery.append(QLatin1String("\nvar qt = { 'jQuery': jQuery.noConflict(true) };"));
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
}

void WebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
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
#define HANDLE_EVENT(f, t) \
    { \
        bool wasAccepted = event->isAccepted(); \
        event->setAccepted(false); \
        f(static_cast<t *>(event)); \
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
