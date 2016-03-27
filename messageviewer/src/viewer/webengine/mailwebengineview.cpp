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
#include <MessageViewer/NetworkAccessManagerWebEngine>

#include "scamdetection/scamdetectionwebengine.h"
#include "scamdetection/scamcheckshorturl.h"

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
        : mWebViewAccessKey(Q_NULLPTR)
    {

    }
    ScamDetectionWebEngine *mScamDetection;
    MailWebEngineAccessKey *mWebViewAccessKey;
};

MailWebEngineView::MailWebEngineView(KActionCollection *ac, QWidget *parent)
    : MessageViewer::WebEngineView(parent),
      d(new MessageViewer::MailWebEngineViewPrivate)

{
    d->mWebViewAccessKey = new MailWebEngineAccessKey(this, this);
    d->mWebViewAccessKey->setActionCollection(ac);
    d->mScamDetection = new ScamDetectionWebEngine(this);
    connect(d->mWebViewAccessKey, &MailWebEngineAccessKey::openUrl, this, &MailWebEngineView::openUrl);

    new MessageViewer::NetworkAccessManagerWebEngine(this, ac, this);
    MailWebEnginePage *pageEngine = new MailWebEnginePage(this);
    setPage(pageEngine);

    setFocusPolicy(Qt::WheelFocus);
    connect(pageEngine, &MailWebEnginePage::urlClicked, this, &MailWebEngineView::openUrl);
    //TODO need info about scrolling
}

MailWebEngineView::~MailWebEngineView()
{
    delete d;
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
