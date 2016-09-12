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
#include "mailwebengineview.h"
#include "mailwebenginepage.h"
#include "webengineviewer/webengineaccesskey.h"
#include "webengineviewer/webenginescript.h"
#include "mailwebenginescript.h"
#include "messageviewer/messageviewersettings.h"
#include "loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.h"
#include "cidreferencesurlinterceptor/cidreferencesurlinterceptor.h"
#include <WebEngineViewer/InterceptorManager>

#include "scamdetection/scamdetectionwebengine.h"
#include "scamdetection/scamcheckshorturl.h"
#include <QContextMenuEvent>
#include <WebEngineViewer/WebHitTest>

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QDebug>

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
        : mScamDetection(Q_NULLPTR),
          mWebViewAccessKey(Q_NULLPTR),
          mExternalReference(Q_NULLPTR),
          mPageEngine(Q_NULLPTR),
          mNetworkAccessManager(Q_NULLPTR)
    {

    }
    ScamDetectionWebEngine *mScamDetection;
    WebEngineViewer::WebEngineAccessKey *mWebViewAccessKey;
    MessageViewer::LoadExternalReferencesUrlInterceptor *mExternalReference;
    MailWebEnginePage *mPageEngine;
    WebEngineViewer::InterceptorManager *mNetworkAccessManager;
};

MailWebEngineView::MailWebEngineView(KActionCollection *ac, QWidget *parent)
    : WebEngineViewer::WebEngineView(parent),
      d(new MessageViewer::MailWebEngineViewPrivate)

{
    d->mPageEngine = new MailWebEnginePage(new QWebEngineProfile(this), this);
    setPage(d->mPageEngine);
    d->mWebViewAccessKey = new WebEngineViewer::WebEngineAccessKey(this, this);
    d->mWebViewAccessKey->setActionCollection(ac);
    d->mScamDetection = new ScamDetectionWebEngine(this);
    connect(d->mScamDetection, &ScamDetectionWebEngine::messageMayBeAScam, this, &MailWebEngineView::messageMayBeAScam);
    connect(d->mWebViewAccessKey, &WebEngineViewer::WebEngineAccessKey::openUrl, this, &MailWebEngineView::openUrl);
    connect(this, &MailWebEngineView::loadFinished, this, &MailWebEngineView::slotLoadFinished);

    d->mNetworkAccessManager = new WebEngineViewer::InterceptorManager(this, ac, this);
    d->mExternalReference = new MessageViewer::LoadExternalReferencesUrlInterceptor(this);
    d->mNetworkAccessManager->addInterceptor(d->mExternalReference);
    MessageViewer::CidReferencesUrlInterceptor *cidReference = new MessageViewer::CidReferencesUrlInterceptor(this);
    d->mNetworkAccessManager->addInterceptor(cidReference);
    setFocusPolicy(Qt::WheelFocus);
    connect(d->mPageEngine, &MailWebEnginePage::urlClicked, this, &MailWebEngineView::openUrl);
#if QT_VERSION >= 0x050700
    connect(page(), &QWebEnginePage::scrollPositionChanged, d->mWebViewAccessKey, &WebEngineViewer::WebEngineAccessKey::hideAccessKeys);
#endif
    initializeScripts();
}

MailWebEngineView::~MailWebEngineView()
{
    delete d;
}

void MailWebEngineView::initializeScripts()
{
    initializeJQueryScript();
    const QString scripts = MessageViewer::MailWebEngineScript::manageShowHideAttachments() +
                            MessageViewer::MailWebEngineScript::manageExpandAddresses(QStringLiteral("To")) +
                            MessageViewer::MailWebEngineScript::manageExpandAddresses(QStringLiteral("Cc"));
    addScript(scripts, QStringLiteral("emailmanagement"), QWebEngineScript::DocumentReady);
}

void MailWebEngineView::contextMenuEvent(QContextMenuEvent *e)
{
    WebEngineViewer::WebHitTest *webHit = d->mPageEngine->hitTestContent(e->pos());
    connect(webHit, &WebEngineViewer::WebHitTest::finished, this, &MailWebEngineView::slotWebHitFinished);
}

void MailWebEngineView::slotWebHitFinished(const WebEngineViewer::WebHitTestResult &result)
{
    Q_EMIT popupMenu(result);
}

void MailWebEngineView::scrollUp(int pixels)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollUp(pixels));
}

void MailWebEngineView::scrollDown(int pixels)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollDown(pixels));
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
    Q_UNUSED(event);
    //qDebug()<<" void MailWebEngineView::forwardMousePressEvent(QMouseEvent *event)";
}

void MailWebEngineView::forwardMouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug() << "void MailWebEngineView::forwardMouseMoveEvent(QMouseEvent *event)";
}

void MailWebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    //qDebug() << "void MailWebEngineView::forwardMouseReleaseEvent(QMouseEvent *event)";
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

bool MailWebEngineView::isScrolledToBottom() const
{
    qDebug() << "bool MailWebEngineView::isScrolledToBottom() const not implemented";
    //Convert as async
    //TODO '(window.innerHeight + window.scrollY) >= document.body.offsetHeight)'
    return false;
}

void MailWebEngineView::setElementByIdVisible(const QString &id, bool visible)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::setElementByIdVisible(id, visible));
}

void MailWebEngineView::removeAttachmentMarking(const QString &id)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::removeStyleToElement(QLatin1String("*#") + id));
}

void MailWebEngineView::markAttachment(const QString &id, const QString &style)
{
    //TODO verify "*#" + id
    page()->runJavaScript(WebEngineViewer::WebEngineScript::setStyleToElement(QLatin1String("*#") + id, style));
}

void MailWebEngineView::scrollToAnchor(const QString &anchor)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::searchElementPosition(anchor), invoke(this, &MailWebEngineView::handleScrollToAnchor));
}

void MailWebEngineView::handleScrollToAnchor(const QVariant &result)
{
    if (result.isValid()) {
        const QList<QVariant> lst = result.toList();
        if (lst.count() == 2) {
            const QPoint pos(lst.at(0).toInt(), lst.at(1).toInt());
            page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollToPosition(pos));
        }
    }
}

void MailWebEngineView::scrollPageDown(int percent)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollPercentage(percent));
}

void MailWebEngineView::scrollPageUp(int percent)
{
    scrollPageDown(-percent);
}

void MailWebEngineView::executeCustomRenderingScripts()
{
    const QString scripts = MessageViewer::MailWebEngineScript::manageShowHideAttachments() +
                            MessageViewer::MailWebEngineScript::manageExpandAddresses(QStringLiteral("To")) +
                            MessageViewer::MailWebEngineScript::manageExpandAddresses(QStringLiteral("Cc"));
    page()->runJavaScript(scripts);
}

void MailWebEngineView::injectAttachments(const boost::function<QString()> &delayedHtml)
{
    const QString html = delayedHtml();
    if (html.isEmpty()) {
        return;
    }
    page()->runJavaScript(MessageViewer::MailWebEngineScript::injectAttachments(html, QStringLiteral("attachmentInjectionPoint")));
}

void MailWebEngineView::toggleFullAddressList(const QString &field, const boost::function<QString()> &delayedHtml)
{
    const QString html = delayedHtml();
    if (html.isEmpty()) {
        return;
    }

    //qDebug() << "void MailWebEngineView::toggleFullAddressList(const QString &field, const boost::function<QString()> &delayedHtml, bool doShow)" << html << " fields " << field;
    page()->runJavaScript(MessageViewer::MailWebEngineScript::replaceInnerHtml(field, html));
}

bool MailWebEngineView::hasVerticalScrollBar() const
{
    qDebug() << "bool MailWebEngineView::hasVerticalScrollBar() const not implemented";
    //TODO
    return false;
}

bool MailWebEngineView::isAttachmentInjectionPoint(const QPoint &globalPos) const
{
    Q_UNUSED(globalPos);
    qDebug() << "bool MailWebEngineView::isAttachmentInjectionPoint(const QPoint &globalPos) const not implemented";
    //TODO
    return false;
}

void MailWebEngineView::scrollToRelativePosition(qreal pos)
{
    page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollToRelativePosition(pos));
}

QUrl MailWebEngineView::linkOrImageUrlAt(const QPoint &global) const
{
    Q_UNUSED(global);
    //TODO
    return {};
}

void MailWebEngineView::setAllowExternalContent(bool b)
{
    if (d->mExternalReference->allowExternalContent() != b) {
        d->mExternalReference->setAllowExternalContent(b);
        reload();
    }
}

QList<QAction *> MailWebEngineView::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    return d->mNetworkAccessManager->interceptorUrlActions(result);
}

void MailWebEngineView::slotLoadFinished()
{
    scamCheck();
}

void MailWebEngineView::dragEnterEvent(QDragEnterEvent *event)
{
    Q_UNUSED(event);
    qDebug() << " void MailWebEngineView::dragEnterEvent(QDragEnterEvent *event)";
}

void MailWebEngineView::dragMoveEvent(QDragMoveEvent *e)
{
    qDebug() << " void MailWebEngineView::dragMoveEvent(QDragMoveEvent *e)";
    WebEngineViewer::WebEngineView::dragMoveEvent(e);
    //TODO
}

void MailWebEngineView::dropEvent(QDropEvent *e)
{
    Q_UNUSED(e);
    qDebug() << " MailWebEngineView::dropEvent";
}

void MailWebEngineView::setPrintElementBackground(bool printElementBackground)
{
    d->mPageEngine->setPrintElementBackground(printElementBackground);
}
