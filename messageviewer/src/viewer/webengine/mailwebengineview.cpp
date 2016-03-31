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

#include "mailwebengineview.h"
#include "mailwebenginepage.h"
#include "mailwebengineaccesskey.h"
#include "webengine/webenginescript.h"
#include "messageviewer/messageviewersettings.h"
#include "webengine/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.h"
#include <MessageViewer/NetworkAccessManagerWebEngine>

#include "scamdetection/scamdetectionwebengine.h"
#include "scamdetection/scamcheckshorturl.h"
#include <QContextMenuEvent>
#include <MessageViewer/WebHitTest>
#include <MessageViewer/WebHitTestResult>

#include <QWebEngineSettings>

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
    MailWebEngineAccessKey *mWebViewAccessKey;
    MessageViewer::LoadExternalReferencesUrlInterceptor *mExternalReference;
    MailWebEnginePage *mPageEngine;
    MessageViewer::NetworkAccessManagerWebEngine *mNetworkAccessManager;
};

MailWebEngineView::MailWebEngineView(KActionCollection *ac, QWidget *parent)
    : MessageViewer::WebEngineView(parent),
      d(new MessageViewer::MailWebEngineViewPrivate)

{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    d->mWebViewAccessKey = new MailWebEngineAccessKey(this, this);
    d->mWebViewAccessKey->setActionCollection(ac);
    d->mScamDetection = new ScamDetectionWebEngine(this);
    connect(d->mScamDetection, &ScamDetectionWebEngine::messageMayBeAScam, this, &MailWebEngineView::messageMayBeAScam);
    connect(d->mWebViewAccessKey, &MailWebEngineAccessKey::openUrl, this, &MailWebEngineView::openUrl);
    connect(this, &MailWebEngineView::loadFinished, this, &MailWebEngineView::slotLoadFinished);

    d->mNetworkAccessManager = new MessageViewer::NetworkAccessManagerWebEngine(this, ac, this);
    d->mExternalReference = new MessageViewer::LoadExternalReferencesUrlInterceptor(this);
    d->mNetworkAccessManager->addInterceptor(d->mExternalReference);
    d->mPageEngine = new MailWebEnginePage(this);
    setPage(d->mPageEngine);

    setFocusPolicy(Qt::WheelFocus);
    connect(d->mPageEngine, &MailWebEnginePage::urlClicked, this, &MailWebEngineView::openUrl);
    //TODO need info about scrolling
}

MailWebEngineView::~MailWebEngineView()
{
    delete d;
}

void MailWebEngineView::contextMenuEvent(QContextMenuEvent *e)
{
    MessageViewer::WebHitTest *webHit = d->mPageEngine->hitTestContent(e->pos());
    connect(webHit, &MessageViewer::WebHitTest::finished, this, &MailWebEngineView::slotWebHitFinished);
}

void MailWebEngineView::slotWebHitFinished(const MessageViewer::WebHitTestResult &result)
{
    Q_EMIT popupMenu(result);
}

void MailWebEngineView::scrollUp(int pixels)
{
    page()->runJavaScript(MessageViewer::WebEngineScript::scrollUp(pixels));
}

void MailWebEngineView::scrollDown(int pixels)
{
    page()->runJavaScript(MessageViewer::WebEngineScript::scrollDown(pixels));
}

void MailWebEngineView::selectAll()
{
    page()->triggerAction(QWebEnginePage::SelectAll);
}

void MailWebEngineView::slotZoomChanged(qreal zoom)
{
    qDebug() << " void MailWebEngineView::slotZoomChanged(qreal zoom)*******" << zoom;
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

bool MailWebEngineView::isScrolledToBottom() const
{
    //Convert as async
    //TODO '(window.innerHeight + window.scrollY) >= document.body.offsetHeight)'
    return false;
}

void MailWebEngineView::setElementByIdVisible(const QString &id, bool visible)
{
    page()->runJavaScript(MessageViewer::WebEngineScript::setElementByIdVisible(id, visible));
}

bool MailWebEngineView::removeAttachmentMarking(const QString &id)
{
#if 0
    QWebElement doc = page()->mainFrame()->documentElement();
    QWebElement attachmentDiv = doc.findFirst(QLatin1String("*#") + id);
    if (attachmentDiv.isNull()) {
        return false;
    }
    attachmentDiv.removeAttribute(QStringLiteral("style"));
#endif
    return true;
}

void MailWebEngineView::markAttachment(const QString &id, const QString &style)
{
    //TODO verify "*#" + id
    page()->runJavaScript(MessageViewer::WebEngineScript::setStyleToElement(QLatin1String("*#") + id, style));
}

void MailWebEngineView::scrollToAnchor(const QString &anchor)
{
    page()->runJavaScript(MessageViewer::WebEngineScript::searchElementPosition(anchor), invoke(this, &MailWebEngineView::handleScrollToAnchor));
}

void MailWebEngineView::handleScrollToAnchor(const QVariant &result)
{
    if (result.isValid()) {
        const QList<QVariant> lst = result.toList();
        if (lst.count() == 2) {
            const QPoint pos(lst.at(0).toInt(), lst.at(1).toInt());
            page()->runJavaScript(MessageViewer::WebEngineScript::scrollToPosition(pos));
        }
    }
}

void MailWebEngineView::scrollPageDown(int percent)
{
#if 0
    const qint64 height =  page()->viewportSize().height();
    const qint64 current = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    // do arithmetic in higher precision, and check for overflow:
    const qint64 newPosition = current + height * percent / 100;
    if (newPosition > std::numeric_limits<int>::max()) {
        qCWarning(MESSAGEVIEWER_LOG) << "new position" << newPosition << "exceeds range of 'int'!";
    }
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, newPosition);
#endif
}

void MailWebEngineView::scrollPageUp(int percent)
{
    scrollPageDown(-percent);
}

void MailWebEngineView::injectAttachments(const boost::function<QString()> &delayedHtml)
{
    //TODO
#if 0
    QWebElement doc = page()->currentFrame()->documentElement();
    QWebElement injectionPoint = doc.findFirst(QStringLiteral("*#attachmentInjectionPoint"));
    if (injectionPoint.isNull()) {
        return;
    }

    const QString html = delayedHtml();
    if (html.isEmpty()) {
        return;
    }

    assert(injectionPoint.tagName().toLower() == QLatin1String("div"));
    injectionPoint.setInnerXml(html)
#endif
}

bool MailWebEngineView::replaceInnerHtml(const QString &id, const boost::function<QString()> &delayedHtml)
{
#if 0
    QWebElement doc = page()->currentFrame()->documentElement();
    QWebElement tag = doc.findFirst(QLatin1String("*#") + id);
    if (tag.isNull()) {
        return false;
    }
    tag.setInnerXml(delayedHtml());
    return true;
#endif
    //TODO
    return false;
}

bool MailWebEngineView::hasVerticalScrollBar() const
{
    //TODO
    return false;
}

bool MailWebEngineView::isAttachmentInjectionPoint(const QPoint &globalPos) const
{
    //TODO
    return false;
}

void MailWebEngineView::scrollToRelativePosition(double pos)
{
    page()->runJavaScript(MessageViewer::WebEngineScript::scrollToRelativePosition(pos));
}

double MailWebEngineView::relativePosition() const
{

    //TODO
    return {};
}

QUrl MailWebEngineView::linkOrImageUrlAt(const QPoint &global) const
{
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

QList<QAction *> MailWebEngineView::interceptorUrlActions() const
{
    return d->mNetworkAccessManager->actions();
}

void MailWebEngineView::slotLoadFinished()
{
    scamCheck();
}
