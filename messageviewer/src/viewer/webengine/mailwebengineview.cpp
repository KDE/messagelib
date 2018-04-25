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
#include "mailwebengineview.h"
#include "mailwebenginepage.h"
#include "webengineviewer/webengineaccesskey.h"
#include "webengineviewer/webenginescript.h"
#include "mailwebenginescript.h"
#include "messageviewer/messageviewersettings.h"
#include "../urlhandlermanager.h"
#include "loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.h"
#include "blockexternalresourcesurlinterceptor/blockexternalresourcesurlinterceptor.h"
#include "blockmailtrackingurlinterceptor/blockmailtrackingurlinterceptor.h"
#include "cidreferencesurlinterceptor/cidreferencesurlinterceptor.h"
#include <WebEngineViewer/InterceptorManager>
#include <WebEngineViewer/WebEngineManageScript>

#include "scamdetection/scamdetectionwebengine.h"
#include "scamdetection/scamcheckshorturl.h"
#include <QContextMenuEvent>
#include <WebEngineViewer/WebHitTest>

#include <QWebEngineProfile>
#include <QPrinter>

#include <WebEngineViewer/WebHitTestResult>

using namespace MessageViewer;
template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFunction)(result);
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}

class MessageViewer::MailWebEngineViewPrivate
{
public:
    MailWebEngineViewPrivate()
    {
    }

    QUrl mHoveredUrl;
    QPoint mLastClickPosition;
    ScamDetectionWebEngine *mScamDetection = nullptr;
    WebEngineViewer::WebEngineAccessKey *mWebViewAccessKey = nullptr;
    MessageViewer::LoadExternalReferencesUrlInterceptor *mExternalReference = nullptr;
    MailWebEnginePage *mPageEngine = nullptr;
    WebEngineViewer::InterceptorManager *mNetworkAccessManager = nullptr;
    MessageViewer::ViewerPrivate *mViewer = nullptr;
    bool mCanStartDrag = false;
};

MailWebEngineView::MailWebEngineView(KActionCollection *ac, QWidget *parent)
    : WebEngineViewer::WebEngineView(parent)
    , d(new MessageViewer::MailWebEngineViewPrivate)
{
    d->mPageEngine = new MailWebEnginePage(new QWebEngineProfile(this), this);
    setPage(d->mPageEngine);
    d->mWebViewAccessKey = new WebEngineViewer::WebEngineAccessKey(this, this);
    d->mWebViewAccessKey->setActionCollection(ac);
    d->mScamDetection = new ScamDetectionWebEngine(this);
    connect(d->mScamDetection, &ScamDetectionWebEngine::messageMayBeAScam, this,
            &MailWebEngineView::messageMayBeAScam);
    connect(d->mWebViewAccessKey, &WebEngineViewer::WebEngineAccessKey::openUrl, this,
            &MailWebEngineView::openUrl);
    connect(this, &MailWebEngineView::loadFinished, this, &MailWebEngineView::slotLoadFinished);

    d->mNetworkAccessManager = new WebEngineViewer::InterceptorManager(this, ac, this);
    d->mExternalReference = new MessageViewer::LoadExternalReferencesUrlInterceptor(this);
    d->mNetworkAccessManager->addInterceptor(d->mExternalReference);
    MessageViewer::CidReferencesUrlInterceptor *cidReference
        = new MessageViewer::CidReferencesUrlInterceptor(this);
    d->mNetworkAccessManager->addInterceptor(cidReference);
    MessageViewer::BlockExternalResourcesUrlInterceptor *blockExternalUrl
        = new MessageViewer::BlockExternalResourcesUrlInterceptor(this);
    connect(blockExternalUrl, &BlockExternalResourcesUrlInterceptor::formSubmittedForbidden, this,
            &MailWebEngineView::formSubmittedForbidden);
    d->mNetworkAccessManager->addInterceptor(blockExternalUrl);

    MessageViewer::BlockMailTrackingUrlInterceptor *blockMailTrackingUrl
        = new MessageViewer::BlockMailTrackingUrlInterceptor(this);
    connect(blockMailTrackingUrl, &BlockMailTrackingUrlInterceptor::mailTrackingFound, this,
            &MailWebEngineView::mailTrackingFound);
    d->mNetworkAccessManager->addInterceptor(blockMailTrackingUrl);

    setFocusPolicy(Qt::WheelFocus);
    connect(d->mPageEngine, &MailWebEnginePage::urlClicked, this, &MailWebEngineView::openUrl);
    connect(
        page(), &QWebEnginePage::scrollPositionChanged, d->mWebViewAccessKey,
        &WebEngineViewer::WebEngineAccessKey::hideAccessKeys);
    initializeScripts();
}

MailWebEngineView::~MailWebEngineView()
{
    delete d;
}

void MailWebEngineView::setLinkHovered(const QUrl &url)
{
    //TODO we need to detect image url too.
    d->mHoveredUrl = url;
}

void MailWebEngineView::runJavaScriptInWordId(const QString &script)
{
    page()->runJavaScript(script, WebEngineViewer::WebEngineManageScript::scriptWordId());
}

void MailWebEngineView::setViewer(MessageViewer::ViewerPrivate *viewer)
{
    d->mViewer = viewer;
}

void MailWebEngineView::initializeScripts()
{
    initializeJQueryScript();
}

void MailWebEngineView::contextMenuEvent(QContextMenuEvent *e)
{
    WebEngineViewer::WebHitTest *webHit = d->mPageEngine->hitTestContent(e->pos());
    connect(webHit, &WebEngineViewer::WebHitTest::finished, this,
            &MailWebEngineView::slotWebHitFinished);
}

void MailWebEngineView::slotWebHitFinished(const WebEngineViewer::WebHitTestResult &result)
{
    Q_EMIT popupMenu(result);
}

void MailWebEngineView::scrollUp(int pixels)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::scrollUp(pixels));
}

void MailWebEngineView::scrollDown(int pixels)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::scrollDown(pixels));
}

void MailWebEngineView::selectAll()
{
    page()->triggerAction(QWebEnginePage::SelectAll);
}

void MailWebEngineView::slotZoomChanged(qreal zoom)
{
    setZoomFactor(zoom);
}

void MailWebEngineView::scamCheck()
{
    d->mScamDetection->scanPage(page());
}

void MailWebEngineView::slotShowDetails()
{
    d->mScamDetection->showDetails();
}

void MailWebEngineView::forwardKeyReleaseEvent(QKeyEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        d->mWebViewAccessKey->keyReleaseEvent(e);
    }
}

void MailWebEngineView::forwardMousePressEvent(QMouseEvent *event)
{
    if (d->mViewer && !d->mHoveredUrl.isEmpty()) {
        if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) {
            // special processing for shift+click
            URLHandlerManager::instance()->handleShiftClick(d->mHoveredUrl, d->mViewer);
            event->accept();
            return;
        }
        if (event->button() == Qt::LeftButton) {
            d->mCanStartDrag = URLHandlerManager::instance()->willHandleDrag(d->mHoveredUrl,
                                                                             d->mViewer);
            d->mLastClickPosition = event->pos();
        }
    }
}

void MailWebEngineView::forwardMouseMoveEvent(QMouseEvent *event)
{
    if (d->mViewer && !d->mHoveredUrl.isEmpty()) {
        // If we are potentially handling a drag, deal with that.
        if (d->mCanStartDrag && (event->buttons() & Qt::LeftButton)) {
            if ((d->mLastClickPosition - event->pos()).manhattanLength()
                > QApplication::startDragDistance()) {
                if (URLHandlerManager::instance()->handleDrag(d->mHoveredUrl, d->mViewer)) {
                    // If the URL handler manager started a drag, don't handle this in the future
                    d->mCanStartDrag = false;
                }
            }
            event->accept();
        }
    }
}

void MailWebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    d->mCanStartDrag = false;
}

void MailWebEngineView::forwardKeyPressEvent(QKeyEvent *e)
{
    if (e && hasFocus()) {
        if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
            d->mWebViewAccessKey->keyPressEvent(e);
        }
    }
}

void MailWebEngineView::forwardWheelEvent(QWheelEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        d->mWebViewAccessKey->wheelEvent(e);
    }
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        const int numDegrees = e->delta() / 8;
        const int numSteps = numDegrees / 15;
        Q_EMIT wheelZoomChanged(numSteps);
        e->accept();
    }
}

void MailWebEngineView::resizeEvent(QResizeEvent *e)
{
    if (MessageViewer::MessageViewerSettings::self()->accessKeyEnabled()) {
        d->mWebViewAccessKey->resizeEvent(e);
    }
    QWebEngineView::resizeEvent(e);
}

void MailWebEngineView::saveMainFrameScreenshotInFile(const QString &filename)
{
    //TODO need to verify it
    QImage image(size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    render(&painter);
    painter.end();
    image.save(filename);
}

void MailWebEngineView::showAccessKeys()
{
    d->mWebViewAccessKey->showAccessKeys();
}

void MailWebEngineView::hideAccessKeys()
{
    d->mWebViewAccessKey->hideAccessKeys();
}

void MailWebEngineView::isScrolledToBottom()
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::isScrolledToBottom(),
                          WebEngineViewer::WebEngineManageScript::scriptWordId(),
                          invoke(this, &MailWebEngineView::handleIsScrolledToBottom));
}

void MailWebEngineView::setElementByIdVisible(const QString &id, bool visible)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::setElementByIdVisible(id, visible));
}

void MailWebEngineView::removeAttachmentMarking(const QString &id)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::removeStyleToElement(id));
}

void MailWebEngineView::markAttachment(const QString &id, const QString &style)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::setStyleToElement(id, style));
}

void MailWebEngineView::scrollToAnchor(const QString &anchor)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::searchElementPosition(anchor),
                          WebEngineViewer::WebEngineManageScript::scriptWordId(),
                          invoke(this, &MailWebEngineView::handleScrollToAnchor));
}

void MailWebEngineView::handleIsScrolledToBottom(const QVariant &result)
{
    bool scrolledToBottomResult = false;
    if (result.isValid()) {
        scrolledToBottomResult = result.toBool();
    }
    Q_EMIT pageIsScrolledToBottom(scrolledToBottomResult);
}

void MailWebEngineView::handleScrollToAnchor(const QVariant &result)
{
    if (result.isValid()) {
        const QList<QVariant> lst = result.toList();
        if (lst.count() == 2) {
            const QPoint pos(lst.at(0).toInt(), lst.at(1).toInt());
            runJavaScriptInWordId(WebEngineViewer::WebEngineScript::scrollToPosition(pos));
        }
    }
}

void MailWebEngineView::scrollPageDown(int percent)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::scrollPercentage(percent));
}

void MailWebEngineView::scrollPageUp(int percent)
{
    scrollPageDown(-percent);
}

void MailWebEngineView::executeHideShowEncryptionDetails(bool hide)
{
    const QString source = MessageViewer::MailWebEngineScript::manageShowHideEncryptionDetails(hide);
    runJavaScriptInWordId(source);
}

void MailWebEngineView::scrollToRelativePosition(qreal pos)
{
    runJavaScriptInWordId(WebEngineViewer::WebEngineScript::scrollToRelativePosition(pos));
}

void MailWebEngineView::setAllowExternalContent(bool b)
{
    if (d->mExternalReference->allowExternalContent() != b) {
        d->mExternalReference->setAllowExternalContent(b);
        reload();
    }
}

QList<QAction *> MailWebEngineView::interceptorUrlActions(
    const WebEngineViewer::WebHitTestResult &result) const
{
    return d->mNetworkAccessManager->interceptorUrlActions(result);
}

void MailWebEngineView::slotLoadFinished()
{
    scamCheck();
}

void MailWebEngineView::setPrintElementBackground(bool printElementBackground)
{
    d->mPageEngine->setPrintElementBackground(printElementBackground);
}

bool MailWebEngineView::execPrintPreviewPage(QPrinter *printer, int timeout)
{
    return d->mPageEngine->execPrintPreviewPage(printer, timeout);
}
